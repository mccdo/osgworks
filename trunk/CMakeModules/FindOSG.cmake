# Locate OpenSceneGraph.
#
# This script defines:
#   OSG_FOUND, set to "YES" or "NO".   
#   OSG_LIBRARIES
#   OSG_INCLUDE_DIR
#   OSG_GEN_INCLUDE_DIR, OSG's CMake-generated "Config" header files directory.
#   OSG_INCLUDE_DIRS, both OSG_INCLUDE_DIR and OSG_GEN_INCLUDE_DIR together.
#   OSG*_LIBRARY, one for each library.
#   OSG*_LIBRARY_debug, one for each library.
#   OSG_LIBRARIES_DIR, path to the OSG libraries.
#
# This script will look in standard locations for installed OSG. However, if you
# install OSG into a non-standard location, you can use the OSG_ROOT
# variable (in environment or CMake) to specify the location.
#
# You can also use OSG out of a source tree by specifying OSG_SOURCE_DIR
# and OSG_BUILD_DIR (in environment or CMake).


SET( OSG_BUILD_DIR "" CACHE PATH "If using OSG out of a source tree, specify the build directory." )
SET( OSG_SOURCE_DIR "" CACHE PATH "If using OSG out of a source tree, specify the root of the source tree." )
SET( OSG_ROOT "" CACHE PATH "Specify non-standard OSG install directory. It is the parent of the include and lib dirs." )

MACRO( FIND_OSG_INCLUDE THIS_OSG_INCLUDE_DIR THIS_OSG_INCLUDE_FILE )
    MARK_AS_ADVANCED( ${THIS_OSG_INCLUDE_DIR} )
    FIND_PATH( ${THIS_OSG_INCLUDE_DIR} ${THIS_OSG_INCLUDE_FILE}
        PATHS
            ${OSG_ROOT}
            $ENV{OSG_ROOT}
            ${OSG_SOURCE_DIR}
            $ENV{OSG_SOURCE_DIR}
            /usr/local
            /usr
            /sw # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            "C:/Program Files/OpenSceneGraph"
            ~/Library/Frameworks
            /Library/Frameworks
        PATH_SUFFIXES
            include
    )
ENDMACRO( FIND_OSG_INCLUDE THIS_OSG_INCLUDE_DIR THIS_OSG_INCLUDE_FILE )

FIND_OSG_INCLUDE( OSG_INCLUDE_DIR osg/PositionAttitudeTransform )
FIND_OSG_INCLUDE( OSG_GEN_INCLUDE_DIR osg/Config )

MACRO(FIND_OSG_LIBRARY MYLIBRARY MYLIBRARYNAME)
    MARK_AS_ADVANCED( ${MYLIBRARY} )
    MARK_AS_ADVANCED( ${MYLIBRARY}_debug )
    FIND_LIBRARY( ${MYLIBRARY}
        NAMES
            ${MYLIBRARYNAME}
        PATHS
            ${OSG_ROOT}
            $ENV{OSG_ROOT}
            ${OSG_BUILD_DIR}
            $ENV{OSG_BUILD_DIR}
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /usr
            /sw
            /opt/local
            /opt/csw
            /opt
            "C:/Program Files/OpenSceneGraph"
            /usr/freeware/lib64
        PATH_SUFFIXES
            lib
            .
    )
    FIND_LIBRARY( ${MYLIBRARY}_debug
        NAMES
            ${MYLIBRARYNAME}d
        PATHS
            ${OSG_ROOT}
            $ENV{OSG_ROOT}
            ${OSG_BUILD_DIR}
            $ENV{OSG_BUILD_DIR}
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /usr
            /sw
            /opt/local
            /opt/csw
            /opt
            "C:/Program Files/OpenSceneGraph"
            /usr/freeware/lib64
        PATH_SUFFIXES
            lib
            .
    )
#    message( STATUS ${${MYLIBRARY}} ${${MYLIBRARY}_debug} )
#    message( SEND_ERROR ${MYLIBRARYNAME} )
    IF( ${MYLIBRARY} )
        SET( OSG_LIBRARIES ${OSG_LIBRARIES}
            "optimized" ${${MYLIBRARY}}
        )
    ENDIF( ${MYLIBRARY} )
    IF( ${MYLIBRARY}_debug )
        SET( OSG_LIBRARIES ${OSG_LIBRARIES}
            "debug" ${${MYLIBRARY}_debug}
        )
    ENDIF( ${MYLIBRARY}_debug )
ENDMACRO(FIND_OSG_LIBRARY LIBRARY LIBRARYNAME)

# FIND_OSG_LIBRARY(OSGTERRAIN_LIBRARY osgTerrain)
# FIND_OSG_LIBRARY(OSGFX_LIBRARY osgFX)

FIND_OSG_LIBRARY(OSGSIM_LIBRARY osgSim)
FIND_OSG_LIBRARY(OSGTEXT_LIBRARY osgText)
FIND_OSG_LIBRARY(OSGVIEWER_LIBRARY osgViewer)
FIND_OSG_LIBRARY(OSGGA_LIBRARY osgGA)
FIND_OSG_LIBRARY(OSGDB_LIBRARY osgDB)
FIND_OSG_LIBRARY(OSGUTIL_LIBRARY osgUtil)
FIND_OSG_LIBRARY(OSG_LIBRARY osg)
FIND_OSG_LIBRARY(OPENTHREADS_LIBRARY OpenThreads)

SET( OSG_FOUND "NO" )
IF( OSG_LIBRARIES AND OSG_INCLUDE_DIR )
    SET( OSG_FOUND "YES")
    SET( OSG_INCLUDE_DIRS ${OSG_INCLUDE_DIR} ${OSG_GEN_INCLUDE_DIR} )
    GET_FILENAME_COMPONENT( OSG_LIBRARIES_DIR ${OSG_LIBRARY} PATH )
ENDIF( OSG_LIBRARIES AND OSG_INCLUDE_DIR )

