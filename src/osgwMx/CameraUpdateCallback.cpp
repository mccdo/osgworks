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

#include <osgwMx/CameraUpdateCallback.h>
#include <osgwMx/MxCore.h>

#include <osg/Camera>


namespace osgwMx
{


CameraUpdateCallback::CameraUpdateCallback( MxCore* viewingCore )
  : _firstUpdate( true ),
    _mxCore( viewingCore )
{
}
CameraUpdateCallback::CameraUpdateCallback( const CameraUpdateCallback& rhs, const osg::CopyOp& copyop )
  : osg::NodeCallback( rhs, copyop ),
    _firstUpdate( rhs._firstUpdate ),
    _mxCore( rhs._mxCore )
{
}
CameraUpdateCallback::~CameraUpdateCallback()
{
}

void
CameraUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( _mxCore == NULL )
    {
        osg::notify( osg::WARN ) << "CameraUpdateCallback::operator(): _mxCore is NULL." << std::endl;
        return;
    }

    if( _firstUpdate )
    {
        _mxCore->computeInitialView();
        _firstUpdate = false;
    }

    osg::Camera* cam = static_cast< osg::Camera* >( node );

    // Update the aspect ratio before we query the projection matrix.
    const osg::Viewport* vp = cam->getViewport();
    _mxCore->setAspect( vp->width() / vp->height() );

    cam->setViewMatrix( _mxCore->getInverseMatrix() );
    cam->setProjectionMatrix( _mxCore->computeProjection() );

    traverse( node, nv );
}


// osgwMx
}
