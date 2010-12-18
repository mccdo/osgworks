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

#ifndef __OSGWTOOLS_NODE_PATH_UTILS__
#define __OSGWTOOLS_NODE_PATH_UTILS__ 1


#include "osgwTools/Export.h"
#include <osg/Node>

#include <string>
#include <vector>


namespace osgwTools
{


/** \defgroup NodePathUtils Utilities for NodePaths.
*/
/*@{*/


/** Documentation is TBD.
*/
struct NodeData
{
    NodeData();
    NodeData( unsigned int index, const osg::Node& node );

    osg::Node* findNode( osg::Group* parent ) const;

    bool operator==( const NodeData& rhs );
    bool operator!=( const NodeData& rhs );

    unsigned int _index;
    std::string _className;
    std::string _objectName;
};

/** Documentation is TBD.
*/
typedef std::vector< NodeData > IndexedNodePath;


/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT IndexedNodePath nodePathToIndexed( const osg::NodePath& nodePath );

/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT osg::NodePath indexedToNodePath( const IndexedNodePath& indexedNodePath, osg::Group* root );


/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT std::string indexedToString( const IndexedNodePath& indexedNodePath );

/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT IndexedNodePath stringToIndexed( const std::string& stringPath );


/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT std::string nodePathToString( const osg::NodePath& nodePath );

/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT osg::NodePath stringToNodePath( const std::string& stringPath, osg::Group* root );


/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT osg::Node* findNode( const IndexedNodePath& indexedNodePath, osg::Group* root );

/** Documentation is TBD.
*/
OSGWTOOLS_EXPORT osg::Node* findNode( const std::string& stringPath, osg::Group* root );


#ifdef OSGWORKS_BUILD_TESTS

/** Documentation is TBD.
\return 0 for success, 1 for failure.
*/
OSGWTOOLS_EXPORT int testNodePathUtils();

// OSGWORKS_BUILD_TESTS
#endif


/*@}*/


// namespace osgwTools
}

// __OSGWTOOLS_NODE_PATH_UTILS__
#endif
