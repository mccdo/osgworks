// Copyright (c) 2011 Digital Transforms.

#include <osgwMx/MxMovementStyle.h>

namespace osgwMx
{

// **************************************************************************

MxMovementStyle::MxMovementStyle(MxInputAdapter *ia) :
   _mxCore(new MxCore),
   _inputAdapter(ia),
   _chgRate(1.0),
   _rotating(false),
   _moving(false),
   _zooming(false),
   _dollying(false),
   _lastRotateTime(0),
   _lastMoveTime(0),
   _lastZoomTime(0),
   _lastDollyTime(0),
   _reverseRotateX(false),
   _reverseRotateY(false),
   _reverseMoveX(false),
   _reverseMoveY(false),
   _reverseZoom(false),
   _reverseDolly(false)

{
    _mxCore->setMxCoreMode( MxCore::FIRST_PERSON );
}

// **************************************************************************

osg::NodeCallback *MxMovementStyle::getMatrixCallback()

{
   if (_cameraUpdateCallback.valid() == false)
      _cameraUpdateCallback = new CameraUpdateCallback(_mxCore.get());
   return _cameraUpdateCallback.get();
}

// osgwMx
}
