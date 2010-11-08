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

#ifndef __GEOMETRY_OPERATION_H__
#define __GEOMETRY_OPERATION_H__

//#include <osgBullet/Export.h>
#include <osg/Object>
#include <osg/Geometry>
#include <osgwTools/Export.h>

namespace osgwTools {


/** Base class for performing operations on osg::Geometry objects in collaboration
with the GeometryModifier class.
*/
class OSGWTOOLS_EXPORT GeometryOperation : public osg::Object
{
public:
    GeometryOperation();

    /** Derived classes must override this method and return
     a pointer to a new Geometry object. */
    virtual osg::Geometry* operator()( osg::Geometry& geom ) = 0;

protected:
    virtual ~GeometryOperation();
};

}

#endif
