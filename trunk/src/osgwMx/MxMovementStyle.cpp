/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2011 by Kenneth Mark Bryden
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#include <osgwMx/MxMovementStyle.h>

namespace osgwMx
{

// **************************************************************************

MxMovementStyle::MxMovementStyle(MxInputAdapter *ia)
  : _mxCore(new MxCore),
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
