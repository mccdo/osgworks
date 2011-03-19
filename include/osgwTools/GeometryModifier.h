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

#ifndef __GEOMETRY_MODIFIER_H__
#define __GEOMETRY_MODIFIER_H__

#include <osgwTools/Export.h>
#include <osgwTools/GeometryOperation.h>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osgwTools/Version.h>
#include <ostream>

namespace osgwTools {


/** Convenience NodeVisitor to support finding and performing operations
on osg::Geometry objects. Use with classes derived from GeometryOperation
to perform various tasks.
*/
class OSGWTOOLS_EXPORT GeometryModifier : public osg::NodeVisitor
{
public:
    GeometryModifier();
    GeometryModifier( GeometryOperation* geomOp );
    ~GeometryModifier();

#if( OSGWORKS_OSG_VERSION >= 20800 )
    META_NodeVisitor(osgBullet,GeometryModifier)
#endif

    void setGeometryOperation( GeometryOperation* geomOp ) { _geomOp = geomOp; }
    GeometryOperation* getGeometryOperation() { return( _geomOp.get() ); }
    const GeometryOperation* getGeometryOperation() const { return( _geomOp.get() ); }

    void reset();

    void displayStatistics( std::ostream& ostr ) const;

    virtual void apply( osg::Geode& geode );

protected:
    void incStatistics( const osg::Geometry* geom, unsigned int& vert, unsigned int& ind, unsigned int& prim );
    osg::ref_ptr< GeometryOperation > _geomOp;

    unsigned int _drawableCount;
    unsigned int _geometryCount;

    unsigned int _preVertices, _preIndices, _prePrimitives;
    unsigned int _postVertices, _postIndices, _postPrimitives;
};

}

#endif