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

#include <osgwQuery/NodeData.h>
#include <osgwQuery/QueryBenchmarks.h>
#include <osgwQuery/QueryStats.h>
#include <osgwTools/Shapes.h>
#include <osgwTools/FindNamedNode.h>
#include <osgUtil/RenderBin>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>


struct InitCallback : public osg::Camera::DrawCallback
{
public:
    InitCallback() {}
    virtual void operator()( osg::RenderInfo& renderInfo ) const
    {
        unsigned int contextID = renderInfo.getState()->getContextID();
        osgwQuery::QueryBenchmarks* qb = osgwQuery::getQueryBenchmarks( contextID, &renderInfo );
    }
};
void addInit( osgViewer::Viewer& viewer )
{
    viewer.getCamera()->setPreDrawCallback( new InitCallback() );
}
void removeInit( osgViewer::Viewer& viewer )
{
    viewer.getCamera()->setPreDrawCallback( NULL );
}


osg::Node* makeSceneA()
{
    osg::Group* grp = new osg::Group;
    osg::Geode* geode = new osg::Geode;
    grp->addChild( geode );
    osg::Geometry* geom = osgwTools::makeGeodesicSphere( 3., 5 );
    geode->addDrawable( geom );

    return( grp );
}
osg::Node* makeSceneB()
{
    osg::Group* grp = new osg::Group;
    osg::Geode* geode = new osg::Geode;
    grp->addChild( geode );
    osg::Geometry* geom = osgwTools::makePlane( osg::Vec3( -10., -50., -10. ),
        osg::Vec3( 20., 0., 0. ), osg::Vec3( 0., 0., 20. ) );
    geode->addDrawable( geom );

    return( grp );
}

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    osgUtil::RenderBin::setDefaultRenderBinSortMode(
        osgUtil::RenderBin::SORT_FRONT_TO_BACK );

    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow( 0., 0., 1024., 768. );
    viewer.setCameraManipulator( new osgGA::TrackballManipulator );
    viewer.addEventHandler( new osgViewer::StatsHandler );
    viewer.addEventHandler( new osgGA::StateSetManipulator(
        viewer.getCamera()->getOrCreateStateSet() ) );


    osg::Group* root = new osg::Group;
    osg::Node* models = osgDB::readNodeFiles( arguments );
    if( models != NULL )
        root->addChild( models );
    else
    {
        root->addChild( makeSceneA() );
        root->addChild( makeSceneB() );
    }

    // Add the Query statistics HUD.
    osg::ref_ptr< osgwQuery::QueryStats > qs;
    {
        osgwTools::FindNamedNode fnn( "Dumptruck" );
        root->accept( fnn );
        if( fnn._napl.empty() )
            return( 1 );
        osg::notify( osg::ALWAYS ) << "Stats for node " << fnn._napl[ 0 ].first->getName() << std::endl;
        qs = new osgwQuery::QueryStats( fnn._napl[ 0 ].first );
        root->addChild( qs->getSceneGraph() );

        viewer.addEventHandler( new osgwQuery::QueryStatsHandler( qs.get() ) );
    }

    viewer.setSceneData( root );


    viewer.setThreadingModel( osgViewer::ViewerBase::SingleThreaded );

    // First frame creates GL objects. Must do this before we add
    // any Drawable draw callbacks or Camera pre-draw callbacks.
    viewer.frame();

    osgwQuery::AddQueries aqs;
    aqs.setQueryStats( qs.get() );
    root->accept( aqs );
    addInit( viewer );

    while( !viewer.done() )
    {
        osg::notify( osg::INFO ) << "        *** Frame ***" << std::endl;

        osgwQuery::AddQueries::setCscrOi( 0. );
        viewer.frame();

        removeInit( viewer );

        qs->incFrames();
    }
}
