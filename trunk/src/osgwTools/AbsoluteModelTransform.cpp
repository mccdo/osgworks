/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009 by Kenneth Mark Bryden
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

#include <osgwTools/AbsoluteModelTransform.h>
#include <osgUtil/CullVisitor>
#include <osg/NodeVisitor>
#include <osg/Transform>
#include <osg/Matrix>

#include <string>
#include <osg/io_utils>


namespace osgwTools
{


AbsoluteModelTransform::AbsoluteModelTransform()
{
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );
}
AbsoluteModelTransform::AbsoluteModelTransform( const osg::Matrix& m )
  : _matrix( m )
{
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );
}
AbsoluteModelTransform::AbsoluteModelTransform( const AbsoluteModelTransform& rhs, const osg::CopyOp& copyop )
  : osg::Transform( rhs, copyop ),
    _matrix( rhs._matrix )
{
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );
}
AbsoluteModelTransform::~AbsoluteModelTransform()
{
}


bool
AbsoluteModelTransform::computeLocalToWorldMatrix( osg::Matrix& matrix, osg::NodeVisitor* nv ) const
{
    if( getReferenceFrame() == osg::Transform::ABSOLUTE_RF )
    {
        osg::Matrix view;
        if( !nv )
            osg::notify( osg::INFO ) << "AbsoluteModelTransform: NULL NodeVisitor; can't get view." << std::endl;
        else if( nv->getVisitorType() != osg::NodeVisitor::CULL_VISITOR )
            osg::notify( osg::INFO ) << "AbsoluteModelTransform: NodeVisitor is not CullVisitor; can't get view." << std::endl;
        else
        {
            osgUtil::CullVisitor* cv = dynamic_cast< osgUtil::CullVisitor* >( nv );
            osg::Camera* cam = cv->getCurrentCamera();
            cam->computeLocalToWorldMatrix( view, cv );
        }
        matrix = ( _matrix * view );
    }
    else
        // RELATIVE_RF
        matrix.preMult(_matrix);

    return( true );
}

bool
AbsoluteModelTransform::computeWorldToLocalMatrix( osg::Matrix& matrix, osg::NodeVisitor* nv ) const
{
    osg::Matrix inv( osg::Matrix::inverse( _matrix ) );
    if( getReferenceFrame() == osg::Transform::ABSOLUTE_RF )
    {
        osg::Matrix invView;
        if( !nv )
            osg::notify( osg::NOTICE ) << "AbsoluteModelTransform: NULL NodeVisitor; can't get invView." << std::endl;
        else if( nv->getVisitorType() != osg::NodeVisitor::CULL_VISITOR )
            osg::notify( osg::NOTICE ) << "AbsoluteModelTransform: NodeVisitor is not CullVisitor; can't get invView." << std::endl;
        else
        {
            osgUtil::CullVisitor* cv = dynamic_cast< osgUtil::CullVisitor* >( nv );
            osg::Camera* cam = cv->getCurrentCamera();
            cam->computeWorldToLocalMatrix( invView, cv );
        }
        matrix = ( invView * inv );
    }
    else
        // RELATIVE_RF
        matrix.postMult( inv );

    return( true );
}


// namespace osgwTools
}
