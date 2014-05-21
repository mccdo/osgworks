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

#ifndef __OSGWTOOLS_VERSION_OSG_H__
#define __OSGWTOOLS_VERSION_OSG_H__ 1



/** \addtogroup Version */
/*@{*/

/** \brief OSG version number as an integer

This macro provides the same functionality as 
OSGWORKS_VERSION, but instead encodes the OSG version
number as an integer.
OSG didn't provide a useful compile-time version
comparison macro until after the 2.9.6 and 2.8.2 releases.
\see OSGWORKS_VERSION */
#define OSGWORKS_OSG_VERSION ( \
        ( OPENSCENEGRAPH_MAJOR_VERSION * 10000 ) + \
        ( OPENSCENEGRAPH_MINOR_VERSION * 100 ) + \
        OPENSCENEGRAPH_PATCH_VERSION )

/*@}*/


// __OSGWTOOLS_VERSION_OSG_H__
#endif
