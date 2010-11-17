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


/** \mainpage osgWorks Documentation

\section IntroSection Introduction

osgWorks is an open source SDK intended for OSG developers.
The project contains developer tools that are useful to developers
of most OSG applications, such as standalone applications,
transformation utilities, visitors, custom nodes, scene graph
editing facilities, basic geometric shapes, plugins, test models,
and example programs.

\section LibrariesSection Libraries

\subsection osgwTools osgwTools
\subsection osgwControls osgwTools

\section PluginsSection Plugins

\subsection PluginosgwTools osgdb_osgwTools
\subsection Pluginskeletom osgdb_skeleton

See the \ref Plugins module description.

\section AppsSection Applications
\li \ref osgwbvv visualizes OSG bounding volumes.
\li \ref osgwcomp compares the structure of two scene graphs.
\li \ref osgwnames displays the structure of a scene graph.
\li \ref osgwwhich locates OSG data files and shared libraries.

*/




/** \brief Set the model part of the OpenGL modelview matrix.

AbsoluteModelTransform is an OSG Transform that overloads ABSOLUTE_RF to
preserve the view. Regardless of (non-Camera) transforms above this node in
the hierarchy, the effective transform will be this transform concatenated
with the view. This allows Bullet to drive a model's transform with a single
matrix.

This behavior can be disabled by setting the reference frame to RELATIVE_RF,
which causes this Transform to behave like a regular MatrixTransform. The
default reference frame is ABSOLUTE_RF.

In order for AbsoluteModelTransform to operate, it needs the current view matrix.
The code supports obtaining the view matrix in two ways, controlled by the
setting of OSGWORKS_SCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT in CMake. By default,
OSGWORKS_SCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT is disabled, and AbsoluteModelTransform
assumes there is a Camera in the CullVisitor NodePath that contains the view matrix.
This is the most efficient method, but doesn't work in some corner-case usages, such
as when the user has enabled anaglyphic stereo via the OSG_STEREO environment variable.
Set OSGWORKS_SCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT on in CMake to support this usage. When
enabled, AbsoluteModelTransform obtains the view matrix by accumulating the model transform
from the NodePath, inverting it, and multiplying it by the CullVisitor's current modelview
matrix. This is a more general-purpose, but less efficient, solution.

\test amt

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
