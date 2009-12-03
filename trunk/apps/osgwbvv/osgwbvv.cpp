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

#include "osgwTools/Version.h"
#include <osg/ArgumentParser>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/ComputeBoundsVisitor>
#include <osg/BoundingSphere>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>

#include <osg/io_utils>





int
main( int argc,
      char ** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    arguments.getApplicationUsage()->setApplicationName( arguments.getApplicationName() );
    arguments.getApplicationUsage()->setDescription( arguments.getApplicationName() + " visualizes the OSG bounding volume of the loaded model(s)." );
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] filename ..." );

    arguments.getApplicationUsage()->addCommandLineOption( "--box", "Display the bounding box." );
    arguments.getApplicationUsage()->addCommandLineOption( "--sphere", "Display the bounding sphere. This is the default." );
    arguments.getApplicationUsage()->addCommandLineOption( "--both", "Display both the bounding sphere and bounding box." );
    arguments.getApplicationUsage()->addCommandLineOption( "-v/--version", "Display the osgWorks version string." );

    if( arguments.read( "-h" ) || arguments.read( "--help" ) )
    {
        osg::notify( osg::ALWAYS ) << arguments.getApplicationUsage()->getDescription() << std::endl;
        arguments.getApplicationUsage()->write( osg::notify( osg::ALWAYS ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }

    bool doBox( false );
    bool doSphere( true );
    if( arguments.find( "--box" ) >= 0 )
    {
        doBox = true;
        doSphere = false;
    }
    if( arguments.find( "--both" ) >= 0 )
        doBox = doSphere = true;
    if( arguments.find( "--sphere" ) >= 0 )
        doSphere = true;
    
    if( arguments.read( "-v" ) || arguments.read( "--version" ) )
    {
        osg::notify( osg::ALWAYS ) << osgwTools::getVersionString() << std::endl << std::endl;
    }

    osg::ref_ptr< osg::Group > root = new osg::Group;

    osg::ref_ptr< osg::Node > model = osgDB::readNodeFiles( arguments );
    if( model.get() == NULL )
    {
        osg::notify( osg::FATAL ) << "Unable to load model(S)." << std::endl;
        arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return( 1 );
    }
    root->addChild( model.get() );


    osg::ref_ptr< osg::Geode > geode;
    if( doSphere || doBox )
    {
        geode = new osg::Geode;
        root->addChild( geode.get() );

        osg::StateSet* ss = geode->getOrCreateStateSet();
        ss->setAttributeAndModes( new osg::PolygonMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ) );
        ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    }
    if( doSphere )
    {
        const osg::BoundingSphere bs( model->getBound() );
        osg::Sphere* sp = new osg::Sphere( bs._center, bs._radius );
        osg::ShapeDrawable* shape = new osg::ShapeDrawable( sp );
        geode->addDrawable( shape );

        osg::notify( osg::ALWAYS ) << "Sphere:" << std::endl;
        osg::notify( osg::ALWAYS ) << "\tCenter\t" << bs._center << std::endl;
        osg::notify( osg::ALWAYS ) << "\tRadius\t" << bs._radius << std::endl;
    }
    if( doBox )
    {
        osg::ComputeBoundsVisitor cbv;
        model->accept( cbv );
        const osg::BoundingBox bb( cbv.getBoundingBox() );

        osg::Vec3 ext( bb._max - bb._min );
        osg::Box* box = new osg::Box( bb.center(), ext.x(), ext.y(), ext.z() );
        osg::ShapeDrawable* shape = new osg::ShapeDrawable( box );
        geode->addDrawable( shape );

        osg::notify( osg::ALWAYS ) << "Box:" << std::endl;
        osg::notify( osg::ALWAYS ) << "\tCenter\t" << bb.center() << std::endl;
        osg::notify( osg::ALWAYS ) << "\tRadius\t" << bb.radius() << std::endl;
        osg::notify( osg::ALWAYS ) << "\tExtents\t" << ext << std::endl;
    }


    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return( viewer.run() );
}
