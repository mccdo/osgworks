/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2011 by Kenneth Mark Bryden
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
#include <osg/NodeVisitor>
#include <osg/Texture2D>
#include <osg/io_utils>

#include <osgUtil/TangentSpaceGenerator>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "osgwTools/Shapes.h"

#include <iostream>


////////////////////////////////////////////////////////////////////////////////
class TSGVisitor : public osg::NodeVisitor
{
public:
    ///Constructor
    TSGVisitor(
               osg::Node* const node,
               unsigned int normalMapTexUnit,
               unsigned int tangentIndex = 6,
               unsigned int binormalIndex = 7,
               unsigned int normalIndex = 15 )
        :
        osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
        m_normalMapTexUnit( normalMapTexUnit ),
        m_tangentIndex( tangentIndex ),
        m_binormalIndex( binormalIndex ),
        m_normalIndex( normalIndex )
    {
        node->accept( *this );
    }
    
    ///Destructor
    virtual ~TSGVisitor()
    {
        ;
    }
    ///
    META_NodeVisitor( osgw, TSGVisitor );
    
    ///
    virtual void apply( osg::Geode& geode )
    {
        for( unsigned int i = 0; i < geode.getNumDrawables(); ++i )
        {
            osg::Geometry* geometry =
            static_cast< osg::Geometry* >( geode.getDrawable( i ) );
            if( geometry )
            {
                PrepareGeometry( geometry );
            }
        }
        
        osg::NodeVisitor::apply( geode );
    }

protected:
    
private:
    ///
    void PrepareGeometry( osg::Geometry* const geometry )
    {
        //Create the tangent, binormal, and normal array data
        osg::ref_ptr< osgUtil::TangentSpaceGenerator > tsg =
            new osgUtil::TangentSpaceGenerator;
            tsg->generate( geometry, m_normalMapTexUnit );
        
        //Assign the tangent array data
        if( !geometry->getVertexAttribArray( m_tangentIndex ) )
        {
            geometry->setVertexAttribData(
                m_tangentIndex,
                osg::Geometry::ArrayData(
                tsg->getTangentArray(),
                osg::Geometry::BIND_PER_VERTEX, GL_FALSE ) );
        }
        else
        {
            ;
        }
        
        //Assign the binormal array data
        if( !geometry->getVertexAttribArray( m_binormalIndex ) )
        {
            geometry->setVertexAttribData(
                m_binormalIndex,
                osg::Geometry::ArrayData(
                tsg->getBinormalArray(),
                osg::Geometry::BIND_PER_VERTEX, GL_FALSE ) );
        }
        else
        {
            ;
        }
        
        //Assign the normal array data
        if( !geometry->getVertexAttribArray( m_normalIndex ) )
        {
            geometry->setVertexAttribData(
                m_normalIndex,
                osg::Geometry::ArrayData(
                tsg->getNormalArray(),
                osg::Geometry::BIND_PER_VERTEX, GL_FALSE ) );
        }
        else
        {
            ;
        }
    }
    ///
    unsigned int m_normalMapTexUnit;
    
    ///
    unsigned int m_tangentIndex;
    
    ///
    unsigned int m_binormalIndex;
    
