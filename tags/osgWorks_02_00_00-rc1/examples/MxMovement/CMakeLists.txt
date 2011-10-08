SET( CATEGORY Example )

if( DIRECTINPUT_FOUND )
   MAKE_EXECUTABLE( MxMovement
      MxMovementExample.cpp
   )
endif()
