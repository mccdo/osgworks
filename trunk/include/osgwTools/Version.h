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

#ifndef __OSGWTOOLS_VERSION_H__
#define __OSGWTOOLS_VERSION_H__ 1

#include "osgwTools/Export.h"
#include <osg/Version>
#include <string>


namespace osgwTools {


#define OSGWORKS_MAJOR_VERSION 1
#define OSGWORKS_MINOR_VERSION 1
#define OSGWORKS_SUB_VERSION 50

// C preprocessor integrated version number.
// The form is Mmmss, where:
//   M is the major version
//   mm is the minor version (zero-padded)
//   ss is the sub version (zero padded)
// Use this in version-specific code, for example:
//   #if( OSGWORKS_VERSION < 10500 )
//      ... code specific to releases before v1.05
//   #endif
#define OSGWORKS_VERSION ( \
        ( OSGWORKS_MAJOR_VERSION * 10000 ) + \
        ( OSGWORKS_MINOR_VERSION * 100 ) + \
        OSGWORKS_SUB_VERSION )

// This macro does the same thing for the OSG version.
// OSG did not provide a useful compile-time version
// comparison macro until after the 2.9.6 release.
#define OSGWORKS_OSG_VERSION ( \
        ( OPENSCENEGRAPH_MAJOR_VERSION * 10000 ) + \
        ( OPENSCENEGRAPH_MINOR_VERSION * 100 ) + \
        OPENSCENEGRAPH_PATCH_VERSION )

// Returns OSGWORKS_VERSION.
unsigned int OSGWTOOLS_EXPORT getVersionNumber();

// Pretty string.
std::string OSGWTOOLS_EXPORT getVersionString();


// namespace osgwTools
}

// __OSGWTOOLS_VERSION_H__
#endif
