set( LIB_NAME osgwMx )

if( BUILD_SHARED_LIBS )
    add_definitions( -DOSGWMX_LIBRARY )
endif()


set( HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME} )
set( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/CameraUpdateCallback.h
    ${HEADER_PATH}/MxCore.h
    ${HEADER_PATH}/MxEventHandler.h
    ${HEADER_PATH}/Export.h
)

set( _osgwMxSourceFiles
    CameraUpdateCallback.cpp
    MxCore.cpp
    MxEventHandler.cpp
)

add_library( ${LIB_NAME}
    ${LIB_PUBLIC_HEADERS}
    ${_osgwMxSourceFiles}
)

find_package( OpenGL REQUIRED )
include_directories(
    ${OPENGL_INCLUDE_DIR}
)

if( BUILD_SHARED_LIBS )
    # Shared libs must explicitly resolve all symbols.
    # Link against our dependency: OSG.
    target_link_libraries( ${LIB_NAME}
        ${OSG_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )

    if( WIN32 )
        set( RELATIVE_LIB_PATH ../../lib/ )
    endif()
    # osgwMx depends on osgwTools
    target_link_libraries( ${LIB_NAME}
        optimized "osgwTools"
        debug "${RELATIVE_LIB_PATH}osgwTools${CMAKE_DEBUG_POSTFIX}"
    )
endif()

set_target_properties( ${LIB_NAME} PROPERTIES VERSION ${OSGWORKS_VERSION} )
set_target_properties( ${LIB_NAME} PROPERTIES SOVERSION ${OSGWORKS_VERSION} )
set_target_properties( ${LIB_NAME} PROPERTIES PROJECT_LABEL "Lib ${LIB_NAME}" )

include( ModuleInstall REQUIRED )
