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

#include "osgwTools/LODCreationNodeVisitor.h"
#include "osgwTools/ShortEdgeOp.h"
#include <osgwTools/GeometryModifier.h>
#include <osg/NodeVisitor>
#include <osg/LOD>
#include <osg/BoundingSphere>


namespace osgwTools
{


LODCreationNodeVisitor::LODCreationNodeVisitor(GeodeReducableCallback *reducableCallback )
  : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
    _geodesLocated(0),
    _geodesProcessed(0),
    _minTestVertices(100), 
    _minTestPrimitives(100),
    _maxDecPercent(static_cast<float>(.01)),
    _decIgnoreBoundaries(1),
    _decMinPrimitives(1),
    _geodeReducableCallback(reducableCallback)
{
    // default values for geometry reduction
    double lodPixelSize[] = {100.0, 25, 0.0};
    double lodFeatureSize[] = {0.0, 0.1, 0.3};

    if( _geodeReducableCallback==NULL )
        _geodeReducableCallback = new BasicGeodeReducableCallback;

    LODPairList lodPairList;
    unsigned int numPairs = sizeof(lodPixelSize) / sizeof(double);
    if (numPairs > sizeof(lodFeatureSize) / sizeof(double))
        numPairs = sizeof(lodFeatureSize) / sizeof(double);

    for ( unsigned int i = 0; i < numPairs; ++i)
    {
        LODPair pair(lodPixelSize[i], lodFeatureSize[i]);
        lodPairList.push_back(pair);
    }
    setLODPairs(lodPairList);
}

unsigned int LODCreationNodeVisitor::finishProcessingGeodes(void)
{
    // process all the stored references to Geodes
	for(GeodeSet::iterator itr=_lodCandidates.begin();
        itr!=_lodCandidates.end();
        ++itr)
    {
		osg::notify( osg::DEBUG_INFO ) << "LODCreationNodeVisitor::finishProcessingGeodes Geode number: " << _geodesProcessed << std::endl;

        // get list of parents of this Geode
        osg::ref_ptr<osg::Geode> currentGeode = *itr;
		osg::Node::ParentList unchangedParents = currentGeode->getParents();
        osg::BoundingSphere currentBounds = currentGeode->getBound();
        float currentDiameter = 2.0 * currentBounds.radius();

        // for each element in _lodPairList create a separate LOD node and add it to parent
        float prevMaxPixels = FLT_MAX;
        unsigned int lodNum = 0;
        osg::ref_ptr<osg::LOD> lodNode = new osg::LOD;
        lodNode.get()->setRangeMode(osg::LOD::PIXEL_SIZE_ON_SCREEN);
        lodNode.get()->setCenterMode(osg::LOD::USE_BOUNDING_SPHERE_CENTER);

        for(LODPairList::iterator pitr = _lodPairList.begin();
            pitr != _lodPairList.end();
            ++pitr, ++lodNum)
        {
            lodNode.get()->setRange(lodNum, pitr->first, prevMaxPixels);
            // do geometry reduction for this level and attach to LOD
            float shortEdgeFeature = pitr->second * currentDiameter;
            if (shortEdgeFeature > 0.0)
            {
                osg::Geode* geodeCopy = new osg::Geode( *currentGeode.get() , osg::CopyOp::DEEP_COPY_ALL);
                osgwTools::GeometryOperation* reducer;
                osgwTools::ShortEdgeOp* seOp = new osgwTools::ShortEdgeOp;
                //seOp->setSampleRatio(_maxDecPercent);
                float decPct = (1.0 - pitr->second) / ((1 + lodNum) * (1 + lodNum));
                if (decPct < _maxDecPercent)
                    decPct = _maxDecPercent;
                seOp->setSampleRatio(decPct);
                seOp->setMaxFeature(shortEdgeFeature);
                seOp->setMaximumError(shortEdgeFeature);
                seOp->setIgnoreBoundaries(_decIgnoreBoundaries);
                seOp->setMinPrimitives(_decMinPrimitives);
                reducer = seOp;
                osgwTools::GeometryModifier modifier(reducer);
                modifier.setDrawableMerge(true);
                geodeCopy->accept( modifier);
                lodNode.get()->addChild(geodeCopy);
            }
            else
                lodNode.get()->addChild(currentGeode.get());

            prevMaxPixels = pitr->first;
        }
        // for each parent, remove this Geode and replace with LOD nodes
		for(osg::Node::ParentList::iterator parentIter = unchangedParents.begin();
			parentIter != unchangedParents.end(); parentIter++)
		{
            osg::ref_ptr<osg::Group> parentAsGroup = (*parentIter)->asGroup();
            if (parentAsGroup.valid())
            {
                parentAsGroup.get()->replaceChild(currentGeode.get(), lodNode.get());
            }
        }

		++_geodesProcessed;
    } // for

	return(_geodesProcessed);
} // LODCreationNodeVisitor::finishProcessingGeodes


void LODCreationNodeVisitor::processNode( osg::Node& node )
{
    osg::ref_ptr<osg::Geode> nodeAsGeode = node.asGeode();
    if (nodeAsGeode.valid())
    {
        bool passedTest = _geodeReducableCallback->testGeodeReducable(nodeAsGeode.get(), _minTestVertices, _minTestPrimitives);
        if (passedTest)
        {
            _lodCandidates.insert( nodeAsGeode.get() );
        }
        // count the geodes tested, not the ones passed which you can get from _lodCandidates.size()
        ++_geodesLocated;
    }
    else
        traverse(node);

} // LODCreationNodeVisitor::processNode

// Only here to do depth-tracking processing on all node types
void LODCreationNodeVisitor::apply( osg::Node& node )
{
    processNode( node );
}

// osgwTools
}
