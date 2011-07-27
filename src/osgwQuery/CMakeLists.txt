set( LIB_NAME osgwQuery )

if( BUILD_SHARED_LIBS )
    add_definitions( -DOSGWQUERY_LIBRARY )
endif()


set( HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME} )
set( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/NodeData.h
    ${HEADER_PATH}/QueryAPI.h
    ${HEADER_PATH}/QueryBenchmarks.h
    ${HEADER_PATH}/QueryBin.h
    ${HEADER_PATH}/QueryDrawCB.h
    ${HEADER_PATH}/QueryObject.h
    ${HEADER_PATH}/QueryStats.h
    ${HEADER_PATH}/Export.h
)

set( _osgwQuerySourceFiles
    NodeData.cpp
    QueryAPI.cpp
    QueryBenchmarks.cpp
    QueryBin.cpp
    QueryDrawCB.cpp
    QueryObject.cpp
    QueryStats.cpp
)

add_library( ${LIB_NAME}
    ${LIB_PUBLIC_HEADERS}
    ${_osgwQuerySourceFiles}
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
    # osgwQuery depends on osgwTools
    target_link_libraries( ${LIB_NAME}
        optimized "osgwTools"
        debug "${RELATIVE_LIB_PATH}osgwTools${CMAKE_DEBUG_POSTFIX}"
    )
endif()

set_target_properties( ${LIB_NAME} PROPERTIES VERSION ${OSGWORKS_VERSION} )
set_target_properties( ${LIB_NAME} PROPERTIES SOVERSION ${OSGWORKS_VERSION} )
set_target_properties( ${LIB_NAME} PROPERTIES PROJECT_LABEL "Lib ${LIB_NAME}" )

include( ModuleInstall REQUIRED )
