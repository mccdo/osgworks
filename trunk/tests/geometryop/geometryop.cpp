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

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osg/Geode>

#include <osg/io_utils>
#include <iostream>
#include <sstream>

#include <osgwTools/GeometryModifier.h>
#include <osgwTools/ReducerOp.h>
#include <osgwTools/DecimatorOp.h>
#include <osgwTools/SimplifierOp.h>
#include <osgwTools/ShortEdgeOp.h>
#include <osgwTools/DecimationTestModel.h>

// TBD #include <osgBullet/VertexAggOp.h>

#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/ComputeBoundsVisitor>
#include <osgUtil/TransformAttributeFunctor>
#include <osg/Version>
#include <osg/io_utils>
#include <osgGA/StateSetManipulator>



int main( int argc,
          char* argv[] )
{
    osg::ArgumentParser arguments( &argc, argv );

    arguments.getApplicationUsage()->setApplicationName( arguments.getApplicationName() );
    arguments.getApplicationUsage()->setDescription( arguments.getApplicationName() + " shows a before and after image of the DecimatorOp module, using a default decimation of 0.6." );
    arguments.getApplicationUsage()->setCommandLineUsage( arguments.getApplicationName() + " [options] filename ..." );

    arguments.getApplicationUsage()->addCommandLineOption( "--reducer", "Use ReducerOp." );
    arguments.getApplicationUsage()->addCommandLineOption( "--shortedge", "Use ShortEdgeOp." );
    arguments.getApplicationUsage()->addCommandLineOption( "--decPercent <n>", "Use DecimatorOp (also valid parameter for ShortEdgeOp). <n> is the target percentage of triangles to remain, in the range 0.0 to 1.0. Default 0.6" );
    arguments.getApplicationUsage()->addCommandLineOption( "--decMaxError <n>", "Specifies the Decimator maximum error tolerance. Geometry exceeding this tolerance is not reduced. <n> is in the range 0.0 to FLT_MAX. Default FLT_MAX" );
    arguments.getApplicationUsage()->addCommandLineOption( "--respectBoundaries", "Will not decimate boundary polygons, will not decimate fully but may fix some mesh errors. Default False" );
    arguments.getApplicationUsage()->addCommandLineOption( "--minPrimatives <n>", "Sets the minimum primitives a geometry must have to start Decimation. Default 1." );
    arguments.getApplicationUsage()->addCommandLineOption( "--maxFeature <n>", "Specifies the ShortEdgeOp largest feature size to be removed, measured in model units. Can be combined with decPercent to limit the decimation using ShortEdgeOp. Default 0.1" );
    arguments.getApplicationUsage()->addCommandLineOption( "--attemptMerge <n>", "Attempts to merge drawables within the model prior to any geometry reduction using a MergeGeometryVisitor. In cases where there are multiple drawables, more functional decimation may result. Default False" );
    arguments.getApplicationUsage()->addCommandLineOption( "--numParts <n>", "Controls the geometry building process if user chooses to use a model built in software (see GeometryModifier.h). numParts controls the geometry and can be used to test different aspects of the decimation routines. Default 3. Range 0-4." );
    arguments.getApplicationUsage()->addCommandLineOption( "--attemptMerge <n>", "Attempt to merge geometry drawables into one using Optimizer::MergeGeometryVisitor before using specified geometry reduction operator." );

    bool useReducer( arguments.find( "--reducer" ) >= 0 );
    bool useShortEdge( arguments.find( "--shortedge" ) >= 0 );

    if( arguments.read( "-h" ) || arguments.read( "--help" ) )
    {
        arguments.getApplicationUsage()->write( osg::notify( osg::ALWAYS ), osg::ApplicationUsage::COMMAND_LINE_OPTION );
        return 1;
    }

    if (arguments.errors())
    {
        arguments.writeErrorMessages( osg::notify( osg::FATAL ) );
        return 1;
    }


    float decimatorPercent( .6 );
    std::string str;
    if ( arguments.read( "--decPercent", str ) )
    {
        if( sscanf( str.c_str(), "%f", &decimatorPercent ) != 1 )
        {
            arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ) );
            return 1;
        }

    }
    float decimatorMaxError( FLT_MAX );
    if ( arguments.read( "--decMaxError", str ) )
    {
        if( sscanf( str.c_str(), "%f", &decimatorMaxError ) != 1 )
        {
            arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ) );
            return 1;
        }
    }
    bool decimatorIgnoreBoundaries = (true);
    if (arguments.read( "--respectBoundaries" ))
        decimatorIgnoreBoundaries = false;

    int minprim(1);
    if (arguments.read("--minPrimatives", str))
    {
        if( sscanf( str.c_str(), "%u", &minprim ) != 1 )
        {
            arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ) );
            return 1;
        }
    }
    if (decimatorPercent < 1.f )
        osg::notify( osg::INFO ) << "DecimatorOp: " << decimatorPercent << ", " << decimatorMaxError << std::endl;

    float shortEdgeFeature( .1 );
    if ( arguments.read( "--maxFeature", str ) )
    {
        if( sscanf( str.c_str(), "%f", &shortEdgeFeature ) != 1 )
        {
            arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ) );
            return 1;
        }
    }
    bool attemptMerge = (false);
    if (arguments.read( "--attemptMerge" ))
        attemptMerge = true;

    bool useAgg = false;
    unsigned int aggMaxVerticies( 0 );
    osg::Vec3 aggMinCellSize( 0., 0., 0. );
    if(arguments.read( "--aggMaxVerts", str) )
    {
        if( sscanf( str.c_str(), "%u", &aggMaxVerticies) != 1 )
        {
            arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ) );
        }
        useAgg = true;
        if ( arguments.read( "--aggMinCellSize", str ) )
        {
            char comma;
            std::istringstream oStr( str );
            oStr >> aggMinCellSize[ 0 ] >> comma >>
                aggMinCellSize[ 1 ] >> comma >>
                aggMinCellSize[ 2 ];
        }
    }
    
    std::string modelname, namebase;
    namebase = "C:\\OSGDev\\Stable\\data\\";
    bool savefile = false;
    // builds a model for testing if no model file supplied
    osg::Node*  model = osgDB::readNodeFiles( arguments );
    if( !model )
        {
            // built model can consist of one part or two or three. If one part, the front, back and sides can share vertices or have duplicate sets of vertices for the edge triangles. 
            // Merge results may differ depending which is modeled and how the decimation algorithm is implemented and whether or not a geometry merge is attempted prior to decimation.
            int numParts( 3 );
            if ( arguments.read( "--numParts", str ) )
            {
                if( sscanf( str.c_str(), "%u", &numParts ) != 1 )
                {
                    arguments.getApplicationUsage()->write( osg::notify( osg::FATAL ) );
                    return 1;
                }
            }
            osgwTools::DecimationTestModel* builtModel = new osgwTools::DecimationTestModel(numParts);
            if (builtModel)
            {
                model = builtModel->getModel();
                if (savefile)
                {
                    char partNum[64];
                    sprintf(partNum, "%1d_", numParts);
                    namebase.append("DecimationTestModel_");
                    namebase.append(partNum);
                }
            }
            if (!model)
            {
                osg::notify( osg::FATAL ) << "Can't load input file(s)." << std::endl;
                return 1;
            }
           
        }
    else
    {
        namebase.append("LoadedTestModel_");
    }

    osg::ref_ptr<osg::Group> grporig = new osg::Group;
    grporig->addChild(model);
    osg::notify( osg::INFO ) << "geometryop: Loaded model(s)." << std::endl;


    osg::ref_ptr<osg::Group> grpcopy = new osg::Group( *grporig , osg::CopyOp::DEEP_COPY_ALL);
    grpcopy->setName("grpcopy"); // for ease of debugging

    osgwTools::GeometryOperation* reducer;
    if(!useAgg && !useReducer && !useShortEdge)
    {
        osgwTools::DecimatorOp* decimate = new osgwTools::DecimatorOp;
        decimate->setSampleRatio(decimatorPercent);
        decimate->setMaximumError(decimatorMaxError);
        decimate->setIgnoreBoundaries(decimatorIgnoreBoundaries);
        decimate->setMinPrimatives(minprim);
        reducer = decimate;
    }
    else if( useReducer )
    {
        osgwTools::ReducerOp* redOp = new osgwTools::ReducerOp;
        reducer = redOp;
    }
    else if( useShortEdge )
    {
        osgwTools::ShortEdgeOp* seOp = new osgwTools::ShortEdgeOp(decimatorPercent, decimatorMaxError, shortEdgeFeature);
        seOp->setIgnoreBoundaries(decimatorIgnoreBoundaries);
        seOp->setMinPrimitives(minprim);
        reducer = seOp;
    }
    else
    {
        /* TBD
        osgBullet::VertexAggOp* vertagg = new osgBullet::VertexAggOp;
        vertagg->setMaxVertsPerCell( aggMaxVerticies );
        vertagg->setMinCellSize( aggMinCellSize );reducer = vertagg;
        reducer = vertagg;
        */
    }
    if( reducer != NULL )
    {
        osgwTools::GeometryModifier modifier(reducer);
        modifier.setDrawableMerge(attemptMerge);
        grpcopy->accept( modifier);
        modifier.displayStatistics( osg::notify( osg::ALWAYS ) );
    }

    if (savefile)
    {
        modelname.assign(namebase);
        modelname.append("orig.osg");
        osgDB::writeNodeFile(*grporig, modelname.c_str());
        modelname.assign(namebase);
        modelname.append("reduced.osg");
        osgDB::writeNodeFile(*grpcopy, modelname.c_str());
    }

    //
    // Viewer setup.
    osgViewer::CompositeViewer viewer(arguments);

    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi)
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return 1;
    }
    unsigned int width( 800 ), height( 600 );
    const float aspect( (float)width/(float)(height * 2.f) );
    const osg::Matrix proj( osg::Matrix::perspective( 50., aspect, 1., 10. ) );

    // Shared event handlers and state sets.
    osg::ref_ptr< osgGA::TrackballManipulator > tb = new osgGA::TrackballManipulator;

    osg::ref_ptr< osg::StateSet > camSS;
    osg::ref_ptr<osgGA::StateSetManipulator> statesetManipulator = new osgGA::StateSetManipulator;

    osg::ref_ptr<osg::GraphicsContext> gc;

    // view one
    {
        osgViewer::View* view = new osgViewer::View;
        view->setUpViewInWindow( 10, 30, width, height );
        viewer.addView(view);
        view->setSceneData( grporig.get() );

        camSS = view->getCamera()->getOrCreateStateSet();
        statesetManipulator->setStateSet( camSS.get() );
        view->addEventHandler( statesetManipulator.get() );

        view->setCameraManipulator( tb.get() );

        viewer.realize();
        gc = view->getCamera()->getGraphicsContext();
        view->getCamera()->setViewport(new osg::Viewport(0,0, width/2, height));
        view->getCamera()->setProjectionMatrix( proj );
    }

    // view two
    {
        osgViewer::View* view = new osgViewer::View;
        viewer.addView(view);
        view->setSceneData( grpcopy.get() );

        view->getCamera()->setStateSet( camSS.get() );
        view->addEventHandler( statesetManipulator.get() );

        view->setCameraManipulator( tb.get() );

        view->getCamera()->setViewport(new osg::Viewport(width/2,0, width/2, height));
        view->getCamera()->setGraphicsContext(gc.get());
        view->getCamera()->setProjectionMatrix( proj );
    }

    return( viewer.run() );
}
