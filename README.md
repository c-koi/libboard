# Libboard: a vector graphics C++ library

<img src="https://foureys.users.greyc.fr/board/doc/LibBoardLogoII_Small.png">

## The LibBoard C++ library

(Motivation for this piece of code) There are more people on this planet who are fluent in, or at least know as beginners, the C++ language than there are people who know all together the Postscript, SVG, and FIG (XFig) file formats. Actually Postscript is also a programming language, but that's part of another story.

The next point is: how can one draw a vector graphic with his favorite drawing software when this graphic involves a lot, say a huge amount of primitives, especially when the graphic would be easy to build using loops, variables, functions. Eventually, one may enjoy being able to code the drawing in C++ and then export it as an EPS file for inclusion in a LaTeX document, as an XFig file for further editing with Xfig, or as an SVG file that may be augmented/edited with any SVG drawing tool.

## Features

For now, LibBoard can handle primitives like lines, rectangles, triangles, polylines, circles, ellipses, bitmap images and text. See the [TODO](https://raw.githubusercontent.com/c-koi/libboard/master/TODO) file for a list of features that should be added in future releases.

## Documentation

 * See the Doxygen-made [documentation](https://foureys.users.greyc.fr/board/doc/).

 * [Sample codes](EXAMPLES.md) with their output.


## Licence

Starting with version 0.9.0, LibBoard is distributed under the term of the [GNU Lesser General Public License](https://github.com/c-koi/libboard/blob/master/LICENSE).

### Citing LibBoard

LibBoard: A vector graphics C++ library (Version 0.9.0). GREYC laboratory. [Software]. https://github.com/c-koi/libboard. [accessed Sept. 2015].
```
   @misc{LibBoard,
      key = {{LibBoard}},
      title = {{LibBoard: A vector graphics C++ library (Version 0.9.0). [Software]. GREYC Laboratory}},
      howpublished = {{https://github.com/c-koi/libboard}},
      note = {{[accessed Sept. 2014]}},
      year = {2014},
    }
```
## Download

   You may get the lastest source code from the Git repository with the command below:
```
   git clone https://github.com/c-koi/libboard.git
```

## Author

  * Sébastien Fourey (foureys.users.greyc.fr).
