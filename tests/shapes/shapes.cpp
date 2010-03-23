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
#include <osgGA/StateSetManipulator>
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

    osg::ref_ptr< osg::Geode > geode;
    osg::ref_ptr< osg::MatrixTransform > mt;
    
    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeGeodesicSphere( 1., 2 ) );
    mt = new osg::MatrixTransform( osg::Matrix::translate( -3., 0., 3. ) );
    root->addChild( mt.get() );
    mt->addChild( geode.get() );

    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeAltAzSphere( 1., 8, 16 ) );
    mt = new osg::MatrixTransform( osg::Matrix::translate( 0., 0., 3. ) );
    root->addChild( mt.get() );
    mt->addChild( geode.get() );

    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeWireAltAzSphere( 1., 5, 7 ) );
    mt = new osg::MatrixTransform( osg::Matrix::translate( 3., 0., 3. ) );
    root->addChild( mt.get() );
    mt->addChild( geode.get() );

    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeBox( osg::Vec3( .75, .75, .75 ), osg::Vec3s( 2, 2, 3 ) ) );
    mt = new osg::MatrixTransform( osg::Matrix::translate( -3., 0., 0. ) );
    root->addChild( mt.get() );
    mt->addChild( geode.get() );

    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeWireBox( osg::Vec3( .75, .75, .75 ) ) );
    mt = new osg::MatrixTransform( osg::Matrix::translate( 0., 0., 0. ) );
    root->addChild( mt.get() );
    mt->addChild( geode.get() );

    geode = new osg::Geode;
    geode->addDrawable( osgwTools::makeArrow() );
    mt = new osg::MatrixTransform( osg::Matrix::translate( -3., 0., -3. ) );
    root->addChild( mt.get() );
    mt->addChild( geode.get() );

    osgGA::StateSetManipulator* ssmanip = new osgGA::StateSetManipulator;
    ssmanip->setStateSet( root->getOrCreateStateSet() );

    osgViewer::Viewer viewer;
    viewer.addEventHandler( ssmanip );
    viewer.setSceneData( root.get() );
    return( viewer.run() );
}
