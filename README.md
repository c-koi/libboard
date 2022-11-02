# Libboard: a vector graphics C++ library

<img src="https://foureys.users.greyc.fr/board/doc/LibBoardLogoII_Small.png">

## The LibBoard C++ library

#### Purpose

The LibBoard C++ library allows the drawing of Postscript, SVG, and FIG (XFig)
vector graphics using the C++ programming language.

It is suitable for adding SVG/EPS/FIG output support to a program; but also for
drawing graphics that involve a large (huge?) number of primitives. In the
latter case, one can indeed use loops, variables, functions and all C++
features. Produced files may afterwards be edited using dedicated tools.

### Features


For now, LibBoard can handle primitives like lines, rectangles, triangles, polylines, circles, ellipses, bitmap images and text. See the [TODO](TODO) file for a list of features that should be added in future releases.

### Documentation

 * See the Doxygen-made [documentation](https://foureys.users.greyc.fr/board/doc/0.9.6/).

 * [Sample codes](EXAMPLES.md) with their output.

### Licence

Starting with version 0.9.0, LibBoard is distributed under the term of the [GNU Lesser General Public License](LICENSE).

#### Citing LibBoard

LibBoard: A vector graphics C++ library (Version 0.9.6). GREYC laboratory. [Software]. https://github.com/c-koi/libboard. [accessed Sept. 2015].
```
    @misc{LibBoard,
      key = {{LibBoard}},
      title = {{LibBoard: A vector graphics C++ library (Version 0.9.6). [Software]. GREYC Laboratory}},
      howpublished = {{https://github.com/c-koi/libboard}},
      note = {{[accessed June 2016]}},
      year = {2016},
    }
```
### Download

  * You may get the lastest source code from the Git repository with the command below:

```
git clone https://github.com/c-koi/libboard.git
```

  * You may also consider [downloading an archive of the latest release](https://github.com/c-koi/libboard/releases/latest).

### Author

  * Sébastien Fourey (foureys.users.greyc.fr).
