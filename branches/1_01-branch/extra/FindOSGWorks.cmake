# Locate osgWorks.
#
# This script defines:
#   OSGWORKS_FOUND, set to 1 if found
#   OSGWORKS_LIBRARIES
#   OSGWORKS_INCLUDE_DIR
#   OSGWCONTROLS_LIBRARY
#   OSGWTOOLS_LIBRARY
#
# This script will look in standard locations for installed osgWorks. However, if you
# install osgWorks into a non-standard location, you can use the OSGWORKS_ROOT
# variable (in environment or CMake) to specify the location.
#
# You can also use osgWorks out of a source tree by specifying OSGWORKS_SOURCE_DIR
# and OSGWORKS_BUILD_DIR (in environment or CMake).


SET( OSGWORKS_BUILD_DIR "" CACHE PATH "If using osgWorks out of a source tree, specify the build directory." )
SET( OSGWORKS_SOURCE_DIR "" CACHE PATH "If using osgWorks out of a source tree, specify the root of the source tree." )
SET( OSGWORKS_ROOT "" CACHE PATH "Specify non-standard osgWorks install directory. It is the parent of the include and lib dirs." )

MACRO( FIND_OSGWORKS_INCLUDE THIS_OSGWORKS_INCLUDE_DIR THIS_OSGWORKS_INCLUDE_FILE )
    UNSET( ${THIS_OSGWORKS_INCLUDE_DIR} CACHE )
    MARK_AS_ADVANCED( ${THIS_OSGWORKS_INCLUDE_DIR} )
    FIND_PATH( ${THIS_OSGWORKS_INCLUDE_DIR} ${THIS_OSGWORKS_INCLUDE_FILE}
        PATHS
            ${OSGWORKS_ROOT}
            $ENV{OSGWORKS_ROOT}
            ${OSGWORKS_SOURCE_DIR}
            $ENV{OSGWORKS_SOURCE_DIR}
            /usr/local
            /usr
            /sw/ # Fink
            /opt/local # DarwinPorts
            /opt/csw # Blastwave
            /opt
            "C:/Program Files/osgWorks"
            "C:/Program Files (x86)/osgWorks"
            ~/Library/Frameworks
            /Library/Frameworks
        PATH_SUFFIXES
            include
            .
    )
ENDMACRO( FIND_OSGWORKS_INCLUDE THIS_OSGWORKS_INCLUDE_DIR THIS_OSGWORKS_INCLUDE_FILE )

FIND_OSGWORKS_INCLUDE( OSGWORKS_INCLUDE_DIR osgwTools/FindNamedNode.h )
# message( STATUS ${OSGWORKS_INCLUDE_DIR} )

MACRO( FIND_OSGWORKS_LIBRARY MYLIBRARY MYLIBRARYNAME )
    UNSET( ${MYLIBRARY} CACHE )
    UNSET( ${MYLIBRARY}_debug CACHE )
    MARK_AS_ADVANCED( ${MYLIBRARY} )
    MARK_AS_ADVANCED( ${MYLIBRARY}_debug )
    FIND_LIBRARY( ${MYLIBRARY}
        NAMES ${MYLIBRARYNAME}
        PATHS
            ${OSGWORKS_ROOT}
            $ENV{OSGWORKS_ROOT}
            ${OSGWORKS_BUILD_DIR}
            $ENV{OSGWORKS_BUILD_DIR}
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /usr
            /sw
            /opt/local
            /opt/csw
            /opt
            "C:/Program Files/osgWorks"
            "C:/Program Files (x86)/osgWorks"
            /usr/freeware/lib64
        PATH_SUFFIXES
            lib
            .
    )
    FIND_LIBRARY( ${MYLIBRARY}_debug
        NAMES ${MYLIBRARYNAME}d
        PATHS
            ${OSGWORKS_ROOT}
            $ENV{OSGWORKS_ROOT}
            ${OSGWORKS_BUILD_DIR}
            $ENV{OSGWORKS_BUILD_DIR}
            ~/Library/Frameworks
            /Library/Frameworks
            /usr/local
            /usr
            /sw
            /opt/local
            /opt/csw
            /opt
            "C:/Program Files/osgWorks"
            "C:/Program Files (x86)/osgWorks"
            /usr/freeware/lib64
        PATH_SUFFIXES
            lib
            .
    )
#    message( STATUS ${${MYLIBRARY}} ${${MYLIBRARY}_debug} )
#    message( STATUS ${MYLIBRARYNAME} )
    IF( ${MYLIBRARY} )
        SET( OSGWORKS_LIBRARIES ${OSGWORKS_LIBRARIES}
            "optimized" ${${MYLIBRARY}}
        )
    ENDIF( ${MYLIBRARY} )
    IF( ${MYLIBRARY}_debug )
        SET( OSGWORKS_LIBRARIES ${OSGWORKS_LIBRARIES}
            "debug" ${${MYLIBRARY}_debug}
        )
    ENDIF( ${MYLIBRARY}_debug )
ENDMACRO(FIND_OSGWORKS_LIBRARY LIBRARY LIBRARYNAME)

FIND_OSGWORKS_LIBRARY( OSGWTOOLS_LIBRARY osgwTools )
FIND_OSGWORKS_LIBRARY( OSGWCONTROLS_LIBRARY osgwControls )

SET( OSGWORKS_FOUND 0 )
IF( OSGWORKS_LIBRARIES AND OSGWORKS_INCLUDE_DIR )
    SET( OSGWORKS_FOUND 1 )
ENDIF( OSGWORKS_LIBRARIES AND OSGWORKS_INCLUDE_DIR )
