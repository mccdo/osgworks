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

#include <osgwQuery/QueryBenchmarks.h>

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>



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


int main( int argc, char** argv )
{
    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow( 0., 0., 1024., 768. );
    viewer.setThreadingModel( osgViewer::ViewerBase::SingleThreaded );

    addInit( viewer );

    viewer.realize();
    viewer.frame();
    // In some thread models, 2nd frame required before pre-draw callback executes.
    viewer.frame();

    removeInit( viewer );
}
