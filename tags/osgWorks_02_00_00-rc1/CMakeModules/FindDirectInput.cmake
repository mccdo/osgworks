# Locate directinput
# This module defines
# DIRECTINPUT_LIBRARIES
# DIRECTINPUT_FOUND, if false, do not try to link to directinput
# DIRECTINPUT_INCLUDE_DIR, where to find the headers
#
# $DIRECTINPUT_DIR is an environment variable that would
# point to the this path in the plateform devkit (Samples\Multimedia\DirectShow)
#
# Created by Cedric Pinson.
#

SET( DIRECTINPUT_FOUND FALSE )

IF( WIN32 )
    FIND_PATH( DIRECTINPUT_ROOT_DIR Include/dinput.h
               PATHS
               $ENV{PATH}
               $ENV{PROGRAMFILES}
    )

    FIND_PATH( DIRECTINPUT_INCLUDE_DIR dinput.h
               PATHS
               ${DIRECTINPUT_ROOT_DIR}/Include
    )

    FIND_LIBRARY( DIRECTINPUT_LIBRARY dinput8.lib
                  PATHS
                  ${DIRECTINPUT_ROOT_DIR}/lib/x86
                  ${DIRECTINPUT_ROOT_DIR}/lib
    )

    FIND_LIBRARY( DIRECTINPUT_GUID_LIBRARY dxguid.lib
                  PATHS
                  ${DIRECTINPUT_ROOT_DIR}/lib/x86
                  ${DIRECTINPUT_ROOT_DIR}/lib
    )

#    FIND_LIBRARY( DIRECTINPUT_ERR_LIBRARY dxerr.lib
#                  PATHS
#                  ${DIRECTINPUT_ROOT_DIR}/lib/x86
#                  ${DIRECTINPUT_ROOT_DIR}/lib
#    )

    IF ( DIRECTINPUT_INCLUDE_DIR AND DIRECTINPUT_LIBRARY AND DIRECTINPUT_GUID_LIBRARY )
        SET( DIRECTINPUT_FOUND TRUE )
        SET( DIRECTINPUT_LIBRARIES
           ${DIRECTINPUT_LIBRARY}
           ${DIRECTINPUT_GUID_LIBRARY}
#           ${DIRECTINPUT_ERR_LIBRARY}
           )
    ENDIF ()
ENDIF( WIN32 )

MARK_AS_ADVANCED( DIRECTINPUT_FOUND )
MARK_AS_ADVANCED( DIRECTINPUT_INCLUDE_DIR )
MARK_AS_ADVANCED( DIRECTINPUT_LIBRARY )
MARK_AS_ADVANCED( DIRECTINPUT_GUID_LIBRARY )
#MARK_AS_ADVANCED( DIRECTINPUT_ERR_LIBRARY )
