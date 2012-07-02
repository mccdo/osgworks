# # # # # #  project setup  # # # # # #
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( osgWorks )

# Note: These values are used by include/osgwTools/Version.in
# to generate the Version.h file.
set( OSGWORKS_VERSION 2.00.02 )
set( OSGWORKS_MAJOR_VERSION 2 )
set( OSGWORKS_MINOR_VERSION 0 )
set( OSGWORKS_SUB_VERSION 2 )

SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" ${CMAKE_MODULE_PATH} )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
if( WIN32 )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
else()
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endif()


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

# find the optional Directx SDK (for DirectInput game pad support).
find_package( DirectInput QUIET )

# find the optional VRPN library (for VRPN input support).
find_package( VRPN QUIET )

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

option( OSGWORKS_BUILD_EXAMPLES "Enable to build example code" OFF )
if( OSGWORKS_BUILD_EXAMPLES )
    ADD_SUBDIRECTORY( examples )
endif()

option( OSGWORKS_INSTALL_DATA "Enable to add the data directory to the install target" ON )
if( OSGWORKS_INSTALL_DATA )
    add_subdirectory( data )
endif()


ADD_SUBDIRECTORY( src )


#
# Doxygen
find_package( Doxygen )
if( DOXYGEN_FOUND )
    set( HAVE_DOT "NO" )
    if( DOXYGEN_DOT_PATH )
        set( HAVE_DOT "YES" )
    endif()
    
    configure_file( ${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
    add_custom_target( Documentation ${DOXYGEN_EXECUTABLE}
        ${PROJECT_BINARY_DIR}/doc/doxyfile
    )
    
    add_subdirectory( doc/images )
endif()

# On windows provide the user with the pdb files 
# for debugging if they are present
if(MSVC)
    install(
        DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\${CMAKE_INSTALL_CONFIG_NAME}/
        DESTINATION lib
        USE_SOURCE_PERMISSIONS
        COMPONENT libosgworks-dev
        FILES_MATCHING PATTERN "*.pdb"
    )
endif(MSVC)

# Install the CMake find script for osgWorks
install( FILES "extra/FindosgWorks.cmake"
    DESTINATION "share/${CMAKE_PROJECT_NAME}/extra"
    COMPONENT libosgworks
)

#
# Create and install all of the auto find tools
# =============================================

# Add all targets to the build-tree export set
export(TARGETS osgwTools osgwQuery osgwMx
  FILE "${PROJECT_BINARY_DIR}/lib/osgWorksLibraryDepends.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE osgWorks)

# Create a osgWorksBuildTreeSettings.cmake file for the use from the build tree
configure_file(osgWorksBuildTreeSettings.cmake.in
  "${PROJECT_BINARY_DIR}/lib/osgWorksBuildTreeSettings.cmake" @ONLY)

#configure_file(osgWorksConfig.cmake.in
#  "${PROJECT_BINARY_DIR}/lib/osgWorksConfig.cmake" @ONLY)
configure_file(osgWorksConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/lib/osgWorksConfigVersion.cmake" @ONLY)
configure_file(UseosgWorks.cmake.in
  "${PROJECT_BINARY_DIR}/lib/UseosgWorks.cmake" @ONLY)

# Install the export set for use with the install-tree
install(EXPORT osgworks-targets DESTINATION lib)

include( CMakePackageConfigHelpers )
# Create the osgWorksConfig.cmake and osgWorksConfigVersion files
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/lib"
   "${CMAKE_INSTALL_PREFIX}/include")

configure_package_config_file( 
    osgWorksConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/lib/osgWorksConfig.cmake
    INSTALL_DESTINATION lib
    PATH_VARS CONF_REL_INCLUDE_DIR )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/osgWorksConfig.cmake 
              ${CMAKE_CURRENT_BINARY_DIR}/lib/osgWorksConfigVersion.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/lib/UseosgWorks.cmake
        DESTINATION lib )

#
# Setup the fpc auto detection files
if(WIN32)
    set(OSGWORKS_FPC_LIBRARIES "/libpath:\"\${libdir}\" osgwControls.lib osgwMx.lib osgwTools.lib osgwQuery.lib")
    set(OSGWORKS_FPC_INCLUDES "/I\"\${includedir}\"")
else(WIN32)
    set(OSGWORKS_FPC_INCLUDES "-I\${includedir}")
    if(APPLE)
        set(OSGWORKS_FPC_LIBRARIES "-L\${libdir} -losgwControls -losgwMx -losgwTools -losgwQuery")
    else(APPLE)
        set(OSGWORKS_FPC_LIBRARIES "-L\${libdir} -losgwControls -losgwMx -losgwTools -losgwQuery")
    endif(APPLE)
endif(WIN32)

configure_file(extra/osgworks.fpc.in
  "${PROJECT_BINARY_DIR}/lib/flagpoll/osgworks.fpc" @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/flagpoll/osgworks.fpc
        DESTINATION lib/flagpoll )


