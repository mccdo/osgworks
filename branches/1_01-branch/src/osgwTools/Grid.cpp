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

#include "osgwTools/Grid.h"

namespace osgwTools {


osg::Geometry*
makeGrid( const osg::Vec3& corner, const osg::Vec3& s, const osg::Vec3& t, float stepSize )
{
    float sLen( s.length() );
    osg::Vec3 sStep( s / sLen * stepSize );
    int sCount( (int)(sLen / stepSize) );

    float tLen( t.length() );
    osg::Vec3 tStep( t / tLen * stepSize );
    int tCount( (int)(tLen / stepSize) );

    osg::Vec3Array* v = new osg::Vec3Array;

    osg::Vec3 end( corner+t );
    int mIdx;
    for( mIdx=0; mIdx<=sCount; mIdx++ )
    {
        osg::Vec3 strut( sStep * mIdx );
        v->push_back( corner+strut );
        v->push_back( end+strut );
    }
    end.set( corner+s );
    for( mIdx=0; mIdx<=tCount; mIdx++ )
    {
        osg::Vec3 strut( tStep * mIdx );
        v->push_back( corner+strut );
        v->push_back( end+strut );
    }

    osg::Vec4Array* c = new osg::Vec4Array;
    c->push_back( osg::Vec4( 1., 1., 1., 1. ) );

    osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;
    geom->setVertexArray( v );
    geom->setColorArray( c );
    geom->setColorBinding( osg::Geometry::BIND_OVERALL );

    geom->addPrimitiveSet( new osg::DrawArrays( GL_LINES, 0, v->getNumElements() ) );

    geom->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    return( geom.release() );
}

// osgwTools
}
