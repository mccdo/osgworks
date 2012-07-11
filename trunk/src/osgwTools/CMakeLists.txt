set( LIB_NAME osgwTools )

if( BUILD_SHARED_LIBS )
    add_definitions( -DOSGWTOOLS_LIBRARY )
endif()


set( HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${LIB_NAME} )

configure_file( "${HEADER_PATH}/Version.h.in" "${HEADER_PATH}/Version.h" @ONLY)

set( LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/Export.h
    ${HEADER_PATH}/AbsoluteModelTransform.h
    ${HEADER_PATH}/CallbackSupport.h
    ${HEADER_PATH}/CameraConfigObject.h
    ${HEADER_PATH}/CountStateSets.h
    ${HEADER_PATH}/CountsVisitor.h
    ${HEADER_PATH}/DataLoader.h
    ${HEADER_PATH}/DecimatorOp.h
    ${HEADER_PATH}/DecimationTestModel.h
    ${HEADER_PATH}/FBOUtils.h
    ${HEADER_PATH}/FindNamedNode.h
    ${HEADER_PATH}/ForceFlattenTransforms.h
    ${HEADER_PATH}/GeometryModifier.h
    ${HEADER_PATH}/GeometryOperation.h
    ${HEADER_PATH}/CollapseLOD.h
    ${HEADER_PATH}/LODCreationNodeVisitor.h
    ${HEADER_PATH}/NodePathUtils.h
    ${HEADER_PATH}/NodeUtils.h
    ${HEADER_PATH}/InsertRemove.h
    ${HEADER_PATH}/ParallelVisitor.h
    ${HEADER_PATH}/PrimitiveSetConversion.h
    ${HEADER_PATH}/Quat.h
    ${HEADER_PATH}/ReadFile.h
    ${HEADER_PATH}/ReducerOp.h
    ${HEADER_PATH}/RefID.h
    ${HEADER_PATH}/RemoveData.h
    ${HEADER_PATH}/RemoveLOD.h
    ${HEADER_PATH}/RemoveProgram.h
    ${HEADER_PATH}/ScreenCapture.h
    ${HEADER_PATH}/Shapes.h
    ${HEADER_PATH}/ShortEdgeOp.h
    ${HEADER_PATH}/SimplifierOp.h
    ${HEADER_PATH}/StateSetUtils.h
    ${HEADER_PATH}/StateTrackingNodeVisitor.h
    ${HEADER_PATH}/TangentSpaceGeneratorDouble.h
    ${HEADER_PATH}/TangentSpaceOp.h
    ${HEADER_PATH}/Transform.h
    ${HEADER_PATH}/TransparencyUtils.h
    ${HEADER_PATH}/Uniqifier.h
    ${HEADER_PATH}/Version.h
)

set( _osgwToolsSourceFiles
    AbsoluteModelTransform.cpp
    CallbackSupport.cpp
    CameraConfigObject.cpp
    CountStateSets.cpp
    CountsVisitor.cpp
    DecimatorOp.cpp
    FBOUtils.cpp
    FindNamedNode.cpp
    ForceFlattenTransforms.cpp
    GeometryModifier.cpp
    GeometryOperation.cpp
    CollapseLOD.cpp
    LODCreationNodeVisitor.cpp
    NodePathUtils.cpp
    NodeUtils.cpp
    InsertRemove.cpp
    ParallelVisitor.cpp
    PluginLoader.cpp
    PrimitiveSetConversion.cpp
    Quat.cpp
    ReadFile.cpp
    ReducerOp.cpp
    RefID.cpp
    RemoveData.cpp
    RemoveLOD.cpp
    RemoveProgram.cpp
    ScreenCapture.cpp
    Shapes.cpp
    ShortEdgeOp.cpp
    SimplifierOp.cpp
    StateSetUtils.cpp
    StateTrackingNodeVisitor.cpp
    TangentSpaceGeneratorDouble.cpp
    TangentSpaceOp.cpp
    Transform.cpp
    TransparencyUtils.cpp
    Uniqifier.cpp
    Version.cpp
)

add_library( ${LIB_NAME}
    ${LIB_PUBLIC_HEADERS}
    ${_osgwToolsSourceFiles}
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
