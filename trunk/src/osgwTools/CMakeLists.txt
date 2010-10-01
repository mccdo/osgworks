SET( LIB_NAME osgwTools )

ADD_DEFINITIONS( -DOSGWTOOLS_LIBRARY )
SET( HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME} )

SET( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/Export.h
    ${HEADER_PATH}/AbsoluteModelTransform.h
    ${HEADER_PATH}/CallbackSupport.h
    ${HEADER_PATH}/CameraConfigObject.h
    ${HEADER_PATH}/DecimatorOp.h
    ${HEADER_PATH}/FindNamedNode.h
    ${HEADER_PATH}/GeometryModifier.h
    ${HEADER_PATH}/GeometryOperation.h
    ${HEADER_PATH}/RemoveLOD.h
    ${HEADER_PATH}/InsertRemove.h
    ${HEADER_PATH}/ParallelVisitor.h
    ${HEADER_PATH}/Quat.h
    ${HEADER_PATH}/ReadFile.h
    ${HEADER_PATH}/ReducerOp.h
    ${HEADER_PATH}/RefID.h
    ${HEADER_PATH}/RemoveData.h
    ${HEADER_PATH}/ScreenCapture.h
    ${HEADER_PATH}/Shapes.h
    ${HEADER_PATH}/SimplifierOp.h
    ${HEADER_PATH}/Transform.h
    ${HEADER_PATH}/Version.h
)

ADD_SHARED_LIBRARY_INTERNAL( ${LIB_NAME}
    ${LIB_PUBLIC_HEADERS}
    AbsoluteModelTransform.cpp
    CallbackSupport.cpp
    CameraConfigObject.cpp
    DecimatorOp.cpp
    FindNamedNode.cpp
    GeometryModifier.cpp
    GeometryOperation.cpp
    RemoveLOD.cpp
    InsertRemove.cpp
    ParallelVisitor.cpp
    PluginLoader.cpp
    Quat.cpp
    ReadFile.cpp
    ReducerOp.cpp
    RefID.cpp
    RemoveData.cpp
    ScreenCapture.cpp
    Shapes.cpp
    SimplifierOp.cpp
    Version.cpp
    Transform.cpp
)

TARGET_LINK_LIBRARIES(
    ${LIB_NAME}
    ${OSG_LIBRARIES}
)

INCLUDE(ModuleInstall OPTIONAL)
