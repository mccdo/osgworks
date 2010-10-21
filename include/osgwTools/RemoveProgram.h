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

#ifndef __OSGWTOOLS_REMOVE_PROGRAM_H__
#define __OSGWTOOLS_REMOVE_PROGRAM_H__ 1


#include "osgwTools/Export.h"
#include <osg/NodeVisitor>

#include <string>



namespace osgwTools
{


// RemoveProgram - Visitor to strip osg::Program objects out of
// a scene graph's StateSet objects. Optionally also removed Uniforms.
class OSGWTOOLS_EXPORT RemoveProgram : public osg::NodeVisitor
{
public:
    RemoveProgram( bool removeUniforms=false, const osg::NodeVisitor::TraversalMode travMode=osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
    ~RemoveProgram();

    void reset();

    virtual void apply( osg::Node& node );
    virtual void apply( osg::Geode& node );

    unsigned int _programCount, _uniformCount;

protected:
    void processStateSet( osg::StateSet* ss );

    bool _removeUniforms;
};


// osgwTools
}

// __OSGWTOOLS_REMOVE_PROGRAM_H__
#endif
