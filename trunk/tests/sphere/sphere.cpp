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

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>

#include <osg/MatrixTransform>
#include <osg/CullFace>
#include <osg/PolygonMode>
#include <osg/Texture2D>

#include <osgwTools/Shapes.h>

int
main( int argc,
      char ** argv )
{
    osg::ref_ptr< osg::Group > root = new osg::Group;

    osg::ref_ptr< osg::Geode > geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeAltAzSphere( 1., 8, 16 ) );
    root->addChild( geode.get() );

    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeGeodesicSphere( 1., 2 ) );
    osg::ref_ptr< osg::MatrixTransform > mt( new osg::MatrixTransform( osg::Matrix::translate( 3., 0., 0. ) ) );
    mt->addChild( geode.get() );
    root->addChild( mt.get() );

    {
        osg::StateSet* ss = root->getOrCreateStateSet();
        osg::CullFace* cf = new osg::CullFace;
        ss->setAttributeAndModes( cf );
        ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
        ss->setAttributeAndModes( new osg::PolygonMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ) );
    }

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return( viewer.run() );
}
