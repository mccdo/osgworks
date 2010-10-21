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

#ifndef __OSGWORKS_CALLBACK_SUPPORT_H__
#define __OSGWORKS_CALLBACK_SUPPORT_H__


#include <osgwTools/Export.h>
#include <osg/Camera>
#include <osg/ref_ptr>

#include <vector>


namespace osgwTools {


// In OSG 2.8.3 (at least), Cameras can only have one DrawCallback attached
// (as a post draw callback, for example). Nested callbacks, as in the NodeCallback
// class, are not supported. Apps can attach a CompositeDrawCallback to a Camera,
// then attach a list of DrawCallbacks. CompositeDrawCallback's operator() will then
// execute the operator() of each DrawCallback in the list.
struct OSGWTOOLS_EXPORT CompositeDrawCallback : public osg::Camera::DrawCallback
{
public:
    CompositeDrawCallback();
    CompositeDrawCallback( const CompositeDrawCallback& rhs,const osg::CopyOp& copyOp );

    META_Object(osgwTools,CompositeDrawCallback);

    virtual void operator()( osg::RenderInfo& renderInfo ) const;

    typedef std::vector< osg::ref_ptr< osg::Camera::DrawCallback > > DrawCallbackList;
    DrawCallbackList& getDrawCallbackList();
    const DrawCallbackList& getDrawCallbackList() const;

protected:
    virtual ~CompositeDrawCallback();

    DrawCallbackList _dcl;
};

// namespace osgwTools
}

// __OSGWORKS_CALLBACK_SUPPORT_H__
#endif
