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

#include <osgwTools/Shapes.h>
#include <osg/Geometry>

#include <math.h>

#include <osg/io_utils>



#ifdef WIN32
#  pragma warning( disable : 4305 )
#endif

inline unsigned int makeKey( const unsigned short a, const unsigned short b )
{
	if ( a < b )
		return( ( a << 16) | b );
	else
		return( ( b << 16) | a );
};
inline void average3fv( float* r, const float* a, const float* b )
{
	r[0] = (a[0] + b[0]) * .5f;
	r[1] = (a[1] + b[1]) * .5f;
	r[2] = (a[2] + b[2]) * .5f;
};

static bool
buildGeodesicSphereData( const float radius, const unsigned int subdivisions, osg::Geometry* geom )
{
    unsigned int subdivide( subdivisions );
    if( subdivisions > 5 )
    {
        // Would create index array too large for use with DrawElementsUShort.
        // For now, clamp. In the future, just use DrawElementsUInt.
        osg::notify( osg::WARN ) << "makeGeodesicSphere: Clamping subdivisions to 5." << std::endl;
        subdivide = 5;
    }

    GLfloat vertData[] = {
        0.000000, 0.850651, 0.525731,
        0.000000, 0.850651, -0.525731,
        0.000000, -0.850651, -0.525731,
        0.000000, -0.850651, 0.525731,
        0.525731, 0.000000, 0.850651,
        0.525731, 0.000000, -0.850651,
        -0.525731, 0.000000, -0.850651,
        -0.525731, 0.000000, 0.850651,
        0.850651, 0.525731, 0.000000,
        0.850651, -0.525731, 0.000000,
        -0.850651, -0.525731, 0.000000,
        -0.850651, 0.525731, 0.000000 };

    int faces = 20;
    int _numVerts = 12;
    int _numIndices = faces * 3;

    // Data is initially in "golden mean" coordinate system.
    // Rotate around y so that 2 verts exist at (0,0,+/-1).
    {
        //osg::Vec3 v0( 0.525731, 0.000000, 0.850651 );
        //osg::Vec3 v1( 0, 0, 1 );
        //const double angle( acos( v0 * v1 ) );
        const double sinAngle( 0.525731 );
        const double cosAngle( 0.850651 );

        int idx;
        for( idx=0; idx<_numVerts*3; idx+=3 )
        {
            double x( vertData[ idx ] );
            double z( vertData[ idx+2 ] );
            vertData[ idx ] = x * cosAngle + z * -sinAngle;
            vertData[ idx+2 ] = x * sinAngle + z * cosAngle;
        }
    }


    int vertsSize = _numVerts * 3;
    GLfloat* _vertices = new GLfloat[ vertsSize ];
    memcpy( _vertices, vertData, sizeof( vertData ) );

    GLushort* _indices = new GLushort[ _numIndices ];
    GLushort* indexPtr = _indices;
    *indexPtr++ = 0;
    *indexPtr++ = 7;
    *indexPtr++ = 4;
    *indexPtr++ = 0;
    *indexPtr++ = 4;
    *indexPtr++ = 8;
    *indexPtr++ = 0;
    *indexPtr++ = 8;
    *indexPtr++ = 1;
    *indexPtr++ = 0;
    *indexPtr++ = 1;
    *indexPtr++ = 11;
    *indexPtr++ = 0;
    *indexPtr++ = 11;
    *indexPtr++ = 7;
    *indexPtr++ = 2;
    *indexPtr++ = 6;
    *indexPtr++ = 5;
    *indexPtr++ = 2;
    *indexPtr++ = 5;
    *indexPtr++ = 9;
    *indexPtr++ = 2;
    *indexPtr++ = 9;
    *indexPtr++ = 3;
    *indexPtr++ = 2;
    *indexPtr++ = 3;
    *indexPtr++ = 10;
    *indexPtr++ = 2;
    *indexPtr++ = 10;
    *indexPtr++ = 6;
    *indexPtr++ = 7;
    *indexPtr++ = 3;
    *indexPtr++ = 4;
    *indexPtr++ = 4;
    *indexPtr++ = 3;
    *indexPtr++ = 9;
    *indexPtr++ = 4;
    *indexPtr++ = 9;
    *indexPtr++ = 8;
    *indexPtr++ = 8;
    *indexPtr++ = 9;
    *indexPtr++ = 5;
    *indexPtr++ = 8;
    *indexPtr++ = 5;
    *indexPtr++ = 1;
    *indexPtr++ = 1;
    *indexPtr++ = 5;
    *indexPtr++ = 6;
    *indexPtr++ = 1;
    *indexPtr++ = 6;
    *indexPtr++ = 11;
    *indexPtr++ = 11;
    *indexPtr++ = 6;
    *indexPtr++ = 10;
    *indexPtr++ = 11;
    *indexPtr++ = 10;
    *indexPtr++ = 7;
    *indexPtr++ = 7;
    *indexPtr++ = 10;
    *indexPtr++ = 3;

    GLuint _idxStart = 0;
    GLuint _idxEnd = 11;


    // Subdivide as requested
    int idx;
    for (idx = subdivide; idx; idx--)
    {
        // Make a map of edges
        typedef std::map< unsigned int, GLushort> EdgeMapType;
        EdgeMapType edgeMap;
        indexPtr = _indices;
        int f;
        for (f=faces; f; f--)
        {
            unsigned int key = makeKey(indexPtr[0], indexPtr[1]);
            if (edgeMap.find( key ) == edgeMap.end())
                edgeMap[key] = ++_idxEnd;

            key = makeKey(indexPtr[1], indexPtr[2]);
            if (edgeMap.find( key ) == edgeMap.end())
                edgeMap[key] = ++_idxEnd;
            
            key = makeKey(indexPtr[2], indexPtr[0]);
            if (edgeMap.find( key ) == edgeMap.end())
                edgeMap[key] = ++_idxEnd;

            indexPtr += 3;
        }

        GLfloat* oldVerts = _vertices;
        GLushort* oldIndices = _indices;

        _numVerts += (int)(faces * 1.5f);
        int newFaces = faces * 4;
        _numIndices = newFaces * 3;

        // Create new indices
        _indices = new GLushort[ _numIndices ];
        GLushort* oldIdxPtr = oldIndices;
        indexPtr = _indices;
        for (f=faces; f; f--)
        {
            GLushort vertA = *oldIdxPtr++;
            GLushort vertB = *oldIdxPtr++;
            GLushort vertC = *oldIdxPtr++;
            GLushort edgeAB = edgeMap[ makeKey(vertA,vertB) ];
            GLushort edgeBC = edgeMap[ makeKey(vertB,vertC) ];
            GLushort edgeCA = edgeMap[ makeKey(vertC,vertA) ];

            *indexPtr++ = vertA;
            *indexPtr++ = edgeAB;
            *indexPtr++ = edgeCA;
            *indexPtr++ = edgeAB;
            *indexPtr++ = vertB;
            *indexPtr++ = edgeBC;
            *indexPtr++ = edgeAB;
            *indexPtr++ = edgeBC;
            *indexPtr++ = edgeCA;
            *indexPtr++ = edgeCA;
            *indexPtr++ = edgeBC;
            *indexPtr++ = vertC;
        }

        // Copy old vertices into new vertices
        _vertices = new GLfloat[ _numVerts * 3 ];
        memcpy( _vertices, oldVerts, vertsSize * sizeof( GLfloat ) );

        // Create new vertices at midpoint of each edge
        EdgeMapType::const_iterator it = edgeMap.begin();
        while (it != edgeMap.end())
        {
            GLushort idxA, idxB;
            idxA = ((*it).first) >> 16;
            idxB = ((*it).first) & 0xffff;

            GLfloat* dest = &(_vertices[ ((*it).second * 3) ]);
            GLfloat* srcA = &(_vertices[idxA*3]);
            GLfloat* srcB = &(_vertices[idxB*3]);
            average3fv( dest, srcA, srcB );

            it++;
        }


        faces = newFaces;
        vertsSize = _numVerts * 3;
        delete[] oldVerts;
        delete[] oldIndices;
    }


    //
    // Create normal array by making vertices unit length
    GLfloat* _normals = new GLfloat[ _numVerts * 3 ];
    GLfloat* vertPtr = _vertices;
    GLfloat* normPtr = _normals;
    for (idx = _numVerts; idx; idx--)
    {
        osg::Vec3 v( vertPtr[0], vertPtr[1], vertPtr[2] );
        float lengthInv = (float)( 1. / v.length() );
        *normPtr++ = *vertPtr++ * lengthInv;
        *normPtr++ = *vertPtr++ * lengthInv;
        *normPtr++ = *vertPtr++ * lengthInv;
    }

    //
    // Scale vertices out to the specified radius
    vertPtr = _vertices;
    normPtr = _normals;
    for (idx = _numVerts*3; idx; idx--)
        *vertPtr++ = *normPtr++ * radius;

    //
    // Texture coordinates are identical to normals for cube mapping
    GLfloat* _texCoords = new GLfloat[ _numVerts * 3 ];
    memcpy( _texCoords, _normals, _numVerts * 3 * sizeof( GLfloat) );


    // Convert to OSG
    {
        osg::Vec3Array* osgV = new osg::Vec3Array;
        osg::Vec3Array* osgN = new osg::Vec3Array;
        osg::Vec3Array* osgTC = new osg::Vec3Array;
        osgV->resize( _numVerts );
        osgN->resize( _numVerts );
        osgTC->resize( _numVerts );

        geom->setVertexArray( osgV );
        geom->setNormalArray( osgN );
        geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
        geom->setTexCoordArray( 0, osgTC );

        osg::Vec4Array* osgC = new osg::Vec4Array;
        osgC->push_back( osg::Vec4( 1., 1., 1., 1. ) );
        geom->setColorArray( osgC );
        geom->setColorBinding( osg::Geometry::BIND_OVERALL );

        vertPtr = _vertices;
        normPtr = _normals;
        GLfloat* tcPtr = _texCoords;
        int idx;
        for( idx=0; idx<_numVerts; idx++ )
        {
            (*osgV)[ idx ].x() = *vertPtr++;
            (*osgV)[ idx ].y() = *vertPtr++;
            (*osgV)[ idx ].z() = *vertPtr++;
            (*osgN)[ idx ].x() = *normPtr++;
            (*osgN)[ idx ].y() = *normPtr++;
            (*osgN)[ idx ].z() = *normPtr++;
            (*osgTC)[ idx ].x() = *tcPtr++;
            (*osgTC)[ idx ].y() = *tcPtr++;
            (*osgTC)[ idx ].z() = *tcPtr++;
        }

        osg::UShortArray* osgIdx = new osg::UShortArray;
        osgIdx->resize( _numIndices );
        indexPtr = _indices;
        for( idx=0; idx<_numIndices; idx++ )
        {
            (*osgIdx)[ idx ] = *indexPtr++;
        }

        geom->addPrimitiveSet( new osg::DrawElementsUShort( GL_TRIANGLES, _numIndices, _indices ) );
    }


    delete[] _indices;
    delete[] _vertices;
    delete[] _normals;
    delete[] _texCoords;

    osg::notify( osg::INFO ) << "makeGeodesicSphere: numVertices: " << _numVerts << std::endl;
    return( true );
}


