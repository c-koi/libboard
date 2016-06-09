#  LibBoard CMake file
#  Contribution of Jason Gauci, University of Central Florida, http://eplex.cs.ucf.edu/
#  Modified by Guillaume Nee, GREYC, http://www.greyc.fr

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

Project(LibBoard VERSION 0.9.4)

ADD_DEFINITIONS(-D_USE_MATH_DEFINES)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
    set(CMAKE_CXX_STANDARD 11)
endif()

find_package(ImageMagick COMPONENTS Magick++)
IF ( ImageMagick_Magick++_FOUND )
  SET( Board_Have_MagickPlusPlus 1 )
  INCLUDE_DIRECTORIES( ${ImageMagick_INCLUDE_DIRS} )
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
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wno-unused-variable")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -Wno-unused-variable")

  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} -pg")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_DEBUG} -pg")

  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -g")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -g")
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
  src/Board.cpp
  src/Color.cpp
  src/Rect.cpp
  src/Path.cpp
  src/Shapes.cpp
  src/Image.cpp
  src/ShapeList.cpp
  src/ShapeVisitor.cpp
  src/Transforms.cpp
  src/TransformMatrix.cpp
  src/Tools.cpp
  src/PathBoundaries.cpp
  src/PSFonts.cpp
  src/Point.cpp

  include/Board.h
  include/board/Color.h
  include/board/Image.h
  include/board/PSFonts.h
  include/board/Path.h
  include/board/Point.h
  include/board/Rect.h
  include/board/ShapeList.h
  include/board/ShapeVisitor.h
  include/board/Shapes.h
  include/board/Tools.h
  include/board/PathBoundaries.h
  include/board/TransformMatrix.h
  include/board/Transforms.h
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

FOREACH( EXAMPLE logo example1 example2 example3 example4
    arithmetic ellipse graph arrows  ruler koch clipping
    flag scale_ellipse line_style images line_segment tilings stroke_path )
  ADD_EXECUTABLE(
    ${EXAMPLE}
    examples/${EXAMPLE}.cpp
    )
  TARGET_LINK_LIBRARIES(
    ${EXAMPLE}
    debug board_d
    optimized board
    )
  TARGET_LINK_LIBRARIES(
   ${EXAMPLE}
   ${ImageMagick_LIBRARIES}
  )
  SET_TARGET_PROPERTIES(${EXAMPLE} PROPERTIES DEBUG_POSTFIX _d)
ENDFOREACH(EXAMPLE)
