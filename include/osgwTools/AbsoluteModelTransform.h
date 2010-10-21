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

#ifndef __OSGWORKS_ABSOLUTE_MODEL_TRANSFORM_H__
#define __OSGWORKS_ABSOLUTE_MODEL_TRANSFORM_H__


#include <osg/NodeVisitor>
#include <osg/Transform>
#include <osgwTools/Export.h>

#include <string>


namespace osgwTools {


/*!
   AbsoluteModelTransform -- An OSG Transform that overloads ABSOLUTE_RF to
   preserve the view. Regardless of (non-Camera) transforms above this node in
   the hierarchy, the effective transform will be this transform concatenated
   with the view. This allows Bullet to drive a model's transform with a single
   matrix.

   This behavior can be disabled by setting the reference frame to RELATIVE_RF,
   which causes this Transform to behave like a regular MatrixTransform.

   The default reference frame is ABSOLUTE_RF.
*/
class OSGWTOOLS_EXPORT AbsoluteModelTransform : public osg::Transform
{
public:
    AbsoluteModelTransform();
    AbsoluteModelTransform( const osg::Matrix& m );
    AbsoluteModelTransform( const AbsoluteModelTransform& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );

    META_Node( osgwTools, AbsoluteModelTransform );

    virtual bool computeLocalToWorldMatrix( osg::Matrix& matrix, osg::NodeVisitor* nv ) const;
    virtual bool computeWorldToLocalMatrix( osg::Matrix& matrix, osg::NodeVisitor* nv ) const;

    inline void setMatrix( const osg::Matrix& m ) { _matrix = m; dirtyBound(); }
    inline const osg::Matrix& getMatrix() const { return _matrix; }


protected:
    virtual ~AbsoluteModelTransform();

    osg::Matrix _matrix;
};

// namespace osgwTools
}

// __OSGWORKS_ABSOLUTE_MODEL_TRANSFORM_H__
#endif
