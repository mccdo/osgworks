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

#include <osgwQuery/QueryAPI.h>
#include <osg/buffered_value>
#include <osg/ref_ptr>
#include <osg/GLExtensions>


namespace osgwQuery
{


static osg::buffered_value< osg::ref_ptr< QueryAPI > > s_query_api;

QueryAPI* getQueryAPI( unsigned int contextID )
{
    if( !( s_query_api[ contextID ] ) )
        s_query_api[ contextID ] = new QueryAPI( contextID );
    return( s_query_api[ contextID ].get() );
}


QueryAPI::QueryAPI( unsigned int contextID )
  : osg::Referenced(),
    _initialized( false ),
    _queryAPISupported( false ),
    _timerQuerySupported( false ),
    _transformFeedback3Supported( false ),
    _transformFeedbackSupported( false ),
    _occlusionQuery2Supported( false )
{
}
QueryAPI::~QueryAPI()
{
}


void QueryAPI::internalInit( unsigned int contextID )
{
    bool _queryAPISupported = osg::isGLExtensionOrVersionSupported( contextID, "GL_ARB_occlusion_query", 1.5f );
    if( !_queryAPISupported )
        osg::notify( osg::WARN ) << "osgwQuery: Query API is unavailable. GL v" << osg::getGLVersionNumber() << std::endl;
    else
    {
        _beginQuery = (BeginQueryProc)( osg::getGLExtensionFuncPtr( "glBeginQuery", "glBeginQueryARB" ) );
        _endQuery = (EndQueryProc)( osg::getGLExtensionFuncPtr( "glEndQuery", "glEndQueryARB" ) );
        _genQueries = (GenQueriesProc)( osg::getGLExtensionFuncPtr( "glGenQueries", "glGenQueriesARB" ) );
        _deleteQueries = (DeleteQueriesProc)( osg::getGLExtensionFuncPtr( "glDeleteQueries", "glDeleteQueriesARB" ) );
        _isQuery = (IsQueryProc)( osg::getGLExtensionFuncPtr( "glIsQuery", "glIsQueryARB" ) );
        _getQueryiv = (GetQueryivProc)( osg::getGLExtensionFuncPtr( "glGetQueryiv", "glGetQueryivARB" ) );
        _getQueryObjectiv = (GetQueryObjectivProc)( osg::getGLExtensionFuncPtr( "glGetQueryObjectiv", "glGetQueryObjectivARB" ) );
        _getQueryObjectuiv = (GetQueryObjectuivProc)( osg::getGLExtensionFuncPtr( "glGetQueryObjectuiv", "glGetQueryObjectuivARB" ) );
    }

    bool _timerQuerySupported = osg::isGLExtensionOrVersionSupported( contextID, "GL_ARB_timer_query", 3.3f );
    if( !_timerQuerySupported )
        osg::notify( osg::WARN ) << "osgwQuery: Time query is unavailable. GL v" << osg::getGLVersionNumber() << std::endl;
    else
    {
        _getQueryObjecti64v = (GetQueryObjecti64vProc)( osg::getGLExtensionFuncPtr( "glGetQueryObjecti64v", "glGetQueryObjecti64vARB" ) );
        _getQueryObjectui64v = (GetQueryObjectui64vProc)( osg::getGLExtensionFuncPtr( "glGetQueryObjectui64v", "glGetQueryObjectui64vARB" ) );
    }

    bool _transformFeedback3Supported = osg::isGLExtensionOrVersionSupported( contextID, "GL_ARB_transform_feedback3", 4.0f );
    if( !_transformFeedback3Supported )
        osg::notify( osg::WARN ) << "osgwQuery: Transform feedback 3 is unavailable. GL v" << osg::getGLVersionNumber() << std::endl;
    else
    {
        _beginQueryIndexed = (BeginQueryIndexedProc)( osg::getGLExtensionFuncPtr( "glBeginQueryIndexed", "glBeginQueryIndexedARB" ) );
        _endQueryIndexed = (EndQueryIndexedProc)( osg::getGLExtensionFuncPtr( "glEndQueryIndexed", "glEndQueryIndexedARB" ) );
        _getQueryIndexediv = (GetQueryIndexedivProc)( osg::getGLExtensionFuncPtr( "glGetQueryIndexediv", "glGetQueryIndexedivARB" ) );
    }

    bool _transformFeedbackSupported = osg::isGLExtensionOrVersionSupported( contextID, "GL_EXT_transform_feedback", 3.0f );
    bool _occlusionQuery2Supported = osg::isGLExtensionOrVersionSupported( contextID, "GL_ARB_occlusion_query2", 3.3f );

    _initialized = true;
}


// osgwQuery
}
