SET( LIB_NAME osgdb_osgwTools )

ADD_OSGPLUGIN( ${LIB_NAME}
    dotosgAbsoluteModelTransform.cpp
    dotosgRefID.cpp
)

INCLUDE(ModuleInstall OPTIONAL)
