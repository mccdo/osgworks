SET( LIB_NAME osgwTools )

ADD_DEFINITIONS( -DOSGWTOOLS_LIBRARY )
SET( HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME} )

SET( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/AbsoluteModelTransform.h
    ${HEADER_PATH}/FindNamedNode.h
    ${HEADER_PATH}/Grid.h
    ${HEADER_PATH}/InsertRemove.h
    ${HEADER_PATH}/ParallelVisitor.h
    ${HEADER_PATH}/RefID.h
    ${HEADER_PATH}/RemoveData.h
    ${HEADER_PATH}/Export.h
    ${HEADER_PATH}/DecimatorOp.h
    ${HEADER_PATH}/GeometryModifier.h
    ${HEADER_PATH}/Quat.h
    ${HEADER_PATH}/ScreenCapture.h
    ${HEADER_PATH}/ReducerOp.h
    ${HEADER_PATH}/GeometryOperation.h
    ${HEADER_PATH}/SimplifierOp.h
    ${HEADER_PATH}/Version.h
)

ADD_SHARED_LIBRARY_INTERNAL( ${LIB_NAME}
    ${LIB_PUBLIC_HEADERS}
    AbsoluteModelTransform.cpp
    FindNamedNode.cpp
    Grid.cpp
    InsertRemove.cpp
    ParallelVisitor.cpp
    RefID.cpp
    RemoveData.cpp
    DecimatorOp.cpp
    GeometryModifier.cpp
    Quat.cpp
    ScreenCapture.cpp
    ReducerOp.cpp
    GeometryOperation.cpp
    SimplifierOp.cpp
    Version.cpp
)

TARGET_LINK_LIBRARIES(
    ${LIB_NAME}
    ${OSG_LIBRARIES}
)

INCLUDE(ModuleInstall OPTIONAL)
