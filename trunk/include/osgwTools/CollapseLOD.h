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

#ifndef __OSGWTOOLS_COLLAPSE_LOD__
#define __OSGWTOOLS_COLLAPSE_LOD__ 1


#include "osgwTools/Export.h"
#include <osg/NodeVisitor>
#include <osg/Group> // for NodeList type
#include <osg/LOD> // for NodeList type
#include <osg/Notify>

#include <string>
#include <limits>

namespace osg {
    class Geometry;
}


namespace osgwTools
{

// Should this perhaps be exposed somewhere else for wider use?
	typedef std::set< osg::ref_ptr<osg::Node> > NodeSet;

// Callback to decide which LOD child should be retained
class NodeSelectorCallback : public osg::Referenced
{
    public:
        /** returns the node which should be selected for retention or NULL if no nodes should be retained.
		Override to implement the desired behavior. */
		virtual osg::Node *selectChild(osg::Group *parentOfCandidates) const
        {
            return NULL;
        } // selectChild
    
        virtual ~NodeSelectorCallback() {}
}; // NodeSelectorCallback



// Callback to select highest LOD child 
class HighestLODChildSelectorCallback : public osgwTools::NodeSelectorCallback
{
    public:
        /** returns the child with the highest LOD */
		osg::Node *selectChild(osg::Group *parentOfCandidates) const
        {
			osg::Node *selectedChild = NULL;
			if(parentOfCandidates)
			{
				if(osg::LOD *parentAsLOD = dynamic_cast<osg::LOD*>(parentOfCandidates))
				{
					// choose highest LOD
					float closestLODnear;
					if(parentAsLOD->getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT)
					{
						closestLODnear = std::numeric_limits<float>::max();
					} // if
					else
					{
						closestLODnear = std::numeric_limits<float>::min();
					} // else
					for(size_t childLoop = 0; childLoop < parentAsLOD->getNumChildren(); childLoop++)
					{
						osg::notify( osg::DEBUG_INFO ) << "  HighestLODChildSelectorCallback child name: " << parentAsLOD->getChild(childLoop)->getName() << std::endl;
						osg::notify( osg::DEBUG_INFO ) << "  HighestLODChildSelectorCallback child LODmin: " << parentAsLOD->getMinRange(childLoop) << std::endl;
						osg::notify( osg::DEBUG_INFO ) << "  HighestLODChildSelectorCallback child LODmax: " << parentAsLOD->getMaxRange(childLoop) << std::endl;

						// does this child satisfy whichever "nearest" criteria in effect on this LOD?
						bool thisIsClosest = false;
						if(parentAsLOD->getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT)
						{
							// EYE_POINT means lower numbers are nearer
							if(parentAsLOD->getMinRange(childLoop) < closestLODnear) // here we're comparing MAX range
							{
								thisIsClosest = true;
							} // if
						} // if
						else
						{
							// PIXEL_SIZE_ON_SCREEN means higher numbers are nearer
							if(parentAsLOD->getMaxRange(childLoop) > closestLODnear) // now we're comparing MAX range
							{
								thisIsClosest = true;
							} // if
						} // else
						if(thisIsClosest)
						{
							osg::notify( osg::DEBUG_INFO ) << "   HighestLODChildSelectorCallback closest: " << parentAsLOD->getChild(childLoop)->getName() << std::endl;
							// grab a reference to it so it survives the wholesale obliteration below
							selectedChild = parentAsLOD->getChild(childLoop);
							closestLODnear = parentAsLOD->getMinRange(childLoop);
						} // if
					} // for
				} // if
			} // if
            return selectedChild;
        } // selectChild
}; // HighestLODChildSelectorCallback


// CollapseLOD -- Finds LOD nodes and collapses them to a group that has only one child, the highest LOD.
class OSGWTOOLS_EXPORT CollapseLOD : public osg::NodeVisitor
{
public:
    enum CollapseMode {
        COLLAPSE_TO_GROUP,
        COLLAPSE_TO_PARENT,
        NUM_COLLAPSE_MODES
    };

	CollapseLOD( NodeSelectorCallback *SelectorCallback, const CollapseMode collapseMode, const osg::NodeVisitor::TraversalMode travMode=osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
	virtual ~CollapseLOD() {};

	void apply( osg::LOD& node );
	void apply( osg::Node& node );

	unsigned int getLODsLocated(void) const {return(_LODsLocated);}
	unsigned int getLODsProcessed(void) const {return(_LODsProcessed);}
	unsigned int finishProcessingLODs(void);

protected:
	void processNode( osg::Node& node );

	NodeSet _collectedLODNodes;
	unsigned int _LODsLocated, _LODsProcessed, _depth;
	CollapseMode _collapseMode;
	osg::ref_ptr<NodeSelectorCallback> _selectorCallback;

};


// osgwTools
}

// __OSGWTOOLS_COLLAPSE_LOD__
#endif
