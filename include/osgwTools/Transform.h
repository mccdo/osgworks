/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2011 by Kenneth Mark Bryden
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#ifndef __OSGWTOOLS_TRANSFORM_H__
#define __OSGWTOOLS_TRANSFORM_H__1


#include "osgwTools/Export.h"

#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Matrix>


namespace osgwTools
{


// Transform a bounding sphere or box by a matrix, and return a copy
// of the transformed sphere or matrix.
OSGWTOOLS_EXPORT osg::BoundingSphere transform( const osg::Matrix& m, const osg::BoundingSphere& sphere );
OSGWTOOLS_EXPORT osg::BoundingBox transform( const osg::Matrix& m, const osg::BoundingBox& box );



// osgwTools
}

// __OSGWTOOLS_GRID_H__
#endif
