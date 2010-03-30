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

#include <osgwTools/RemoveLOD.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>


class KeyHandler : public osgGA::GUIEventHandler
{
public:
	KeyHandler( osg::Node* topNode )
      : _topNode( topNode )
    {}

    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
    {
        if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
        {
            if( ea.getKey() == 'l' )
            {
                if(osgwTools::RemoveLOD *rLOD = new osgwTools::RemoveLOD())
                {
					osg::notify( osg::WARN ) << "Running osgwTools::RemoveLOD" << std::endl;
					rLOD->traverse(*_topNode);
                } // if
                return( true );
            }
        }
        return( false );
    }

private:
    osg::Node* _topNode;
};




int
main( int argc, char ** argv )
{
    osg::ArgumentParser arguments( &argc, argv );
    arguments.getApplicationUsage()->setApplicationName( arguments.getApplicationName() );
    arguments.getApplicationUsage()->setDescription( arguments.getApplicationName() + " tests running various utility visitors." );
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] filename ..." );
    arguments.getApplicationUsage()->addCommandLineOption( "--saveoutput <filename>", "Resaves the modified scene to the file specified." );

    if( arguments.read( "-h" ) || arguments.read( "--help" ) )
    {
        arguments.getApplicationUsage()->write( osg::notify( osg::ALWAYS ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }

    if (arguments.errors())
    {
        arguments.writeErrorMessages( osg::notify( osg::FATAL ) );
        return 1;
    }

    std::string outputfilestr;
    arguments.read( "--saveoutput", outputfilestr );

	osgViewer::Viewer viewer;
    viewer.setUpViewInWindow( 10, 40, 800, 600 );
    viewer.setSceneData( osgDB::readNodeFiles( arguments ) );
    viewer.realize();

    // setup keyhandler to run RemoveLOD on top of scene when invoked
	KeyHandler* kh = new KeyHandler( viewer.getSceneData() );
    viewer.addEventHandler( kh );

	int runResult = viewer.run();

	if(!outputfilestr.empty())
	{
		osgDB::writeNodeFile(*viewer.getSceneData(), outputfilestr);
	} // if
	return(runResult);

}

