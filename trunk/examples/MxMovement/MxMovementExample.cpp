// Copyright (c) 2011 Skew Matrix Software LLC. All rights reserved.
// Copyright (c) 2011 Digital Transforms.

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgwMx/MxGamePadDX.h>
#include <osgwMx/MxCore.h>


using namespace osgwMx;

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    osg::ref_ptr< osg::Node > root = osgDB::readNodeFiles( arguments );
    if( !( root.valid() ) )
    {
        root = osgDB::readNodeFile( "dumptruck.osg" );
        if( !( root.valid() ) )
        {
            osg::notify( osg::FATAL ) << "Can't load input file." << std::endl;
            return( 1 );
        }
    }

    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow( 30, 30, 800, 450 );
    viewer.setSceneData( root.get() );
    viewer.addEventHandler( new osgViewer::StatsHandler );

    // create a game pad input handler and data interpreter to control the view.
    osg::ref_ptr< MxGamePadDX > gamePad = new MxGamePadDX;

    // Set some MxCore defaults:
    osgwMx::MxCore* mxCore = gamePad->getMxCore();
    mxCore->setInitialValues( osg::Vec3d( 0., 0., 1. ), osg::Vec3d( 0., 1., 0. ),
        osg::Vec3d( 0., -30., 0. ) );
    mxCore->setMoveScale( osg::Vec3d( .4, .4, .4 ) );
    mxCore->reset();


    // We'll modify the Camera node, so mark it as DYNAMIC for thread safety.
    viewer.getCamera()->setDataVariance( osg::Object::DYNAMIC );

    // Do not use viewer.run(), which automatically adds a camera manipulator
    // if one doesn't already exist.
    while( !viewer.done() )
    {
        // DX uses polling; other VR device SDKs might send events.
        gamePad->poll();

        // Update the current view matrix.
        viewer.getCamera()->setViewMatrix( mxCore->getInverseMatrix() );

        viewer.frame();
    }
}
