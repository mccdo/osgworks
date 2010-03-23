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


// This is a set of convenience routines for configuring a Geometry to render
// specific shapes. It is intended as a replacement for OSG's ShapeDrawables.
// While it lacks the implicit metadata found in ShapeDrawables, it allows direct
// access to vertices and vertex data, thus moe widely supported than ShapeDrawables.
//
// All convenience routines follow a common convention of allowint the app to
// supply a Geometry to use. By default, the convenience routines instantiate a
// Geometry. All routines set a color array with a single white color.
//
// All shapes are centered on the origin. Use OSG Transform Nodes to translate
// or orient the shapes.


// Sphere generation routines

// Creates a geodesic sphere starting from the 20 triangles of an icosehedron.
// Each subdivision splits a triangle into four new triangles. The generated
// Geometry uses a single DrawElementsUShort GL_TRIANGLES PrimitiveSet, and contains
// vertex, normal, texture coordinate, and color data. The texture coordinates are
// configured for unit 0 and suitable for cube mapping.
OSGWTOOLS_EXPORT osg::Geometry* makeGeodesicSphere( const float radius=1., const unsigned int subdivisions=2, osg::Geometry* geometry=NULL );

// Creates an alt-az sphere (also commonly called a lat-long sphere) using
// GL_TRIANGLE_STRIP PrimitiveSets. subLat indicates the number of GL_TRIANGLE_STRIPS
// wrapping the sphere in the xy plane. Each strip contains (subLong*2) triangles.
// Normal and texture coordinate (unit 0) data are created. Texture coordinates are
// suitable for applying a geographic projection texture. (t coordinate ranges from 0
// at the (-z) sphere bottom to 1 at the (+z) sphere top, and s wraps around the sphere
// with 0 at +x, to +y, to -x, to -y, and ending with 1.0 at +x.)
OSGWTOOLS_EXPORT osg::Geometry* makeAltAzSphere( const float radius=1., const unsigned int subLat=8, const unsigned int subLong=16, osg::Geometry* geometry=NULL );

// Creates an alt-az sphere using GL_LINE_LOOP and GL_LINE_STRIP PrimitiveSets.
// No normal or texture coordinate data. StateSet is configured to disable GL_LIGHTING
// for OpenGL1/2 builds of OSG.
OSGWTOOLS_EXPORT osg::Geometry* makeWireAltAzSphere( const float radius=1., const unsigned int subLat=8, const unsigned int subLong=16, osg::Geometry* geometry=NULL );


// Box generation routines

// Makes an axis-aligned box using GL_TRIANGLE_STRIP PrimitiveSets. Dimensions
// are (halfExtents*2). By default, two triangles are on each side of the box, for
// a total of 12 triangles. However, this can be increased with the subdivisions
// parameter, which specifices how to subdivide the faces of the sphere in x, y, and z.
// Normal and texture coordinate (unit 0) data are created. The texture coordinate data
// is suitable for applying an entire texture map to each face of the box.
OSGWTOOLS_EXPORT osg::Geometry* makeBox( const osg::Vec3& halfExtents, const osg::Vec3s& subdivisions=osg::Vec3s(1,1,1), osg::Geometry* geometry=NULL );

// Creates a bos using GL_LINE_LOOP and GL_LINES PrimitiveSets. No normal or
// texture coordinate data. StateSet is configured to disable GL_LIGHTING for
// OpenGL1/2 builds of OSG.
OSGWTOOLS_EXPORT osg::Geometry* makeWireBox( const osg::Vec3& halfExtents, osg::Geometry* geometry=NULL );


// Arrow generation routines

// Makes a unit-length arrow pointing towards +z with base at the origin.
// Current implementation uses a minimal number of vertices. In the future,
// this code could use cylinder/cone support routines to provide complete
// control over approximation.
// Adds vertex and normal data, with a single white color.
OSGWTOOLS_EXPORT osg::Geometry* makeArrow( osg::Geometry* geometry=NULL );



// namespace osgwTools
}

// __OSGWTOOLS_SHAPES_H__
#endif
