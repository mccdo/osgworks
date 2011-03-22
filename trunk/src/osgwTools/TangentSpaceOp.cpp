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

#include <osgwTools/TangentSpaceOp.h>
#include <osgUtil/TangentSpaceGenerator>
#include <osg/Geometry>
#include <osg/CopyOp>
#include <osg/io_utils>


namespace osgwTools {


TangentSpaceOp::TangentSpaceOp()
  : _normalMapTextureUnit( 0 ),
    _tangentIndex( 6 ),
    _binormalIndex( 7 ),
    _normalIndex( 15 )
{
    _tsg = new osgUtil::TangentSpaceGenerator;
}
TangentSpaceOp::TangentSpaceOp( const unsigned int normalMapTextureUnit, const unsigned int tangentIndex, const unsigned int binormalIndex, const unsigned int normalIndex )
  : _normalMapTextureUnit( normalMapTextureUnit ),
    _tangentIndex( tangentIndex ),
    _binormalIndex( binormalIndex ),
    _normalIndex( normalIndex )
{
    _tsg = new osgUtil::TangentSpaceGenerator;
}
TangentSpaceOp::TangentSpaceOp( const TangentSpaceOp& rhs, const osg::CopyOp& copyOp )
  : _normalMapTextureUnit( rhs._normalMapTextureUnit ),
    _tangentIndex( rhs._tangentIndex ),
    _binormalIndex( rhs._binormalIndex ),
    _normalIndex( rhs._normalIndex ),
    _tsg( rhs._tsg )
{
}
TangentSpaceOp::~TangentSpaceOp()
{
}


osg::Geometry*
TangentSpaceOp::operator()( osg::Geometry& geom )
{
    if( geom.getTexCoordArray( 0 ) == NULL )
    {
        // TangentSpaceGenerate does nothing if there are no texture coordinates.
        // Need a fallback for this (such as object space TexGen, for example).
        osg::notify( osg::WARN ) << "TangentSpaceOp: Geometry contains no texture coordinate array in unit 0." << std::endl;
        return( &geom );
    }

    _tsg->generate( &geom, _normalMapTextureUnit );

    if( geom.getVertexAttribArray( _normalIndex ) == NULL )
        geom.setVertexAttribData( _normalIndex,
            osg::Geometry::ArrayData( _tsg->getNormalArray(), osg::Geometry::BIND_PER_VERTEX ) );

    if( geom.getVertexAttribArray( _tangentIndex ) == NULL )
        geom.setVertexAttribData( _tangentIndex,
            osg::Geometry::ArrayData( _tsg->getTangentArray(), osg::Geometry::BIND_PER_VERTEX ) );

    if( geom.getVertexAttribArray( _binormalIndex ) == NULL )
        geom.setVertexAttribData( _binormalIndex,
            osg::Geometry::ArrayData( _tsg->getBinormalArray(), osg::Geometry::BIND_PER_VERTEX ) );

    return( &geom );
}


void TangentSpaceOp::setNormalMapTextureUnit( const unsigned int normalMapTextureUnit )
{
    _normalMapTextureUnit = normalMapTextureUnit;
}
unsigned int TangentSpaceOp::getNormalMapTextureUnit() const
{
    return( _normalMapTextureUnit );
}

void TangentSpaceOp::setVertexArrayLocations( const unsigned int tangentIndex, const unsigned int binormalIndex, const unsigned int normalIndex )
{
    _tangentIndex = tangentIndex;
    _binormalIndex = binormalIndex;
    _normalIndex = normalIndex;
}
void TangentSpaceOp::getVertexArrayLocations( unsigned int& tangentIndex, unsigned int& binormalIndex, unsigned int& normalIndex )
{
    tangentIndex = _tangentIndex;
    binormalIndex = _binormalIndex;
    normalIndex = _normalIndex;
}


// namespace osgwTools
}