    ///
    unsigned int m_normalIndex;
};
////////////////////////////////////////////////////////////////////////////////
void addShaders( osg::Node* node, osg::Image* baseImage, osg::Image* normalImage, osg::Image* heightImage )
{
    osg::ref_ptr< osg::StateSet > stateSet = node->getOrCreateStateSet();

    if( baseImage )
    {
        osg::ref_ptr< osg::Texture2D > baseMap = new osg::Texture2D();
        baseMap->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
        baseMap->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
        baseMap->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
        baseMap->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
        stateSet->
        setTextureAttributeAndModes( 0, baseMap.get(), osg::StateAttribute::ON );
        baseMap->setImage( baseImage );
    }

    osg::ref_ptr< osg::Uniform > baseMapUniform =
        new osg::Uniform( "baseMap", 0 );
    stateSet->addUniform( baseMapUniform.get() );

    ///Setup the normal map
    osg::ref_ptr< osg::Texture2D > normalMap = new osg::Texture2D();
    normalMap->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
    normalMap->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    normalMap->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
    normalMap->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
    stateSet->
        setTextureAttributeAndModes( 1, normalMap.get(), osg::StateAttribute::ON );
    normalMap->setImage( normalImage );

    osg::ref_ptr< osg::Uniform > normalMapUniform =
        new osg::Uniform( "normalMap", 1 );
    stateSet->addUniform( normalMapUniform.get() );

    ///Setup the height map
    osg::ref_ptr< osg::Texture2D > heightMap = new osg::Texture2D();
    heightMap->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
    heightMap->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    heightMap->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
    heightMap->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
    stateSet->
        setTextureAttributeAndModes( 2, heightMap.get(), osg::StateAttribute::ON );
    heightMap->setImage( heightImage );

    osg::ref_ptr< osg::Uniform > heightMapUniform =
        new osg::Uniform( "heightMap", 2 );
    stateSet->addUniform( heightMapUniform.get() );

    ///Set the bool to control the height map usage
    osg::ref_ptr< osg::Uniform > heightControl = 
        new osg::Uniform( "useHeightMap", false );
    if( heightImage )
    {
        heightControl->set( true );
    }
    stateSet->addUniform( heightControl.get() );

    ///Setup the shaders and programs
    std::string shaderName = osgDB::findDataFile( "parallax_mapping.fs" );
    osg::ref_ptr< osg::Shader > fragmentShader = 
        osg::Shader::readShaderFile( osg::Shader::FRAGMENT, shaderName );

    shaderName = osgDB::findDataFile( "parallax_mapping.vs" );
    osg::ref_ptr< osg::Shader > vertexShader = 
        osg::Shader::readShaderFile( osg::Shader::VERTEX, shaderName );

    osg::ref_ptr< osg::Program > program = new osg::Program();
    program->addShader( vertexShader.get() );
    program->addShader( fragmentShader.get() );
    program->addBindAttribLocation( "a_tangent", 6 );
    program->addBindAttribLocation( "a_binormal", 7 );
    program->addBindAttribLocation( "a_normal", 15 );
    stateSet->setAttribute( program.get(), osg::StateAttribute::ON );
    
    ///Now process the geometry
    TSGVisitor tsgVisitor( node, 0 );    
}
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    arguments.getApplicationUsage()->setApplicationName( arguments.getApplicationName() );
    arguments.getApplicationUsage()->setDescription( arguments.getApplicationName() + " map the normal texture to the loaded model." );
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] filename ..." );

    arguments.getApplicationUsage()->addCommandLineOption( "--height", "Path to the height texture." );
    arguments.getApplicationUsage()->addCommandLineOption( "--normal", "Path to the normal texture." );
    arguments.getApplicationUsage()->addCommandLineOption( "--base", "Path to the base texture if texture 0 is not the base texture." );
    arguments.getApplicationUsage()->addCommandLineOption( "-v/--version", "Display the osgWorks version string." );

    if( arguments.read( "-h" ) || arguments.read( "--help" ) )
    {
        osg::notify( osg::ALWAYS ) << arguments.getApplicationUsage()->getDescription() << std::endl;
        arguments.getApplicationUsage()->write( osg::notify( osg::ALWAYS ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }

    ///Grab the normal map name
    if( arguments.find( "--normal" ) == -1 )
    {
        osg::notify( osg::FATAL ) << "There were no normal textures specified. Please supply a normal texture." << std::endl;
        arguments.getApplicationUsage()->
            write( osg::notify( osg::FATAL ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }
    std::string normalMapName;
    arguments.read( "--normal", normalMapName );

    ///Grab the height map name
    bool heightMap( false );
    std::string heightMapName;
    if( arguments.find( "--height" ) > 0 )
    {
        heightMap = true;
        arguments.read( "--height", heightMapName );
    }
    
    ///Grab the base map name
    bool baseMap( false );
    std::string baseMapName;
    if( arguments.find( "--base" ) > 0 )
    {
        baseMap = true;
        arguments.read( "--base", baseMapName );
    }
    
    if( arguments.read( "-v" ) || arguments.read( "--version" ) )
    {
        osg::notify( osg::ALWAYS ) << osgwTools::getVersionString() << std::endl << std::endl;
    }

    ///Now lets make the model
    osg::ref_ptr< osg::Group > root = new osg::Group();

    osg::ref_ptr< osg::Node > model = osgDB::readNodeFiles( arguments );
    if( model.get() == NULL )
    {
        osg::notify( osg::FATAL ) << "Unable to load model." << std::endl;
        arguments.getApplicationUsage()->
            write( osg::notify( osg::FATAL ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return( 1 );
    }

    root->addChild( model.get() );

    osg::ref_ptr< osg::Image > normalImage = osgDB::readImageFile( normalMapName );

    osg::ref_ptr< osg::Image > baseImage;
    if( baseMap )
    { 
        baseImage = osgDB::readImageFile( baseMapName );
    }

    osg::ref_ptr< osg::Image > heightImage;
    if( heightMap )
    {
       heightImage = osgDB::readImageFile( heightMapName );
    }

    ///Now setup the shaders
    addShaders( model.get(), baseImage.get(), normalImage.get(), heightImage.get() );

    osgViewer::Viewer viewer(arguments);
    // add the window size toggle handler
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);

    viewer.setSceneData( root.get() );
    return viewer.run();
}



/** \page osgwnormalmap The osgwnormalmap Application
osgwnormalmap maps the specified normal map to the loaded file. We are assuming
that texture unit 0 contains the base texture for the model.

\code
C:\Projects>osgwnormalmap --normal file.dds --height file2.dds file.ive
\endcode

\section clp Command Line Parameters
<table border="0">
  <tr>
    <td><b>--normal</b></td>
    <td>Supply a normal texture. This is required.</td>
  </tr>
  <tr>
    <td><b>--height</b></td>
    <td>Supply a height map if desired.</td>
  </tr>
  <tr>
    <td><b>--base</b></td>
    <td>Supply a base map if the loaded model does not contain a base texture in unit 0.</td>
  </tr>
  <tr>
    <td><b>-v/--version</b></td>
    <td>Display the osgWorks version string.</td>
  </tr>
</table>
*/
