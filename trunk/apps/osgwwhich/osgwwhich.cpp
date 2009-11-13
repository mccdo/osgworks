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



std::string
createLibraryName( std::string simpleName )
{
#if defined( WIN32 ) || defined( _WIN32 )
    return( std::string( "osg" ) +
        std::string( osgGetSOVersion() ) +
        std::string( "-" ) +
        simpleName +
        std::string( ".dll" ) );
#else
    return( std::string( "lib" ) +
        simpleName +
        std::string( "." ) +
        std::string( osgGetVersion() ) +
        std::string( ".dylib" ) );
#endif
}

void
locatePlugin( const std::string& target )
{
    std::string fullName = osgDB::findLibraryFile( target );
    if( fullName == std::string( "" ) )
    {
        osg::notify( osg::ALWAYS ) << "Can't find: " << target << std::endl;
        return;
    }
    osg::notify( osg::ALWAYS ) << fullName << std::endl;
}

void
locateSharedLibrary( const std::string& target )
{

    std::string fullName( osgDB::findLibraryFile( target ) );
#if 0 // future apple support? defined( __APPLE__ )
    if( fullName == std::string( "" ) )
    {
        // findLibraryFile is kind of hosed on OS X because it
        // always searches the plugin paths. To fix, we must explicitly
        // check the non-plugin paths.
        osgDB::FilePathList filePath;
        std::string path;
        if( getenv( "PATH" ) != NULL )
            path = std::string( getenv( "PATH" ) );
        if( getenv( "LD_LIBRARY_PATH" ) != NULL )
            path += std::string( getenv( "LD_LIBRARY_PATH" ) );
        osgDB::convertStringPathIntoFilePathList( path, filePath );
        fullName = osgDB::findFileInPath( target, filePath );
    }
#endif
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
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] [filename [...] ]" );

    arguments.getApplicationUsage()->addCommandLineOption( "-v/--version", "Display the osgWorks version string." );
    arguments.getApplicationUsage()->addCommandLineOption( "-p/--plugin <ext>", "Locate a plugin to support the extension <ext>, e.g., \"flt\"." );
    arguments.getApplicationUsage()->addCommandLineOption( "-l/--library <libname>", "Locate the library indicated with <libName>, e.g., \"osgUtil\"." );

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

    std::string param;
    if( arguments.read( "-p", param ) || arguments.read( "--plugin", param ) )
    {
        std::string plugin( osgDB::Registry::instance()->createLibraryNameForExtension( param ) );
        osg::notify( osg::ALWAYS ) << "Using plugin name: \"" << plugin << "\"." << std::endl;
        locateSharedLibrary( plugin );
    }    

    if( arguments.read( "-l", param ) || arguments.read( "--library", param ) )
    {
        std::string library( createLibraryName( param ) );
        locateSharedLibrary( library );
    }    


    int idx;
    for( idx=1; idx < arguments.argc(); idx++ )
    {
        if( !arguments.isOption( idx ) )
        {
            // Argument is not an option. Try to locate it.
            const std::string target( arguments[ idx ] );
            const std::string ext( osgDB::getLowerCaseFileExtension( target ) );
            if( ( ext == std::string( "dll" ) ) ||
                ( ext == std::string( "so" ) ) ||
                ( ext == std::string( "dylib" ) ) )
                locateSharedLibrary( target );
            else
                locateDataFile( target );
        }
    }
    
    return( 0 );
}

