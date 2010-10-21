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

#include "osgwTools/FindNamedNode.h"
#include <osg/NodeVisitor>

#include <string>


namespace osgwTools
{


FindNamedNode::FindNamedNode( const std::string& name, const osg::NodeVisitor::TraversalMode travMode )
  : osg::NodeVisitor( travMode ),
    _name( name ),
    _method( EXACT_MATCH )
{
}

FindNamedNode::~FindNamedNode()
{
}

void
FindNamedNode::reset()
{
    _napl.clear();
}

void
FindNamedNode::setMatchMethod( MatchMethod method )
{
    _method = method;
}
FindNamedNode::MatchMethod
FindNamedNode::getMatchMethod() const
{
    return( _method );
}

void
FindNamedNode::apply( osg::Node& node )
{
    bool match = (
        ( ( _method == EXACT_MATCH ) &&
            ( node.getName() == _name ) ) ||
        ( ( _method == CONTAINS ) &&
            ( node.getName().find( _name ) != std::string::npos ) ) );
    if( match )
    {
        NodeAndPath nap( &node, getNodePath() );
        _napl.push_back( nap );
    }
    traverse( node );
}

// namespace osgwTools
}
