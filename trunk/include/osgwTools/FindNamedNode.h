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

#ifndef __OSGWTOOLS_FIND_NAMED_NODE__
#define __OSGWTOOLS_FIND_NAMED_NODE__ 1


#include "osgwTools/Export.h"
#include <osg/NodeVisitor>
#include <string>


namespace osgwTools
{


// FindNamedNode -- Find all nodes with the given name. Calling code
// accesses the nodes and their paths using the public _napl member variable.
class OSGWTOOLS_EXPORT FindNamedNode : public osg::NodeVisitor
{
public:
    FindNamedNode( const std::string& name, const osg::NodeVisitor::TraversalMode travMode=osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN );
    ~FindNamedNode();

    typedef std::pair< osg::Node*, osg::NodePath > NodeAndPath;
    typedef std::vector< NodeAndPath > NodeAndPathList;
    NodeAndPathList _napl;

    void reset();

    typedef enum {
        EXACT_MATCH,
        CONTAINS
    } MatchMethod;
    void setMatchMethod( MatchMethod method );
    MatchMethod getMatchMethod() const;

    void apply( osg::Node& node );

protected:
    std::string _name;

    MatchMethod _method;
};

// namespace osgwTools
}

// __OSGWTOOLS_FIND_NAMED_NODE__
#endif
