// Copyright (c) 2011 Skew Matrix Software LLC. All rights reserved.
// Copyright (c) 2011 Digital Transforms.

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgwMx/MxEventHandler.h>
#include <osgwMx/MxInputAdapterGamePadDirectInput.h>
#include <osgwMx/MxMovementStyleGamePad.h>

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
   viewer.setUpViewInWindow( 30, 30, 768, 480 );
   viewer.setSceneData( root.get() );
   viewer.addEventHandler( new osgViewer::StatsHandler );

   // create a game pad input handler and data interpreter to control the view.
   osg::ref_ptr<MxInputAdapterGamePadDirectInput> gpIn = new MxInputAdapterGamePadDirectInput;
   MxMovementStyleGamePad mxGamePad(gpIn.get());

   // Get the update callback from mxGamePad and attach it as an update
   // callback to the viewer's internal Camera node. This will update the view and
   // projection matrices each frame.
   viewer.getCamera()->setUpdateCallback(mxGamePad.getMatrixCallback());
   // We'll modify the Camera node, so mark it as DYNAMIC for thread safety.
   viewer.getCamera()->setDataVariance( osg::Object::DYNAMIC );

   // Do not use viewer.run(), which automatically adds a camera manipulator
   // if one doesn't already exist.
   const osg::FrameStamp *fs;
   while( !viewer.done() )
      {
      // need to pass the current simulation time (seconds & partial seconds as a double)
      // to the updateData() function in order to manage matrix manipulation at a
      // predictable rate.
      fs = viewer.getFrameStamp();
      mxGamePad.updateData(fs->getSimulationTime());
      viewer.frame();
      }
}
