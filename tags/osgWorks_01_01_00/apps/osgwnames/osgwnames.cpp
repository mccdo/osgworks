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
#include <osgDB/ReadFile>
#include <osg/NodeVisitor>

#include <string>

class ShowNodeNamesVisitor : public osg::NodeVisitor
{
public:
    ShowNodeNamesVisitor( const bool showGeodes )
      : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
        _showGeodes( showGeodes ),
        _level( 0 )
    {}

    void apply( osg::Node& n )
    {
        int idx;
        for( idx=0; idx<_level; idx++ )
            osg::notify( osg::NOTICE ) << "  ";

        if( !n.getName().empty() )
            osg::notify( osg::NOTICE ) << n.getName();
        else
            osg::notify( osg::NOTICE ) << "NULL";
        osg::notify( osg::NOTICE ) << " (" << n.className() << ")" << std::endl;

        _level++;
        traverse( n );
        _level--;
    }

    void apply( osg::Geode& n )
    {
        if( _showGeodes )
            apply( *( osg::Node* )&n );
    }

protected:
    int _level;
    bool _showGeodes;
};


int
main( int argc,
      char ** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    arguments.getApplicationUsage()->setApplicationName( arguments.getApplicationName() );
    arguments.getApplicationUsage()->setDescription( arguments.getApplicationName() + " visualizes the OSG bounding volume of the loaded model(s)." );
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] filename ..." );

    arguments.getApplicationUsage()->addCommandLineOption( "-O <option>", "Pass <option> to import plugin." );
    arguments.getApplicationUsage()->addCommandLineOption( "--nogeodes", "Do not display Geodes in the output." );
    arguments.getApplicationUsage()->addCommandLineOption( "-v/--version", "Display the osgWorks version string." );

    if( arguments.read( "-h" ) || arguments.read( "--help" ) )
    {
        osg::notify( osg::ALWAYS ) << arguments.getApplicationUsage()->getDescription() << std::endl;
        arguments.getApplicationUsage()->write( osg::notify( osg::ALWAYS ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }

    bool showGeodes( true );
    if( arguments.find( "--nogeodes" ) >= 0 )
        showGeodes = false;

    std::string str;
    while( arguments.read( "-O", str ) )
    {
        osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options;
        options->setOptionString( str );
        osgDB::Registry::instance()->setOptions( options );
    }

    if( arguments.read( "-v" ) || arguments.read( "--version" ) )
    {
        osg::notify( osg::ALWAYS ) << osgwTools::getVersionString() << std::endl << std::endl;
    }


    osg::ref_ptr< osg::Node > root = osgDB::readNodeFiles( arguments );
    if( root.get() == NULL )
    {
        osg::notify( osg::FATAL ) << "Unable to load model(S)." << std::endl;
        arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return( 1 );
    }


    ShowNodeNamesVisitor snnv( showGeodes );
    root->accept( snnv );

    return( 0 );
}

