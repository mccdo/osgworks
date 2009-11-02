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

#ifndef __OSGWTOOLS_INSERT_NODE__
#define __OSGWTOOLS_INSERT_NODE__ 1


#include "osgwTools/Export.h"
#include <osg/Node>
#include <osg/Group>


namespace osgwTools
{


// Insert the node 'newParent' into the scene graph hierarchy above
// the specified 'node'. After this call, 'node' has one parent (newParent)
// and all of the old parents of 'node' are now parents of 'newParent'.
OSGWTOOLS_EXPORT void insertAbove( osg::Node* node, osg::Group* newParent );

// Insert the node 'newChild' into the scene graph hierarchy below
// the specified 'parent'. After this call, 'parent' has one child (newChild)
// and all of the old children of 'parent' are now children of 'newChild'.
OSGWTOOLS_EXPORT void insertBelow( osg::Group* parent, osg::Group* newChild );

// Remove the specified node. Attaches all of node's children to each
// of its parents, and removes itself as a child from each of its
// parents.
OSGWTOOLS_EXPORT void removeNode( osg::Node* node );


// namespace osgwTools
}

// __OSGWTOOLS_INSERT_NODE__
#endif
