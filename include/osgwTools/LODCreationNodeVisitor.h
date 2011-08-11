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

#ifndef __LOD_CREATION_NODE_VISITOR_H__
#define __LOD_CREATION_NODE_VISITOR_H__

#include <osgwTools/Export.h>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>

namespace osgwTools {

typedef std::set<osg::Geode*> GeodeSet;
typedef std::pair<double, double> LODPair;
typedef std::vector< LODPair > LODPairList;
typedef std::vector< osg::ref_ptr<osg::LOD> > LODList;

/** GeodeReducableCallback
\brief Callback to decide which Geodes to store */
class OSGWTOOLS_EXPORT GeodeReducableCallback : public osg::Referenced
{
    public:
        /** Returns true if Geode should be added to set for later treatment
		Override to implement the desired behavior. */
		virtual bool testGeodeReducable(osg::Geode *geode, unsigned int minVertices, unsigned int minPrimitives) const
        {
            return NULL;
        } // testGeodeReducable
    
        virtual ~GeodeReducableCallback() {}
}; // GeodeReducableCallback



/** \brief Callback to select highest LOD child */
class BasicGeodeReducableCallback : public osgwTools::GeodeReducableCallback
{
public:
    /** returns true if Geode should be added to set for later treatment */
	bool testGeodeReducable(osg::Geode *geode, unsigned int minVertices, unsigned int minPrimitives) const
    {
        unsigned int drawableCount = 0;
        unsigned int geometryCount = 0;
        unsigned int numVertices = 0;
        unsigned int numPrimitives = 0;

        for(unsigned int i = 0; i < geode->getNumDrawables(); ++i)
        {
            ++drawableCount;
            osg::ref_ptr< osg::Geometry > geometry = geode->getDrawable(i)->asGeometry();
            if( geometry.valid() )
            {
                ++geometryCount;
                if( geometry->containsSharedArrays() )
                    osg::notify( osg::ALWAYS ) << "Warning! Geometry contains shared arrays" << std::endl;

                // Get statistics
                numVertices += geometry.get()->getVertexArray()->getNumElements();

                unsigned int idx;
                for( idx = 0; idx < geometry.get()->getNumPrimitiveSets(); ++idx )
                {
                    const osg::PrimitiveSet* ps( geometry.get()->getPrimitiveSet( idx ) );
                    numPrimitives += ps->getNumPrimitives();
                }

            }
        }

        return (numVertices > minVertices || numPrimitives > minPrimitives);
    } // testGeodeReducable
}; // BasicGeodeReducableCallback


/** \brief A node visitor for creating LOD nodes to replace Geodes with simplified geometry at user-supplied levels of detail and pixel size constraints.

*/
class OSGWTOOLS_EXPORT LODCreationNodeVisitor : public osg::NodeVisitor
{
public:

    LODCreationNodeVisitor(GeodeReducableCallback* cb);
    virtual ~LODCreationNodeVisitor() {}

    void setLODPairs(LODPairList& lodPairList)
    {
        _lodPairList.clear();
        // copy the pairs
        std::copy( lodPairList.begin(), lodPairList.end(), std::inserter(_lodPairList, _lodPairList.begin()));
    }
    void setTestMinVertices(unsigned int minVertices)   { _minTestVertices = minVertices; }
    void setTestMinPrimitives(unsigned int minPrimitives)   { _minTestPrimitives = minPrimitives; _decMinPrimitives = minPrimitives;}
    void setMaxDecPercent(unsigned int maxDec)   { _maxDecPercent = maxDec; }
    void setIgnoreBoundaries(bool ignore)   { _decIgnoreBoundaries = ignore; }

    GeodeSet& getLODCandidates(void)    { return (_lodCandidates); }
    LODPairList& getLODPairs(void)    { return (_lodPairList); }

	void apply( osg::Node& node );

    /** Calling code must call this function to create LODs.
    This NodeVisitor collects Geodes, but doesn't actually process
    them until the calling code executes this member function. */
	unsigned int finishProcessingGeodes(void);

protected:
	void processNode( osg::Node& node );

    GeodeSet _lodCandidates;
    /** _lodPairList Calling code can supply a pair list of rely on defaults
    Order of items in pairs is 1) smallest pixel size for LOD to be used, 
    2) maximum feature size to be removed as % of node bounding sphere diameter */
    LODPairList _lodPairList;
    /// if set, _decIgnoreBoundaries prevents elimination of edges that are bounded by only one triangle
    bool _decIgnoreBoundaries;
    /// _geodesLocated, _geodesProcessed for internal control
	unsigned int _geodesLocated, _geodesProcessed;
    /// _minTestVertices, _minTestPrimitives can be supplied to control which Geodes will be selected for decimation using the default callback function
    unsigned int _minTestVertices, _minTestPrimitives;
    /// _decMinPrimitives sets a minimum number of triangles that will be required to be present in the final output Geodes
    unsigned int _decMinPrimitives;
    /// maximum amount of triangle reduction that will be allowed
    float _maxDecPercent;

    /// Calling code can supply a callback function to decide which Geodes should be decimated or rely on default method based on _minTestVertices and _minTestPrimitives
    osg::ref_ptr<GeodeReducableCallback> _geodeReducableCallback;

};

}

#endif