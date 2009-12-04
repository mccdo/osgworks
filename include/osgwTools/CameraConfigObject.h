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

#ifndef __OSGWORKS_CAMERA_CONFIG_OBJECT_H__
#define __OSGWORKS_CAMERA_CONFIG_OBJECT_H__ 1


#include <osgwTools/Export.h>
#include <osg/Object>
#include <osgViewer/Viewer>
#include <osg/Matrix>

#include <vector>


namespace osgwTools {


struct OSGWTOOLS_EXPORT CameraConfigInfo
{
    CameraConfigInfo();
    ~CameraConfigInfo();

    unsigned int _version;

    osg::Matrix _viewOffset;
    osg::Matrix _projectionOffset;
};
typedef std::vector< CameraConfigInfo > CameraConfigList;

class OSGWTOOLS_EXPORT CameraConfigObject : public osg::Object
{
public:
    CameraConfigObject();
    CameraConfigObject( const osgwTools::CameraConfigObject& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );

    META_Object( osgwTools, CameraConfigObject );

    void take( const osgViewer::Viewer& viewer );
    void store( osgViewer::Viewer& viewer );

    CameraConfigList _slaveConfigInfo;

protected:
    ~CameraConfigObject();
};

// namespace osgwTools
}

// __OSGWORKS_CAMERA_CONFIG_OBJECT_H__
#endif
