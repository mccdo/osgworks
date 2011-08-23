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

#ifndef __OSGWTOOLS_COUNTS_VISITOR_H__
#define __OSGWTOOLS_COUNTS_VISITOR_H__

#include <osgwTools/Export.h>
#include <osg/NodeVisitor>
#include <deque>
#include <set>
#include <iostream>


namespace osgwTools
{


/** \class CountsVisitor CountsVisitor.h <osgwTools/CountsVisitor.h>
\brief Accumulate information about a scene graph.

TBD To be done: We should be able to "addUserMode/addUserAttribute" instead of just "set"
so that we can accumulate counts for multiple modes and attributes. There needs to be an
analogous system for adding texture modes and attributes. The dump() method should be able
to display an arbitrary GLenum mode as ASCII string (without a dependency on GLU).
*/
class OSGWTOOLS_EXPORT CountsVisitor : public osg::NodeVisitor
{
public:
    CountsVisitor( osg::NodeVisitor::TraversalMode mode = osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN );
    ~CountsVisitor();

    void setUserMode( GLenum userMode );
    void setUserAttribute( osg::StateAttribute::Type userAttr );

    void reset();

    void dump( std::ostream& ostr=std::cout );

    void apply( osg::Node& node );
    void apply( osg::Group& node );
    void apply( osg::LOD& node );
    void apply( osg::PagedLOD& node );
    void apply( osg::Switch& node );
    void apply( osg::Sequence& node );
    void apply( osg::Transform& node );
    void apply( osg::MatrixTransform& node );
    void apply( osg::Geode& node );

    void apply( osg::Drawable* draw );
    void apply( osg::StateSet* stateSet );

    unsigned int getVertices() const;
    unsigned int getDrawArrays() const;
    unsigned int getTotalDrawables() const;
    unsigned int getNumDrawablesUserModeOff() const;

protected:
    std::deque< osg::ref_ptr< osg::StateSet > > _stateStack;
    void pushStateSet( osg::StateSet* ss );
    void popStateSet();

    bool isSet( GLenum stateItem, osg::StateSet* ss );
    bool isEnabled( GLenum stateItem, osg::StateSet* ss );

    GLenum _userMode;
    bool _countUserMode;
    osg::StateAttribute::Type _userAttr;
    bool _countUserAttr;

    int _depth;
    int _maxDepth;

    unsigned int _nodes;
    unsigned int _groups;
    unsigned int _lods;
    unsigned int _pagedLods;
    unsigned int _switches;
    unsigned int _sequences;
    unsigned int _transforms;
    unsigned int _matrixTransforms;
    unsigned int _dofTransforms;
    unsigned int _geodes;
    unsigned int _drawables;
    unsigned int _geometries;
    unsigned int _nullGeometries;
    unsigned int _texts;
    unsigned int _totalDrawables;
    unsigned int _vertices;
    unsigned int _stateSets;
    unsigned int _emptyStateSets;
    unsigned int _uniforms;
    unsigned int _programs;
    unsigned int _attributes;
    unsigned int _texAttributes;
    unsigned int _modes;
    unsigned int _texModes;
    unsigned int _textures;
    unsigned int _primitiveSets;
    unsigned int _drawArrays;

    unsigned int _totalUserModes;
    unsigned int _totalUserAttrs;
    unsigned int _drawUserModeOn;
    unsigned int _drawUserModeOff;
    unsigned int _drawUserModeNotSet;

    unsigned int _totalChildren;
    unsigned int _slowPathGeometries;

    typedef std::set< osg::ref_ptr<osg::Object> > ObjectSet;
    ObjectSet _uNodes;
    ObjectSet _uGroups;
    ObjectSet _uLods;
    ObjectSet _uPagedLods;
    ObjectSet _uSwitches;
    ObjectSet _uSequences;
    ObjectSet _uTransforms;
    ObjectSet _uMatrixTransforms;
    ObjectSet _uDofTransforms;
    ObjectSet _uGeodes;
    ObjectSet _uDrawables;
    ObjectSet _uGeometries;
    ObjectSet _uTexts;
    ObjectSet _uVertices;
    ObjectSet _uStateSets;
    ObjectSet _uUniforms;
    ObjectSet _uPrograms;
    ObjectSet _uAttributes;
    ObjectSet _uTexAttributes;
    ObjectSet _uTextures;
    ObjectSet _uPrimitiveSets;
    ObjectSet _uDrawArrays;
};

// osgwTools
}

// __OSGWTOOLS_COUNTS_VISITOR_H__
#endif
