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
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osg/Version>
#include <osg/ArgumentParser>

#include <osg/io_utils>
#include <string>



void
locateSharedLibrary( const std::string& target )
{
    std::string fullName( osgDB::findLibraryFile( target ) );
    if( fullName == std::string( "" ) )
    {
        // We didn't find this as a library.
        // Add the plugin prefix and check in that locaion.

        // From osgDB::Registry::createLibraryNameForExtension
        const std::string prepend = std::string("osgPlugins-")+std::string(osgGetVersion())+std::string("/");

        fullName = osgDB::findLibraryFile( prepend + target );
        if( fullName == std::string( "" ) )
        {
            osg::notify( osg::ALWAYS ) << "Can't find: " << target << std::endl;
            return;
        }
    }
    osg::notify( osg::ALWAYS ) << fullName << std::endl;
}

void
locateDataFile( const std::string& target )
{
    const std::string fullName( osgDB::findDataFile( target ) );
    if( fullName == std::string( "" ) )
        osg::notify( osg::ALWAYS ) << "Can't find: " << target << std::endl;
    else
        osg::notify( osg::ALWAYS ) << fullName << std::endl;
}


int
main( int argc,
      char ** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    arguments.getApplicationUsage()->setApplicationName( arguments.getApplicationName() );
    arguments.getApplicationUsage()->setDescription( arguments.getApplicationName() + " locates data files and shared libraries using OSG's search algorithms." );
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] filename ..." );

    arguments.getApplicationUsage()->addCommandLineOption( "-v/--version", "Display the osgWorks version string." );

    if( arguments.read( "-h" ) || arguments.read( "--help" ) )
    {
        osg::notify( osg::ALWAYS ) << arguments.getApplicationUsage()->getDescription() << std::endl;
        arguments.getApplicationUsage()->write( osg::notify( osg::ALWAYS ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }

    if( arguments.read( "-v" ) || arguments.read( "--version" ) )
    {
        osg::notify( osg::ALWAYS ) << osgwTools::getVersionString() << std::endl << std::endl;
    }


    int idx;
    for( idx=1; idx < arguments.argc(); idx++ )
    {
        if( !arguments.isOption( idx ) )
        {
            // Argument is not an option. Try to locate it.
            const std::string target( arguments[ idx ] );
            const std::string ext( osgDB::getLowerCaseFileExtension( target ) );
            if( ( ext == std::string( "dll" ) ) || ( ext == std::string( "so" ) ) )
                locateSharedLibrary( target );
            else
                locateDataFile( target );
        }
    }
    
    return( 0 );
}

