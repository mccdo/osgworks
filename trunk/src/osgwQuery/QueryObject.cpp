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

#include <osgwQuery/QueryObject.h>
#include <osgwQuery/QueryAPI.h>
#include <osg/Notify>


namespace osgwQuery
{


QueryObject::QueryObject( unsigned int numIDs )
  : osg::Object(),
    _numIDs( numIDs )
{
}
QueryObject::QueryObject( const QueryObject& rhs, const osg::CopyOp& copyop )
  : osg::Object( rhs, copyop ),
    _numIDs( rhs._numIDs )
{
}
QueryObject::~QueryObject()
{
}


GLuint QueryObject::getID( unsigned int contextID, unsigned int queryIDIndex )
{
    if( queryIDIndex >= _numIDs )
    {
        osg::notify( osg::WARN ) << "QueryObject::getID queryIDIndex (" << queryIDIndex << ") >= _numIDs (" << _numIDs << ")." << std::endl;
        return( 0 );
    }

    QueryIDVector& idVec = _ids[ contextID ];
    if( idVec.size() == 0 )
    {
        idVec.resize( _numIDs );
        QueryAPI* qapi = getQueryAPI( contextID );
        qapi->glGenQueries( _numIDs, &( idVec[0] ) );
    }

    return( idVec[ queryIDIndex ] );
}

void QueryObject::deleteIDs( unsigned int contextID )
{
    QueryIDVector& idVec = _ids[ contextID ];
    if( idVec.size() == 0 )
        return;

    QueryAPI* qapi = getQueryAPI( contextID );
    qapi->glDeleteQueries( _numIDs, &( idVec[0] ) );
    idVec.resize( 0 );
}


// osgwQuery
}
