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

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>

#include <osgwTools/CameraConfigObject.h>


int
main( int argc, char ** argv )
{
    osgViewer::Viewer viewer;
    viewer.setSceneData( osgDB::readNodeFile( "cow.osg" ) );
#if 0
    viewer.setUpViewAcrossAllScreens();
    viewer.realize();

    osg::ref_ptr< osgwTools::CameraConfigObject > cco = new osgwTools::CameraConfigObject;
    cco->take( viewer );
    osgDB::writeObjectFile( *cco, "viewerconfig.osg" );
#else
    osg::ref_ptr< osgwTools::CameraConfigObject > cco = 
        dynamic_cast< osgwTools::CameraConfigObject* >( osgDB::readObjectFile( "viewerconfig.osg" ) );
    cco->store( viewer );
#endif

    return( viewer.run() );
}

