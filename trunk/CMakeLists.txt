# # # # # #  project setup  # # # # # #
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( osgWorks )

set( OSGWORKS_VERSION 1.1 )

SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}" )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
if( WIN32 )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
else()
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endif()
foreach( CONF ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER "${CONF}" CONF )
    set( "CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
    set( "CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
    if( WIN32 )
        set( "CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
    else()
        set( "CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
    endif()
endforeach()


IF( WIN32 AND MSVC )
    OPTION( OSGWORKS_WIN32_USE_MP "Build with multiple processes (/MP option; VS 2005 and above)." ON )
    MARK_AS_ADVANCED( OSGWORKS_WIN32_USE_MP )
    IF( OSGWORKS_WIN32_USE_MP )
        SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
    ENDIF( OSGWORKS_WIN32_USE_MP )
ENDIF( WIN32 AND MSVC )


include( CMakeMacros )

include( FindOSGHelper )

find_package( Boost )
if( Boost_FOUND )
    add_definitions( -DBOOST_FOUND )
    include_directories( ${Boost_INCLUDE_DIR} )
endif()

# Add OSG for entire project for include & lib paths
INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/include
    ${OSG_INCLUDE_DIRS}
)


option( BUILD_SHARED_LIBS "Set to ON to build osgWorks for dynamic linking. Use OFF for static." ON )
if( NOT BUILD_SHARED_LIBS )
    add_definitions( -DOSGWORKS_STATIC )
endif()

OPTION( OSGWORKS_SCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT "Enable to support OSG_STEREO=ON." OFF )
IF( OSGWORKS_SCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT )
    ADD_DEFINITIONS( -DSCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT )
ENDIF( OSGWORKS_SCENEVIEW_ANAGLYPHIC_STEREO_SUPPORT )


option( OSGWORKS_BUILD_TESTS "Enable to build test code" OFF )
if( OSGWORKS_BUILD_TESTS )
    enable_testing()
    add_definitions( -DOSGWORKS_BUILD_TESTS )
    add_subdirectory( tests )
endif()

OPTION( OSGWORKS_BUILD_APPS "Enable to build applications" ON )
IF( OSGWORKS_BUILD_APPS )
    ADD_SUBDIRECTORY( apps )
ENDIF( OSGWORKS_BUILD_APPS )

ADD_SUBDIRECTORY( src/osgwControls )
ADD_SUBDIRECTORY( src/osgwTools )
ADD_SUBDIRECTORY( src/plugins )


#
# Doxygen
#INCLUDE( ${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL )
OPTION( OSGWORKS_DOCUMENTATION "Enable to create build target for Doxygen documentation." OFF )
MARK_AS_ADVANCED( CLEAR OSGWORKS_DOCUMENTATION )
IF( OSGWORKS_DOCUMENTATION )
    FIND_PACKAGE( Doxygen REQUIRED )
    SET( HAVE_DOT "NO" )
    IF( DOXYGEN_DOT_PATH )
        SET( HAVE_DOT "YES" )
    ENDIF( DOXYGEN_DOT_PATH )
    
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
    ADD_CUSTOM_TARGET( Documentation ${DOXYGEN_EXECUTABLE}
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
    
    add_subdirectory( doc/images )
endif()
