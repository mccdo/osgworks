/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2011 by Kenneth Mark Bryden
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.F
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

#ifndef __OSGWTOOLS_TRANSPARENCY_UTILS_H__
#define __OSGWTOOLS_TRANSPARENCY_UTILS_H__ 1


#include "osgwTools/Export.h"
#include <osg/NodeVisitor>
#include <osg/BlendColor>
#include <osg/BlendFunc>

#include <string>


namespace osgwTools
{



// Forward declaration
class RestoreOpacityVisitor;


/** \defgroup TransparencyUtils Utilities for rendering transparent geometry.
*/
/*@{*/


// When enabling transparency on a Node or Drawable that has no StateSet,
// we assign this name to the newly created StateSet. When transparency
// is later disabled, if the name matches, we delete the StateSet.
static std::string s_magicStateSetName( "TransparentDeleteMe" );



/** \brief Set a transparent StateSet on the given Node or Drawable, saving its current StateSet as UserData.
If UserData is NULL, current StateSet is saved as UserData for later restore.
If there is no current StateSet, one is created.
The current StateSet is modified as follows:
\li BlendColor is set using the specified alpha value.
\li BlendFunc is set to use the BlendColor alpha.
\li The rendering hint is set to TRANSPARENT_BIN.
*/
template< class T >
bool transparentEnable( T* nodeOrDrawable, float alpha )
{
    if( nodeOrDrawable == NULL )
        return( false );

    osg::StateSet* stateSet( nodeOrDrawable->getStateSet() );
    if( ( stateSet != NULL ) &&
        ( nodeOrDrawable->getUserData() == NULL ) )
    {
        // We have a StateSet, and UserData is NULL, so make a copy of the StateSet.
        // We'll store the original StateSet as UserData (for later restore) and modify
        // the copy.
        nodeOrDrawable->setUserData( stateSet );
        stateSet = new osg::StateSet( *( stateSet ), osg::CopyOp::DEEP_COPY_ALL );
        nodeOrDrawable->setStateSet( stateSet );
    }
    else if( stateSet == NULL )
    {
        // This node doesn't have a StateSet, so we create one and tag it
        // with the magic name for later deletion.
        stateSet = new osg::StateSet();
        stateSet->setName( s_magicStateSetName );
        nodeOrDrawable->setStateSet( stateSet );
    }

    const osg::StateAttribute::GLModeValue modeValue = 
        osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;

    osg::BlendColor* bc = new osg::BlendColor( osg::Vec4( 0., 0., 0., alpha ) );
    stateSet->setAttributeAndModes( bc, modeValue );
    osg::BlendFunc* bf = new osg::BlendFunc( osg::BlendFunc::CONSTANT_ALPHA,
        osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA );
    stateSet->setAttributeAndModes( bf, modeValue );
    stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    return( true );
}

/** \brief Restores opacity by undoing the effects of a prior call to transparentEnable.
If the Node or Drawable isn't transparent (as defined by the isTransparent call),
do nothing and return false. Otherwise, copy the node's UserData
to its StateSet.
\param recursive If true, use the RestoreOpacityVisitor to recursively restore opacity. Default is false.
\return false if \c node is NULL or \c node doesn't have a StateSet. Otherwise, returns true.
*/
template< class T >
bool transparentDisable( T* nodeOrDrawable, bool recursive=false )
{
    if( nodeOrDrawable == NULL )
        return( false );

    osg::Node* node( dynamic_cast< osg::Node* >( nodeOrDrawable ) );
    if( recursive && ( node != NULL ) )
    {
        RestoreOpacityVisitor rov;
        node->accept( rov );
        return( true );
    }

    if( !isTransparent( nodeOrDrawable->getStateSet() ) )
        return( false );

    osg::Referenced* userData = nodeOrDrawable->getUserData();
    osg::StateSet* origStateSet = dynamic_cast< osg::StateSet* >( userData );
    if( origStateSet == NULL )
    {
        // Probably the node had something else attached to UserData, so we
        // were unable to save the StateSet and had to modify the attached StateSet.
        osg::StateSet* stateSet = nodeOrDrawable->getStateSet();
        if( stateSet->getName() == s_magicStateSetName )
        {
            // We created the StateSet, so just delete it.
            nodeOrDrawable->setStateSet( NULL );
        }
        else
        {
            // We didn't create this StateSet, and we weren't able to save it.
            // Best thing we can do is delete the state we added and hope we haven't
            // damaged anything.
            stateSet->removeAttribute( osg::StateAttribute::BLENDCOLOR );
            stateSet->removeAttribute( osg::StateAttribute::BLENDFUNC );
            stateSet->removeMode( GL_BLEND );
            stateSet->setRenderingHint( osg::StateSet::DEFAULT_BIN );
        }
    }
    else
    {
        nodeOrDrawable->setStateSet( origStateSet );
        nodeOrDrawable->setUserData( NULL );
    }

    return( true );
}

/** \brief Return true if the given StateSet is configured like one of our transparent StateSets.
\return True if the node has a StateSet and the StateSet has the following signature:
\li A BlendColor StateAttribute
\li A BlendFunc StateAttribute
\li GL_BLEND is enabled
\li Rendering hint set to TRANSPARENT_BIN
*/
OSGWTOOLS_EXPORT bool isTransparent( const osg::StateSet* stateSet );




/** \brief Find StateSets with nominal transparency, and mark the transparent state as PROTECTED.
Run this node visitor on scene graphs / loaded models that potentially already
contain transparency. The visitor marks the transparent state as PROTECTED so that
subsequently enabling transparency on an ancestor node will not affect the
protected state.
*/
class OSGWTOOLS_EXPORT ProtectTransparencyVisitor : public osg::NodeVisitor
{
public:
    ProtectTransparencyVisitor();

    virtual void apply( osg::Node& node );
    virtual void apply( osg::Geode& geode );

protected:
    /** \bried Mark the transparent componenets of \c stateSet as PROTECTED.
    Does nothing if the \c stateSet is NULL.
    */
    virtual void protectTransparent( osg::StateSet* stateSet ) const;

    /** \brief A general test for transparency.
    Code was lifted from osgconv.cpp FixTransparentVisitor and modified.
    */
    virtual bool isTransparentInternal( const osg::StateSet* stateSet ) const;
};




/** \brief Recursively restore opacity on a subgraph.
This visitor should be considered part of the implementation of transparentDisable(),
which invokes this visitor when the \c recursive parameter is true.
Therefore, apps should call transparentDisable(), rather than invoking this
visitor directly.
*/
class OSGWTOOLS_EXPORT RestoreOpacityVisitor : public osg::NodeVisitor
{
public:
    RestoreOpacityVisitor();

    virtual void apply( osg::Node& node );
    virtual void apply( osg::Geode& geode );
};


/*@}*/


// namespace osgwTools
}


// __OSGWTOOLS_TRANSPARENCY_UTILS_H__
#endif
