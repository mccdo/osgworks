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

#include <osgwTools/FBOUtils.h>
#include <osg/FrameBufferObject>


namespace osgwTools
{


#define OSG_FBO_CHANGE_VERSION 20906


GLvoid glGenFramebuffers( osg::FBOExtensions* fboExt, GLsizei n, GLuint* framebuffer )
{
#if( OSGWORKS_OSG_VERSION >= OSG_FBO_CHANGE_VERSION )
    fboExt->glGenFramebuffers( n, framebuffer );
#else
    fboExt->glGenFramebuffersEXT( n, framebuffer );
#endif
}

GLvoid glDeleteFramebuffers( osg::FBOExtensions* fboExt, GLsizei n, GLuint* framebuffer )
{
#if( OSGWORKS_OSG_VERSION >= OSG_FBO_CHANGE_VERSION )
    fboExt->glDeleteFramebuffers( n, framebuffer );
#else
    fboExt->glDeleteFramebuffersEXT( n, framebuffer );
#endif
}

GLvoid glBindFramebuffer( osg::FBOExtensions* fboExt, GLenum target, GLuint framebuffer )
{
#if( OSGWORKS_OSG_VERSION >= OSG_FBO_CHANGE_VERSION )
    fboExt->glBindFramebuffer( target, framebuffer );
#else
    fboExt->glBindFramebufferEXT( target, framebuffer );
#endif
}


GLvoid glFramebufferTexture2D( osg::FBOExtensions* fboExt, GLenum target, GLenum attachment,
            GLenum textarget, GLuint texture, GLint level )
{
#if( OSGWORKS_OSG_VERSION >= OSG_FBO_CHANGE_VERSION )
    fboExt->glFramebufferTexture2D( target, attachment, textarget, texture, level );
#else
    fboExt->glFramebufferTexture2DEXT( target, attachment, textarget, texture, level );
#endif
}



// osgwTools
}
