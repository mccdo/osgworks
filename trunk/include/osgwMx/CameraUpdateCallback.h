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

#ifndef __OSGWMX_CAMERA_UPDATE_CALLBACK_H__
#define __OSGWMX_CAMERA_UPDATE_CALLBACK_H__ 1


#include <osgwMx/Export.h>
#include <osgwMx/MxCore.h>

#include <osg/NodeCallback>


namespace osgwMx
{


/** \class CameraUpdateCallback CameraUpdateCallback.h <osgwMx/CameraUpdateCallback.h>
\brief A Camera update callback that modifies both the view and projection matrices.
*/
class OSGWMX_EXPORT CameraUpdateCallback : public osg::NodeCallback
{
public:
    CameraUpdateCallback( MxCore* viewingCore=NULL );
    CameraUpdateCallback( const CameraUpdateCallback& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    META_Object(osgwMx,CameraUpdateCallback);

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
    ~CameraUpdateCallback();

    bool _firstUpdate;

    osg::ref_ptr< MxCore > _mxCore;
};


// osgwMx
}


// __OSGWMX_CAMERA_UPDATE_CALLBACK_H__
#endif
