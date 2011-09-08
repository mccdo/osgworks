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

#ifndef __OSGWTOOLS_FBO_UTILS_H__
#define __OSGWTOOLS_FBO_UTILS_H__ 1


#include "osgwTools/Export.h"
#include <osg/FrameBufferObject>



namespace osgwTools
{


/** \defgroup FBOUtils OSG version-independent interface for OpenGL framebuffer object commands

OSG made a non-backwards compatible change to its FrameBufferObject header in
OSG version 2.9.6. Any application code referencing the changed routines should
use these version-independent entry points to remain compatible with all
OSG versions.
*/
/*@{*/


#ifndef GL_FRAMEBUFFER
#  define GL_FRAMEBUFFER 0x8D40
#endif


GLvoid OSGWTOOLS_EXPORT glGenFramebuffers( osg::FBOExtensions* fboExt, GLsizei n, GLuint* framebuffer );
GLvoid OSGWTOOLS_EXPORT glDeleteFramebuffers( osg::FBOExtensions* fboExt, GLsizei n, GLuint* framebuffer );
GLvoid OSGWTOOLS_EXPORT glBindFramebuffer( osg::FBOExtensions* fboExt, GLenum target, GLuint framebuffer );

GLvoid OSGWTOOLS_EXPORT glFramebufferTexture2D( osg::FBOExtensions* fboExt, GLenum target, GLenum attachment,
            GLenum textarget, GLuint texture, GLint level );
GLvoid OSGWTOOLS_EXPORT glBlitFramebuffer(  osg::FBOExtensions* fboExt, GLint srcX0, GLint srcY0, GLint srcX1,
            GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter );



/*@}*/


// namespace osgwTools
}


// __OSGWTOOLS_FBO_UTILS_H__
#endif
