/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2010 by Kenneth Mark Bryden
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

#ifndef __OSGWTOOLS_SHAPES_H__
#define __OSGWTOOLS_SHAPES_H__ 1

#include "osgwTools/Export.h"
#include <osg/Geometry>



namespace osgwTools {


OSGWTOOLS_EXPORT osg::Geometry* makeGeodesicSphere( const float radius=1., const unsigned int subdivisions=2, osg::Geometry* geometry=NULL );
OSGWTOOLS_EXPORT osg::Geometry* makeAltAzSphere( const float radius=1., const unsigned int subLat=8, const unsigned int subLong=16, osg::Geometry* geometry=NULL );


// namespace osgwTools
}

// __OSGWTOOLS_SHAPES_H__
#endif
