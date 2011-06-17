set( LIB_NAME osgwQuery )

if( BUILD_SHARED_LIBS )
    add_definitions( -DOSGWQUERY_LIBRARY )
endif()


set( HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME} )
set( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/QueryAPI.h
    ${HEADER_PATH}/QueryBin.h
    ${HEADER_PATH}/QueryObject.h
    ${HEADER_PATH}/Export.h
)

set( _osgwQuerySourceFiles
    QueryAPI.cpp
    QueryBin.cpp
    QueryObject.cpp
)

add_library( ${LIB_NAME}
    ${LIB_PUBLIC_HEADERS}
    ${_osgwQuerySourceFiles}
)
if( BUILD_SHARED_LIBS )
    target_link_libraries( ${LIB_NAME}
        ${OSG_LIBRARIES}
    )
endif()

set_target_properties( ${LIB_NAME} PROPERTIES VERSION ${OSGWORKS_VERSION} )
set_target_properties( ${LIB_NAME} PROPERTIES SOVERSION ${OSGWORKS_VERSION} )
set_target_properties( ${LIB_NAME} PROPERTIES PROJECT_LABEL "Lib ${LIB_NAME}" )

include( ModuleInstall REQUIRED )
