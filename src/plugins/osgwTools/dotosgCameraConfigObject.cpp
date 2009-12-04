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

#include "osgwTools/CameraConfigObject.h"

#include <iostream>

#include <osg/Matrix>
#include <osg/io_utils>

#include <osgDB/Registry>
#include <osgDB/Input>
#include <osgDB/Output>
#include <osgDB/ParameterOutput>

#include <vector>


bool CCO_readLocalData( osg::Object& obj, osgDB::Input& fr );
bool CCO_writeLocalData( const osg::Object& obj, osgDB::Output& fw );

osgDB::RegisterDotOsgWrapperProxy CCO_Proxy
(
    new osgwTools::CameraConfigObject,
    "CameraConfigObject",
    "Object CameraConfigObject",
    CCO_readLocalData,
    CCO_writeLocalData
);


bool readMatrix( osg::Matrix& matrix, osgDB::Input& fr, const char* keyword="Matrix" );
bool writeMatrix( const osg::Matrixd& matrix, osgDB::Output& fw, const char* keyword="Matrix" );


void
readConfigInfo( osgwTools::CameraConfigInfo& cci, osgDB::Input& fr )
{
    int version;
    if( fr[0].getStr() == std::string( "Version" ) )
    {
        fr[1].getInt( version );
        fr+=2;
    }

    osg::Matrix m;
    if( readMatrix( m, fr, "ViewOffset" ))
        cci._viewOffset = m;
    if( readMatrix( m, fr, "ProjectionOffset" ))
        cci._projectionOffset = m;
}
void
writeConfigInfo( const osgwTools::CameraConfigInfo& cci, osgDB::Output& fw )
{
    fw.indent() << "Version " << cci._version << std::endl;
    writeMatrix( cci._viewOffset, fw, "ViewOffset" );
    writeMatrix( cci._projectionOffset, fw, "ProjectionOffset" );
}

bool CCO_readLocalData( osg::Object& obj, osgDB::Input& fr )
{
    osgwTools::CameraConfigObject& cco = static_cast< osgwTools::CameraConfigObject& >( obj );
    bool advance( false );

    if( fr[0].getStr() == std::string( "Size" ) )
    {
        int sz;
        fr[1].getInt( sz );
        cco._slaveConfigInfo.resize( sz );
        fr+=2;
        advance = true;
    }
    unsigned int idx;
    for( idx=0; idx<cco._slaveConfigInfo.size(); idx++ )
    {
        readConfigInfo( cco._slaveConfigInfo[ idx ], fr );
    }
    
    return( advance );
}

bool CCO_writeLocalData( const osg::Object& obj, osgDB::Output& fw )
{
    const osgwTools::CameraConfigObject& cco = static_cast< const osgwTools::CameraConfigObject& >( obj );

    fw.indent() << "Size " << cco._slaveConfigInfo.size() << std::endl;
    unsigned int idx;
    for( idx=0; idx<cco._slaveConfigInfo.size(); idx++ )
    {
        writeConfigInfo( cco._slaveConfigInfo[ idx ], fw );
    }

    return( true );
}