osg::Geometry*
osgwTools::makeGeodesicSphere( const float radius, const unsigned int subdivisions, osg::Geometry* geometry )
{
    osg::ref_ptr< osg::Geometry > geom( geometry );
    if( geom == NULL )
        geom = new osg::Geometry;

    bool result = buildGeodesicSphereData( radius, subdivisions, geom.get() );
    if( !result )
    {
        osg::notify( osg::WARN ) << "makeGeodesicSphere: Error during sphere build." << std::endl;
        return( NULL );
    }
    else
        return( geom.release() );
}




bool
buildAltAzSphereData( const float radius, const unsigned int subLat, const unsigned int subLong, osg::Geometry* geom )
{
    unsigned int numLat( subLat );
    unsigned int numLong( subLong );
    if( numLat < 2 )
        numLat = 2;
    if( numLong < 4 )
        numLong = 4;
    unsigned int totalVerts( (numLat+1) * (numLong+1) );
    if( totalVerts > 65535 )
    {
        // Would create index array too large for use with DrawElementsUShort.
        // For now, clamp. In the future, just use DrawElementsUInt.
        osg::notify( osg::WARN ) << "makeAltAzSphere: Clamping subdivisions to 128x256." << std::endl;
        numLat = 128;
        numLong = 256;
        totalVerts = ( (numLat+1) * (numLong+1) );
    }
    osg::notify( osg::INFO ) << "makeAltAzSphere: totalVerts: " << totalVerts << std::endl;

    // Create data arrays and configure the Geometry
    osg::ref_ptr< osg::Vec3Array > vertices( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > normals( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec2Array > texCoords( new osg::Vec2Array );
    vertices->resize( totalVerts );
    normals->resize( totalVerts );
    texCoords->resize( totalVerts );

    geom->setVertexArray( vertices.get() );
    geom->setNormalArray( normals.get() );
    geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    geom->setTexCoordArray( 0, texCoords.get() );

    // Create the vertices, normals, and tex coords.
    unsigned int idx( 0 );
    unsigned int latCounter;
    for( latCounter=numLat+1; latCounter>0; latCounter-- )
    {
        const double t( (double)(latCounter-1) / (double)numLat );
        const double latAngle( (t-0.5) * osg::PI );
        const osg::Vec3 baseVec( cos( latAngle ), 0., sin( latAngle ) );

        unsigned int longCounter;
        for( longCounter=0; longCounter<numLong; longCounter++ )
        {
            const double s( (double)longCounter / (double)numLong );
            const double longAngle( s * 2.0 * osg::PI );
            const double sinAngle( sin( longAngle ) );
            const double cosAngle( cos( longAngle ) );

            osg::Vec3 v;
            v[ 0 ] = baseVec.x() * cosAngle + baseVec.y() * -sinAngle;
            v[ 1 ] = baseVec.x() * sinAngle + baseVec.y() * cosAngle;
            v[ 2 ] = baseVec.z();

            (*vertices)[ idx ] = ( v * radius );
            (*normals)[ idx ] = v;
            (*texCoords)[ idx ].set( s, t );
            //osg::notify( osg::ALWAYS ) << v << std::endl;

            idx++;
        }
        {
            // Close (required for texture mapping)
            osg::Vec3 v( baseVec );

            (*vertices)[ idx ] = ( v * radius );
            (*normals)[ idx ] = v;
            (*texCoords)[ idx ].set( 1., t );
            //osg::notify( osg::ALWAYS ) << v << std::endl;

            idx++;
        }
    }

    if( idx != totalVerts )
    {
        osg::notify( osg::WARN ) << "makeAltAzSphere: Error creating vertices." << std::endl;
        osg::notify( osg::WARN ) << "  idx " << idx << " != totalVerts " << totalVerts << std::endl;
    }


    // Create indices -- top group of triangles
    osg::DrawElementsUShort* fan( new osg::DrawElementsUShort( GL_TRIANGLES ) );
    fan->resize( numLong*3 );
    for( idx=0; idx<numLong; idx++ )
    {
        (*fan)[ idx*3 ] = idx;
        (*fan)[ idx*3+1 ] = numLong + idx + 1;
        (*fan)[ idx*3+2 ] = numLong + idx + 2;
    }
    geom->addPrimitiveSet( fan );

    // Create indices -- body
    osg::DrawElementsUShort* body;
    unsigned int baseIdx( numLong+1 );
    unsigned int stripIdx;
    for( stripIdx=0; stripIdx<numLat-2; stripIdx++ )
    {
        body = new osg::DrawElementsUShort( GL_TRIANGLE_STRIP );
        body->resize( (numLong+1) * 2 );

        unsigned int longCounter;
        for( longCounter=0; longCounter<numLong; longCounter++ )
        {
            (*body)[ longCounter*2 ] = baseIdx;
            (*body)[ longCounter*2+1 ] = baseIdx + numLong+1;
            baseIdx++;
        }
        // Close strip
        (*body)[ longCounter*2 ] = baseIdx;
        (*body)[ longCounter*2+1 ] = baseIdx + numLong+1;
        baseIdx++;

        geom->addPrimitiveSet( body );
    }

    // Create indices -- bottom group of triangles
    fan = new osg::DrawElementsUShort( GL_TRIANGLES );
    fan->resize( numLong*3 );
    for( idx=0; idx<numLong; idx++ )
    {
        // 14 9 8, 13 8 7, 12 7 6, 11 6 5
        (*fan)[ idx*3 ] = totalVerts - 1 - idx;
        (*fan)[ idx*3+1 ] = totalVerts - 1 - numLong - idx - 1;
        (*fan)[ idx*3+2 ] = totalVerts - 1 - numLong - idx - 2;
    }
    geom->addPrimitiveSet( fan );

    return( true );
}

osg::Geometry*
osgwTools::makeAltAzSphere( const float radius, const unsigned int subLat, const unsigned int subLong, osg::Geometry* geometry )
{
    osg::ref_ptr< osg::Geometry > geom( geometry );
    if( geom == NULL )
        geom = new osg::Geometry;

    bool result = buildAltAzSphereData( radius, subLat, subLong, geom.get() );
    if( !result )
    {
        osg::notify( osg::WARN ) << "makeAltAzSphere: Error during sphere build." << std::endl;
        return( NULL );
    }
    else
        return( geom.release() );
}


