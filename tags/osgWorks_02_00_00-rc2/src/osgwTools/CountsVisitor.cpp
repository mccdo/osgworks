//
// Copyright (c) 2008-2009 Skew Matrix Software LLC.
// All rights reserved.
//

#include <osgwTools/CountsVisitor.h>
#include <osgwTools/StateSetUtils.h>
#include <osg/LOD>
#include <osg/PagedLOD>
#include <osg/Switch>
#include <osg/Sequence>
#include <osg/Transform>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Texture>
#include <osg/Program>
#include <osg/Uniform>
#include <osg/Geometry>
#include <osgSim/DOFTransform>
#include <osgText/Text>
#include <iostream>
#include <map>


namespace osgwTools
{


typedef std::map< int, int > ModeCounter;
static ModeCounter mc;


CountsVisitor::CountsVisitor( osg::NodeVisitor::TraversalMode mode )
  : osg::NodeVisitor( mode ),
    _countUserMode( false ),
    _countUserAttr( false )
{
    reset();
}

CountsVisitor::~CountsVisitor()
{
}

unsigned int CountsVisitor::getVertices() const
{
    return( _vertices );
}
unsigned int CountsVisitor::getDrawArrays() const
{
    return( _drawArrays );
}
unsigned int CountsVisitor::getTotalDrawables() const
{
    return( _totalDrawables );
}
unsigned int CountsVisitor::getNumDrawablesUserModeOff() const
{
    return( _drawUserModeOff );
}

void CountsVisitor::setUserMode( GLenum userMode )
{
    _userMode = userMode;
    _countUserMode = true;
}
void CountsVisitor::setUserAttribute( osg::StateAttribute::Type userAttr )
{
    _userAttr = userAttr;
    _countUserAttr = true;
}


void
CountsVisitor::reset()
{
    _depth = 0;
    _maxDepth = 0;

    _nodes = 0;
    _groups = 0;
    _lods = 0;
    _pagedLods = 0;
    _switches = 0;
    _sequences = 0;
    _transforms = 0;
    _matrixTransforms = 0;
    _dofTransforms = 0;
    _geodes = 0;
    _totalDrawables = 0;
    _drawables = 0;
    _geometries = 0;
    _nullGeometries = 0;
    _texts = 0;
    _vertices = 0;
    _stateSets = 0;
    _emptyStateSets = 0;
    _uniforms = 0;
    _programs = 0;
    _attributes = 0;
    _modes = 0;
    _texAttributes = 0;
    _texModes = 0;
    _textures = 0;
    _primitiveSets = 0;
    _drawArrays = 0;

    _totalUserModes = 0;
    _totalUserAttrs = 0;
    _drawUserModeOn = 0;
    _drawUserModeOff = 0;
    _drawUserModeNotSet = 0;

    _totalChildren = 0;
    _slowPathGeometries = 0;

    _uNodes.clear();
    _uGroups.clear();
    _uLods.clear();
    _uPagedLods.clear();
    _uSwitches.clear();
    _uSequences.clear();
    _uTransforms.clear();
    _uMatrixTransforms.clear();
    _uDofTransforms.clear();
    _uGeodes.clear();
    _uDrawables.clear();
    _uGeometries.clear();
    _uTexts.clear();
    _uVertices.clear();
    _uStateSets.clear();
    _uUniforms.clear();
    _uPrograms.clear();
    _uAttributes.clear();
    _uTexAttributes.clear();
    _uTextures.clear();
    _uPrimitiveSets.clear();
    _uDrawArrays.clear();
}

void
CountsVisitor::dump( std::ostream& ostr )
{
    ostr << std::endl;
    ostr << "         OSG Object \tCount\tUnique" << std::endl;
    ostr << "         ---------- \t-----\t------" << std::endl;
    ostr << "             Groups \t" << _groups << "\t" << _uGroups.size() << std::endl;
    ostr << "               LODs \t" << _lods << "\t" << _uLods.size() << std::endl;
    ostr << "          PagedLODs \t" << _pagedLods << "\t" << _uPagedLods.size() << std::endl;
    ostr << "           Switches \t" << _switches << "\t" << _uSwitches.size() << std::endl;
    ostr << "          Sequences \t" << _sequences << "\t" << _uSequences.size() << std::endl;
    ostr << "   MatrixTransforms \t" << _matrixTransforms << "\t" << _uMatrixTransforms.size() << std::endl;
    ostr << "      DOFTransforms \t" << _dofTransforms << "\t" << _uDofTransforms.size() << std::endl;
    ostr << "   Other Transforms \t" << _transforms << "\t" << _uTransforms.size() << std::endl;
    ostr << "             Geodes \t" << _geodes << "\t" << _uGeodes.size() << std::endl;
    ostr << "        Other Nodes \t" << _nodes << "\t" << _uNodes.size() << std::endl;
    ostr << "    Empty StateSets \t" << _emptyStateSets << std::endl;
    ostr << "    Total StateSets \t" << _stateSets << "\t" << _uStateSets.size() << std::endl;
    ostr << "           Programs \t" << _programs << "\t" << _uPrograms.size() << std::endl;
    ostr << "           Uniforms \t" << _uniforms << "\t" << _uUniforms.size() << std::endl;
    if( _countUserMode )
        ostr << "         User Modes \t" << _totalUserModes << std::endl;
    if( _countUserAttr )
        ostr << "    User Attributes \t" << _totalUserAttrs << std::endl;
    ostr << "   Total Attributes \t" << _attributes << "\t" << _uAttributes.size() << std::endl;
    ostr << "        Total Modes \t" << _modes << std::endl;
    ostr << "           Textures \t" << _textures << "\t" << _uTextures.size() << std::endl;
    ostr << "Total TexAttributes \t" << _texAttributes << "\t" << _uTexAttributes.size() << std::endl;
    ostr << "     Total TexModes \t" << _texModes << std::endl;
    ostr << "    NULL Geometries \t" << _nullGeometries << std::endl;
    ostr << "   Total Geometries \t" << _geometries << "\t" << _uGeometries.size() << std::endl;
    ostr << "              Texts \t" << _texts << "\t" << _uTexts.size() << std::endl;
    ostr << "    Other Drawables \t" << _drawables << "\t" << _uDrawables.size() << std::endl;
    ostr << "    Totol Drawables \t" << _totalDrawables << std::endl;
    ostr << "         DrawArrays \t" << _drawArrays << "\t" << _uDrawArrays.size() << std::endl;
    ostr << "Total PrimitiveSets \t" << _primitiveSets << "\t" << _uPrimitiveSets.size() << std::endl;
    if( _countUserMode )
    {
        ostr << "Drawables with user Modes:" << std::endl;
        ostr << "            Enabled \t" << _drawUserModeOn << std::endl;
        ostr << "           Disabled \t" << _drawUserModeOff << std::endl;
        ostr << "            Not set \t" << _drawUserModeNotSet << std::endl;
    }

    if (_slowPathGeometries)
        ostr << "Slow path Geometries: " << _slowPathGeometries << std::endl;
    float avgChildren = (float)_totalChildren / (float)(_nodes+_groups+_lods+_pagedLods+_switches+_sequences+_transforms+_matrixTransforms+_dofTransforms);
    ostr << "Average children per node: " << avgChildren << std::endl;

    ostr << "Total vertices: " << _vertices << std::endl;
    ostr << "Max depth: " << _maxDepth << std::endl;
}


void CountsVisitor::apply( osg::Drawable* draw )
{
    apply( draw->getStateSet() );

    pushStateSet( draw->getStateSet() );

    if( _countUserMode )
    {
        if( isSet( _userMode, _stateStack.back().get() ) )
        {
            if( isEnabled( _userMode, _stateStack.back().get() ) )
                _drawUserModeOn++;
            else
                _drawUserModeOff++;
        }
        else
            _drawUserModeNotSet++;
    }

    _totalDrawables++;
    osg::Geometry* geom;
    if (dynamic_cast<osgText::Text*>( draw ) != NULL)
    {
        _texts++;
        osg::ref_ptr<osg::Object> rp = (osg::Object*)draw;
        _uTexts.insert( rp );
    }
    else if ( (geom = dynamic_cast<osg::Geometry*>( draw )) != NULL)
    {
        _geometries++;
        osg::ref_ptr<osg::Object> rp = (osg::Object*)geom;
        _uGeometries.insert( rp );

        if (!geom->areFastPathsUsed())
            _slowPathGeometries++;

        if( geom->getNumPrimitiveSets() > 0 )
        {
            unsigned int idx;
            for( idx=0; idx < geom->getNumPrimitiveSets(); idx++ )
            {
                osg::PrimitiveSet* ps = geom->getPrimitiveSet( idx );
                _vertices += ps->getNumIndices();
            }
        }
        else
            _nullGeometries++;
        osg::ref_ptr<osg::Object> rpv = (osg::Object*)( geom->getVertexArray() );
        _uVertices.insert( rpv );

        if( geom->getNumPrimitiveSets() > 0 )
        {
            _primitiveSets += geom->getNumPrimitiveSets();
            osg::Geometry::PrimitiveSetList& psl = geom->getPrimitiveSetList();
            osg::Geometry::PrimitiveSetList::const_iterator pslit;
            for( pslit = psl.begin(); pslit != psl.end(); pslit++ )
            {
                osg::ref_ptr<osg::Object> rpps = (osg::Object*)( pslit->get() );
                _uPrimitiveSets.insert( rpps );
                const osg::DrawArrays* da = dynamic_cast< const osg::DrawArrays* >( pslit->get() );
                if( da )
                {
                    _drawArrays++;
                    osg::ref_ptr<osg::Object> rpda = (osg::Object*)( da );
                    _uDrawArrays.insert( rpda );
                }
            }
        }
    }
    else
    {
        _drawables++;
        osg::ref_ptr<osg::Object> rp = (osg::Object*)draw;
        _uDrawables.insert( rp );
    }

    popStateSet();
}
void CountsVisitor::apply( osg::StateSet* stateSet )
{
    if( stateSet == NULL )
        return;
    _stateSets++;
    osg::ref_ptr<osg::Object> ssrp = (osg::Object*)stateSet;
    _uStateSets.insert( ssrp );

    if( osgwTools::isEmpty( *stateSet ) )
        _emptyStateSets++;

    const osg::StateSet::TextureAttributeList& tal = stateSet->getTextureAttributeList();
    const osg::StateSet::TextureModeList tml = stateSet->getTextureModeList();
    unsigned int idx;
    for( idx=0; idx<tal.size(); idx++ )
    {
        if( tal.size() > 0 )
        {
            const osg::StateSet::AttributeList& al = tal[ idx ];
            _texAttributes += al.size();
            osg::StateSet::AttributeList::const_iterator ait;
            for( ait=al.begin(); ait!=al.end(); ait++ )
            {
                osg::ref_ptr<osg::Object> arp = (osg::Object*)( ait->second.first.get() );
                _uTexAttributes.insert( arp );
            }
        }

        if( tml.size() > 0 )
            _texModes += tml[ idx ].size();

        osg::Texture* texture = static_cast< osg::Texture* >(
            stateSet->getTextureAttribute( idx, osg::StateAttribute::TEXTURE ) );
        if( texture != NULL )
        {
            _textures++;
            osg::ref_ptr<osg::Object> trp = (osg::Object*)texture;
            _uTextures.insert( trp );
        }
    }

    const osg::StateSet::AttributeList& al = stateSet->getAttributeList();
    _attributes += al.size();
    osg::StateSet::AttributeList::const_iterator ait;
    for( ait=al.begin(); ait!=al.end(); ait++ )
    {
        osg::StateAttribute* sa = ait->second.first.get();
        if( _countUserAttr && ( sa->getType() == _userAttr ) )
            _totalUserAttrs++;

        osg::ref_ptr<osg::Object> arp = (osg::Object*)( sa );
        _uAttributes.insert( arp );
    }

    _modes += stateSet->getModeList().size();
    {
        const osg::StateSet::ModeList& ml = stateSet->getModeList();
        osg::StateSet::ModeList::const_iterator it;
        for( it=ml.begin(); it != ml.end(); it++ )
        {
            if( _countUserMode && ( it->first == _userMode ) )
                _totalUserModes++;

            mc[ it->first ] += 1;
        }
    }

    osg::Program* program = static_cast< osg::Program* >(
        stateSet->getAttribute( osg::StateAttribute::PROGRAM ) );
    if( program != NULL )
    {
        _programs++;
        osg::ref_ptr<osg::Object> prp = (osg::Object*)program;
        _uPrograms.insert( prp );
    }

    const osg::StateSet::UniformList ul = stateSet->getUniformList();
    _uniforms += ul.size();
    osg::StateSet::UniformList::const_iterator it;
    for( it=ul.begin(); it!=ul.end(); it++ )
    {
        osg::ref_ptr<osg::Object> urp = (osg::Object*)( it->second.first.get() );
        _uUniforms.insert( urp );
    }
}

void
CountsVisitor::apply( osg::Node& node )
{
    pushStateSet( node.getStateSet() );

    _nodes++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uNodes.insert( rp );
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::Group& node )
{
    pushStateSet( node.getStateSet() );

    _groups++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uGroups.insert( rp );
    _totalChildren += node.getNumChildren();
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::LOD& node )
{
    pushStateSet( node.getStateSet() );

    _lods++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uLods.insert( rp );
    _totalChildren += node.getNumChildren();
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::PagedLOD& node )
{
    pushStateSet( node.getStateSet() );

    osg::Group* grp = node.getParent(0);
    osg::Group* gPar = NULL;
    if (grp)
        gPar = grp->getParent(0);
    apply( node.getStateSet() );

    _pagedLods++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uPagedLods.insert( rp );
    _totalChildren += node.getNumChildren();

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::Switch& node )
{
    pushStateSet( node.getStateSet() );

    _switches++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uSwitches.insert( rp );
    _totalChildren += node.getNumChildren();
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::Sequence& node )
{
    pushStateSet( node.getStateSet() );

    _sequences++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uSequences.insert( rp );
    _totalChildren += node.getNumChildren();
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::Transform& node )
{
    pushStateSet( node.getStateSet() );

    if (dynamic_cast<osgSim::DOFTransform*>( &node ) != NULL)
    {
        _dofTransforms++;
        osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
        _uDofTransforms.insert( rp );
    }
    else
    {
        _transforms++;
        osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
        _uTransforms.insert( rp );
    }
    _totalChildren += node.getNumChildren();
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::MatrixTransform& node )
{
    pushStateSet( node.getStateSet() );

    _matrixTransforms++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uMatrixTransforms.insert( rp );
    _totalChildren += node.getNumChildren();
    apply( node.getStateSet() );

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}

void
CountsVisitor::apply( osg::Geode& node )
{
    pushStateSet( node.getStateSet() );

    _geodes++;
    osg::ref_ptr<osg::Object> rp = (osg::Object*)&node;
    _uGeodes.insert( rp );
    apply( node.getStateSet() );

    unsigned int idx;
    for (idx=0; idx<node.getNumDrawables(); idx++)
    {
        osg::Drawable* draw = node.getDrawable( idx );
        apply( draw );
    }

    if (++_depth > _maxDepth)
        _maxDepth = _depth;
    traverse( (osg::Node&)node );
    _depth--;

    popStateSet();
}


void CountsVisitor::pushStateSet( osg::StateSet* ss )
{
    if( ss == NULL )
        ss = new osg::StateSet;

    if( _stateStack.size() > 0 )
    {
        osg::StateSet* oldTop = _stateStack.back().get();
        osg::StateSet* newTop = new osg::StateSet( *oldTop );
        newTop->merge( *ss );
        _stateStack.push_back( newTop );
    }
    else
    {
        _stateStack.push_back( ss );
    }
}
void CountsVisitor::popStateSet()
{
    if( _stateStack.size() > 0 )
        _stateStack.pop_back();
    else
        osg::notify( osg::WARN ) << "osgw: CountsVisitor: State stack underflow." << std::endl;
}

bool CountsVisitor::isSet( GLenum stateItem, osg::StateSet* ss )
{
    if( ss == NULL )
        return( false );

    // StateSet is not NULL. Query the mode.
    osg::StateAttribute::GLModeValue mode;
    mode = ss->getMode( stateItem );

    // The item is set if the mode is anything other than INHERIT.
    return( mode != osg::StateAttribute::INHERIT );
}
bool CountsVisitor::isEnabled( GLenum stateItem, osg::StateSet* ss )
{
    if( ss == NULL )
        // Calling code must use isSet() to verify a mode is set, then
        // call isEnabled() to see if the mode is enabled or not. We return
        // false here, but calling code should never call us with a NULL
        // StateSet, as isSet() would have returned false.
        return( false );

    // StateSet is not NULL. Query the mode.
    osg::StateAttribute::GLModeValue mode;
    mode = ss->getMode( stateItem );

    if( mode & osg::StateAttribute::ON )
        // Item is enabled if its value is ON.
        return( true );
    else
        // If it's not enabled, then it's off or not set.
        return( false );
}


// osgwTools
}
