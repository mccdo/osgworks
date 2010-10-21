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

#ifndef __REDUCER_OP_H__
#define __REDUCER_OP_H__


#include <osgwTools/Export.h>
#include <osgwTools/GeometryOperation.h>
#include <osg/CopyOp>
#include <osg/Object>
#include <osg/Vec3>

namespace osgwTools {


class OSGWTOOLS_EXPORT ReducerOp : public GeometryOperation
{
public:
    ReducerOp();
    ReducerOp( const ReducerOp& rhs, const osg::CopyOp& copyOp=osg::CopyOp::SHALLOW_COPY );

    META_Object(osgBullet,ReducerOp)

    virtual osg::Geometry* operator()( osg::Geometry& geom );

    void setGroupThreshold( float groupThreshold );
    float getGroupThreshold() const { return( _groupThreshold ); }

    void setMaxEdgeError( float maxEdgeError );
    float getMaxEdgeError() const { return( _maxEdgeError ); }

    void setRemoveDegenerateAndRedundantTriangles( bool remove ) { _removeDegenerateAndRedundantTriangles = remove; }
    bool getRemoveDegenerateAndRedundantTriangles() const { return( _removeDegenerateAndRedundantTriangles ); }

protected:
    ~ReducerOp();

    float _groupThreshold;
    float _groupThresholdRad;
    float _maxEdgeError;
    float _maxEdgeErrorRad;
    bool _removeDegenerateAndRedundantTriangles;



    struct Tri
    {
        Tri( int v0, int v1, int v2, const osg::Vec3Array* verts=NULL )
          : _v0(v0), _v1(v1), _v2(v2)
        {
            if( verts != NULL )
            {
                osg::Vec3 u( (*verts)[ v1 ] - (*verts)[ v0 ] );
                osg::Vec3 v( (*verts)[ v2 ] - (*verts)[ v0 ] );
                _norm = u ^ v;
                _norm.normalize();
            }
        }

        bool operator==( const Tri& rhs ) const
        {
            if( _v0 == rhs._v0 )
                return( (_v1 == rhs._v1) && (_v2 == rhs._v2) );
            if( _v0 == rhs._v1 )
                return( (_v1 == rhs._v2) && (_v2 == rhs._v0) );
            if( _v0 == rhs._v2 )
                return( (_v1 == rhs._v0) && (_v2 == rhs._v1) );
            else
                return( false );
        }

        unsigned int _v0, _v1, _v2;
        osg::Vec3 _norm;
    };
    typedef std::vector< Tri > TriList;
    typedef std::vector< TriList > TriListList;
    typedef std::map< unsigned int, TriList > VertToTriMap;

    struct Edge
    {
        Edge( int a, int b, bool sort=true )
        {
            _a = a;
            _b = b;
            if( sort && (a>b) )
            {
                // Sort indices so that (a,b) and (b,a) edges will be identical.
                _b = a;
                _a = b;
            }
        }

        bool operator<( const Edge& rhs ) const
        {
            if( _a < rhs._a )
                return( true );
            else if( _a == rhs._a )
                return( _b < rhs._b );
            else
                return( false );
        }

        unsigned int _a, _b;
    };
    typedef std::vector< Edge > EdgeList;
    typedef std::map< Edge, TriList > EdgeToTriMap;

    typedef std::vector< unsigned int > IndexList;


    bool makeMap( VertToTriMap& v2t, const osg::Geometry& geom );
    void makeGroups( TriListList& tll, const TriList& tl );
    EdgeList findBoundaryEdges( const TriList& tl, unsigned int vertIdx );
    bool removeableEdge( const EdgeList& el, const osg::Vec3Array* verts );
    void orderVerts( unsigned int removeIdx, const TriList& tl, IndexList& idxList );
    void removeTri( const Tri& tri, TriList& tl );
    bool removeableVertex( unsigned int removeIdx, const TriList& tl, osg::Vec3Array* verts );
    void deleteVertex( unsigned int removeIdx, const TriList& tl, VertToTriMap& v2t, osg::Vec3Array* verts );

    void reduce( osg::Geometry& geom );
};

}

#endif
