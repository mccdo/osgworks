/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2012 by Kenneth Mark Bryden
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

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Version>

#include <osgwTools/Capabilities.h>



struct SimpleContext
{
    SimpleContext()
    {
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->width = 1;
        traits->height = 1;
        traits->pbuffer = true;

        _gc = osg::GraphicsContext::createGraphicsContext(traits.get());

        if (!_gc)
        {
            osg::notify(osg::INFO)<<"Failed to create pbuffer, failing back to normal graphics window."<<std::endl;
                
            traits->pbuffer = false;
            _gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        }

        if (_gc.valid()) 
        {
            _gc->realize();
            _gc->makeCurrent();
            if (dynamic_cast<osgViewer::GraphicsWindow*>(_gc.get()))
            {
                osg::notify(osg::INFO)<<"Realized graphics window for OpenGL operations."<<std::endl;
            }
            else
            {
                osg::notify(osg::INFO)<<"Realized pbuffer for OpenGL operations."<<std::endl;
            }
        }
        else
        {
            osg::notify(osg::WARN)<<"Failed to create GC."<<std::endl;
        }
    }
        
    osg::ref_ptr<osg::GraphicsContext> _gc;
};


int main( int argc, char** argv )
{
    // Bizarre... Must call this function, or context creation will fail.
    ::osgViewerGetVersion();


    SimpleContext context;

    osgwTools::Capabilities caps;
    caps.dump( osg::notify( osg::ALWAYS ) );


    return( 0 );
}



/** \page osgwcomp The osgwcomp Application
osgwcomp compares the structure of two scene graphs using the \ref ParallelVisitor.

Further documentation for osgwcomp is TBD.

*/
