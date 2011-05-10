//
// Copyright (c) 2008-2009 Skew Matrix Software LLC.
// All rights reserved.
//

#ifndef __OSGWTOOLS_COUNTS_VISITOR_H__
#define __OSGWTOOLS_COUNTS_VISITOR_H__

#include <osgwTools/Export.h>
#include <osg/NodeVisitor>
#include <set>
#include <iostream>


namespace osgwTools
{


class OSGWTOOLS_EXPORT CountsVisitor : public osg::NodeVisitor
{
public:
    CountsVisitor( osg::NodeVisitor::TraversalMode mode = osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN );
    ~CountsVisitor();

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

    void apply( osg::StateSet* stateSet );

    int getVertices() const;
    int getDrawArrays() const;

protected:
    int _depth;
    int _maxDepth;

    int _nodes;
    int _groups;
    int _lods;
    int _pagedLods;
    int _switches;
    int _sequences;
    int _transforms;
    int _matrixTransforms;
    int _dofTransforms;
    int _geodes;
    int _drawables;
    int _geometries;
    int _nullGeometries;
    int _texts;
    int _vertices;
    int _stateSets;
    int _emptyStateSets;
    int _uniforms;
    int _programs;
    int _attributes;
    int _texAttributes;
    int _modes;
    int _texModes;
    int _textures;
    int _primitiveSets;
    int _drawArrays;

    int _totalChildren;
    int _slowPathGeometries;

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
