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

#ifndef __OSGWORKS_OSGWTOOLS_CAPABILITIES_H__
#define __OSGWORKS_OSGWTOOLS_CAPABILITIES_H__ 1


#include <osgwTools/Export.h>

#include <osg/GraphicsContext>

#include <iostream>
#include <string>


namespace osgwTools
{


/** \class Capabilities Capabilities.h <osgwTools/Capabilities.h>
\brief
\details Requires a current context.
*/
struct OSGWTOOLS_EXPORT Capabilities
{
    Capabilities( osg::GraphicsContext* gc );
    ~Capabilities();

    void dump( std::ostream& ostr );


    std::string _osgWorksVersion;
    std::string _osgVersion;

    std::string _glVersion;
    std::string _glVendor;
    std::string _glRenderer;
    float _glslVersion;

    GLint _texSize;
    GLint _3DTexSize;
    GLint _cubeMapTexSize;
    GLint _maxTexUnits;

    GLint _shaderUnits;
    GLint _texCoords;
    GLint _vertexAttribs;
    GLint _drawBuffers;
};


// namespace osgwTools
}


// __OSGWORKS_OSGWTOOLS_CAPABILITIES_H__
#endif
