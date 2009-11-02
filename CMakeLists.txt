# # # # # #  project setup  # # # # # #
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
PROJECT( osgWorks )

SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}" )

SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single directory for all libraries." )
MAKE_DIRECTORY( ${LIBRARY_OUTPUT_PATH} )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )

SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single directory for all executables." )
MAKE_DIRECTORY( ${EXECUTABLE_OUTPUT_PATH} )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )

INCLUDE( CMakeMacros )

FIND_PACKAGE( OSG REQUIRED )

# Add OSG for entire project for include & lib paths
INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/include
    ${OSG_INCLUDE_DIRS}
)
# HACK: Not sure why this is necessary, but linket can't find osgwToolsd.lib without it.
LINK_DIRECTORIES(
    ${EXECUTABLE_OUTPUT_PATH}/Debug
)


OPTION( BUILD_TESTS "Enable to build test code" OFF )
IF( BUILD_TESTS )
    ADD_SUBDIRECTORY( tests )
ENDIF( BUILD_TESTS )

OPTION( BUILD_APPS "Enable to build applications" ON )
IF( BUILD_APPS )
    ADD_SUBDIRECTORY( apps )
ENDIF( BUILD_APPS )

ADD_SUBDIRECTORY( src/osgwControls )
ADD_SUBDIRECTORY( src/osgwTools )
ADD_SUBDIRECTORY( src/plugins )
