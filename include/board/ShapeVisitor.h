/* -*- mode: c++ -*- */
/**
 * @file   ShapeVisitor.h
 * @author Sebastien Fourey (GREYC)
 * @date   June 2016
 *
 * @brief
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _BOARD_SHAPE_VISITOR_H_
#define _BOARD_SHAPE_VISITOR_H_

#if __cplusplus<201100
#define override
#endif
#include <cstddef> // For std::size_t

namespace LibBoard {

  struct Shape;
  struct ShapeList;
  struct Board;

  struct ShapeVisitor {
    virtual void visit( Shape & shape ) = 0;
    virtual void visit( Shape & shape ) const = 0;
  };

  struct ConstShapeVisitor {
    virtual void visit( const Shape & shape ) = 0;
    virtual void visit( const Shape & shape ) const = 0;
  };

  /**
   * @brief The BoundingBoxExtractor struct may be used to compute the
   *        bounding box of a composite shape tree.
   */
  struct BoundingBoxExtractor : public ConstShapeVisitor {
    BoundingBoxExtractor( ShapeList & );
    void visit( const Shape & ) override;
    void visit( const Shape & ) const override;
    const ShapeList & shapeList() const;
  private:
    ShapeList & _shapeList;
  };

  /**
   * @brief The BoundingBoxViewer struct may be used to display the bounding
   *        boxes of the shapes in a composite tree.
   */
  struct BoundingBoxViewer : public ConstShapeVisitor {
    void visit( const Shape & ) override;
    void visit( const Shape & ) const override;
  };

  /**
   * @brief The ShapeCounter struct may be used to count shapes in
   *        the composite shapes tree.
   */
  struct ShapeCounter : public ConstShapeVisitor {
    ShapeCounter();
    void clear();
    std::size_t value() const;
    void visit( const Shape & ) override;
    void visit( const Shape & ) const override;
  private:
    std::size_t _count;
  };

}

#if __cplusplus<201100
#undef override
#endif

#endif /* _BOARD_SHAPE_VISITOR_H_ */
