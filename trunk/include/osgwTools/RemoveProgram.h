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


/**
Visitor to strip osg::Program objects and osg::Uniform objects out of
a scene graph's StateSet objects.
*/
class OSGWTOOLS_EXPORT RemoveProgram : public osg::NodeVisitor
{
public:
    /**
    Contructor to configure the RemoveProgram visitor. By default, RemoveProgram
    removed programs, doesn't remove uniforms, and traverses all children. This
    ehavior is configurable by dpecifying your own values for the default parameters.
    @param removePrograms Default: true.
    @param removePrograms Default: false.
    @param travMode The traversal mode. Default: osg::NodeVisitor::TRAVERSE_ALL_CHILDREN.
    */
    RemoveProgram( bool removePrograms=true, bool removeUniforms=false, const osg::NodeVisitor::TraversalMode travMode=osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
    ~RemoveProgram();

    /**
    Sets _programCount and _uniformCount to 0. /see _programCount. /see _uniformCount.
    */
    void reset();

    /**
    Override for base class apply() method.
    */
    virtual void apply( osg::Node& node );
    /**
    Override for base class apply() method.
    */
    virtual void apply( osg::Geode& node );

    /**
    Specify whether or not to remove programs. Default: true (remove programs).
    */
    void setRemovePrograms( bool removePrograms );
    /**
    Specify whether or not to remove uniforms. Default: false (don'tbremove uniforms).
    */
    void setRemoveUniforms( bool removeUniforms );

    /**
    During traversal, these counters track the total number of programs
    and uniforms removed. They are public so that calling code can access
    them directly following the traversal.
    */
    unsigned int _programCount, _uniformCount;

protected:
    void processStateSet( osg::StateSet* ss );

    bool _removePrograms, _removeUniforms;
};


// osgwTools
}

// __OSGWTOOLS_REMOVE_PROGRAM_H__
#endif
