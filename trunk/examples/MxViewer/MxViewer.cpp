// Copyright (c) 2011 Skew Matrix Software LLC. All rights reserved.

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgwMx/MxEventHandler.h>


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
    viewer.setUpViewInWindow( 30, 30, 768, 480 );
    viewer.setSceneData( root.get() );
    viewer.addEventHandler( new osgViewer::StatsHandler );


    /* Set up the MxEventHandler. This is not a typical OSG CameraManipulator,
    which can change only the view matrix. The MxEventHandler seperates the
    act of receiving events from the act of setting the Camera view and projection
    matrices. */
    osg::ref_ptr< osgwMx::MxEventHandler > viewingHandler = new osgwMx::MxEventHandler;

    // Must specify the scene graph being viewed; used internally for zillions of things.
    viewingHandler->setSceneData( root.get() );
    // Add the MxEventHandler as a standard viewer EventHandler.
    viewer.addEventHandler( viewingHandler.get() );
    // Get the update callback from the MxEventHandler and attach it as an update
    // callback to the viewer's internal Camera node. This will update the view and
    // projection matrices each frame.
    viewer.getCamera()->setUpdateCallback( viewingHandler->getMatrixCallback() );
    // We'll modify the Camera node, so mark it as DYNAMIC for thread safety.
    viewer.getCamera()->setDataVariance( osg::Object::DYNAMIC );

    // Do not use viewer.run(), which automatically adds a camera manipulator
    // if one doesn't already exist.
    while( !viewer.done() )
        viewer.frame();
}
