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

#ifndef __OSGWTOOLS_EXPORT__
#define __OSGWTOOLS_EXPORT__ 1


#if defined( _MSC_VER ) || defined( __CYGWIN__ ) || defined( __MINGW32__ ) || defined( __BCPLUSPLUS__ ) || defined( __MWERKS__ )
    #if defined( OSGWORKS_STATIC )
        #define OSGWTOOLS_EXPORT
    #elif defined( OSGWTOOLS_LIBRARY )
        #define OSGWTOOLS_EXPORT __declspec( dllexport )
    #else
        #define OSGWTOOLS_EXPORT __declspec( dllimport )
    #endif
#else
    #define OSGWTOOLS_EXPORT
#endif

//This is ripped straight from wxWidgets
#if defined(_MSC_VER)
#define osgwDEPRECATED(x) __declspec(deprecated) x
#else
#define osgwDEPRECATED(x) x __attribute__ ((deprecated))
#endif

// __OSGWTOOLS_EXPORT__
#endif
