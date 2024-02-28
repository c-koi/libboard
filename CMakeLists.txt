#  LibBoard CMake file
#  Initial version by Jason Gauci, University of Central Florida, http://eplex.cs.ucf.edu/
#  Modified by Guillaume Nee, GREYC, http://www.greyc.fr, then by Sebastien Fourey

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

Project(LibBoard VERSION 0.9.6)

ADD_DEFINITIONS(-D_USE_MATH_DEFINES)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
  message(FATAL_ERROR "c++ compiler does not support C++11, which is required")
endif()

set(CMAKE_CXX_STANDARD 11)

find_package(ImageMagick COMPONENTS Magick++)
IF ( ImageMagick_Magick++_FOUND )
  SET( Board_Have_MagickPlusPlus 1 )
  INCLUDE_DIRECTORIES( ${ImageMagick_INCLUDE_DIRS} )
  ADD_DEFINITIONS( ${ImageMagick_COMPILE_OPTIONS} )
ELSE( ImageMagick_Magick++_FOUND )
  SET( Board_Have_MagickPlusPlus 0 )
ENDIF( ImageMagick_Magick++_FOUND )

IF ( WIN32 )
 SET( Board_Win32 1 )
ELSE ( WIN32 )
 SET( Board_Win32 0 )
ENDIF ( WIN32 )

configure_file (
  "${PROJECT_SOURCE_DIR}/include/BoardConfig.h.in"
  "${PROJECT_SOURCE_DIR}/include/BoardConfig.h"
)

IF(MSVC)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /RTCsu /W3")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /EHsc /RTCsu /W3")

  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /EHsc")

  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /EHsc")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /EHsc")

  SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /EHsc")
  SET(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /EHsc")
ELSE(MSVC)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DBOARD_DEBUG -Wall -Wno-unused-variable -Wno-unknown-pragmas")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DBOARD_DEBUG -Wall -Wno-unused-variable -Wno-unknown-pragmas")

  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} -pg")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_DEBUG} -pg")

  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unknown-pragmas")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wno-unknown-pragmas")
ENDIF(MSVC)

IF(NOT MYPROJ_RAN_ONCE)
  SET(MYPROJ_RAN_ONCE 1 CACHE INTERNAL "")

  SET(
    CMAKE_INSTALL_PREFIX
    "${CMAKE_SOURCE_DIR}/out/"
    CACHE
    FILEPATH
    "Install path prefix, prepended onto install directories."
    FORCE
    )

  SET(
    EXECUTABLE_OUTPUT_PATH
    "${CMAKE_SOURCE_DIR}/bin/"
    CACHE
    FILEPATH
    "Single output directory for building all executables."
    FORCE
    )

  SET(
    LIBRARY_OUTPUT_PATH
    "${CMAKE_SOURCE_DIR}/lib/"
    CACHE
    FILEPATH
    "Single output directory for building all libraries."
    FORCE
    )

  SET(
    CMAKE_BUILD_TYPE
    "Release"
    CACHE
    STRING
    "Description: Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
    FORCE
    )

ENDIF(NOT MYPROJ_RAN_ONCE)

INCLUDE_DIRECTORIES( include/ include/board )

LINK_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/lib/ )

SET(lib_src
  include/BoardConfig.h
  include/board/Group.h
  include/board/TransformMatrix.h
  include/board/Debug.h
  include/board/BoardFontText.h
  include/board/PSFonts.h
  include/board/Bezier.h
  include/board/Polyline.h
  include/board/Globals.h
  include/board/Rect.h
  include/board/SketchFilter.h
  include/board/Arrow.h
  include/board/Exception.h
  include/board/Color.h
  include/board/Types.h
  include/board/Transforms.h
  include/board/PathBoundaries.h
  include/board/Line.h
  include/board/Image.h
  include/board/Point.h
  include/board/Text.h
  include/board/Tools.h
  include/board/Ellipse.h
  include/board/Path.h
  include/board/Dot.h
  include/board/RoughVisitor.h
  include/board/Shape.h
  include/board/Style.h
  include/board/ShapeWithStyle.h
  include/board/ShapeVisitor.h
  include/board/ShapeList.h
  include/Board.h
  src/Arrow.cpp
  src/Ellipse.cpp
  src/BoardFontText.cpp
  src/Color.cpp
  src/ShapeWithStyle.cpp
  src/ShapeVisitor.cpp
  src/Text.cpp
  src/Exception.cpp
  src/Point.cpp
  src/Path.cpp
  src/Dot.cpp
  src/Bezier.cpp
  src/Shape.cpp
  src/ShapeList.cpp
  src/RoughVisitor.cpp
  src/Rect.cpp
  src/Line.cpp
  src/PSFonts.cpp
  src/SketchFilter.cpp
  src/Polyline.cpp
  src/Tools.cpp
  src/Style.cpp
  src/PathBoundaries.cpp
  src/Image.cpp
  src/Group.cpp
  src/inc/BoardFont.cpp
  src/TransformMatrix.cpp
  src/Transforms.cpp
  src/Board.cpp
)

ADD_LIBRARY(board STATIC ${lib_src})
SET_TARGET_PROPERTIES(board PROPERTIES DEBUG_POSTFIX _d)

ADD_LIBRARY(board-dynamic SHARED ${lib_src})
SET_TARGET_PROPERTIES(board-dynamic PROPERTIES OUTPUT_NAME "board")
SET_TARGET_PROPERTIES(board-dynamic PROPERTIES PREFIX "lib")

install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include/board/ DESTINATION include/board FILES_MATCHING PATTERN "*.h")
install(TARGETS board DESTINATION lib)
install(TARGETS board-dynamic DESTINATION lib)
install(DIRECTORY examples/ DESTINATION share/libboard/examples FILES_MATCHING PATTERN "*.cpp")
install(FILES README.md LICENSE AUTHORS DESTINATION share/libboard )

FILE(GLOB EXAMPLES examples/*.cpp)

FOREACH( EXAMPLE_SRC_PATH ${EXAMPLES} )
  get_filename_component(EXAMPLE_EXE ${EXAMPLE_SRC_PATH} NAME_WE)
  ADD_EXECUTABLE(
    ${EXAMPLE_EXE}
    examples/${EXAMPLE_EXE}.cpp
    )
  TARGET_LINK_LIBRARIES(
    ${EXAMPLE_EXE}
    debug board_d
    optimized board
    )
  TARGET_LINK_LIBRARIES(
   ${EXAMPLE_EXE}
   ${ImageMagick_LIBRARIES}
  )
  SET_TARGET_PROPERTIES(${EXAMPLE_EXE} PROPERTIES DEBUG_POSTFIX _d)
ENDFOREACH(EXAMPLE_SRC_PATH)
