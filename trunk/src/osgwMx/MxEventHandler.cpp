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

#include <osgwMx/MxEventHandler.h>
#include <osgwMx/MxCore.h>

#include <osg/Notify>


namespace osgwMx
{


MxEventHandler::MxEventHandler()
  : _mxCore( new MxCore ),
    _cameraUpdateCallback( NULL ),
    _lastX( 0.0 ),
    _lastY( 0.0 ),
    _lastXPixel( 0.f ),
    _lastYPixel( 0.f ),
    _leftDragging( false ),
    _leftClick( false )
{
}
MxEventHandler::MxEventHandler( const MxEventHandler& rhs, const osg::CopyOp& copyop )
  : osgGA::GUIEventHandler( rhs, copyop ),
    _mxCore( rhs._mxCore ),
    _cameraUpdateCallback( rhs._cameraUpdateCallback ),
    _lastX( rhs._lastX ),
    _lastY( rhs._lastY ),
    _lastXPixel( rhs._lastXPixel ),
    _lastYPixel( rhs._lastYPixel ),
    _leftDragging( rhs._leftDragging ),
    _leftClick( rhs._leftClick )
{
}
MxEventHandler::~MxEventHandler()
{
}

bool MxEventHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    const bool ctrlKey( ( ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL ) != 0 );
    const bool shiftKey( ( ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT ) != 0 );

    bool handled( false );
    switch( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::PUSH:
    {
        _lastX = ea.getXnormalized();
        _lastY = ea.getYnormalized();
        _lastXPixel = ea.getX();
        _lastYPixel = ea.getY();

        _leftClick = ( ( ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) != 0 );
        if( _leftClick )
            _mxCore->setPanStart( _lastX, _lastY );

        handled = true;
        break;
    }
    case osgGA::GUIEventAdapter::DRAG:
    {
        const double deltaX = ea.getXnormalized() - _lastX;
        const double deltaY = ea.getYnormalized() - _lastY;

        if( ( ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ) != 0 )
        {
            // Allow for some sloppiness in the DRAG event in order to properly detect a click.
            _leftDragging = ( _leftDragging ||
                // or manhatten distance > 5:
                ( ( osg::absolute< float >( ea.getX() - _lastXPixel ) +
                    osg::absolute< float >( ea.getY() - _lastYPixel ) ) > 5 ) );

            if( !shiftKey )
            {
                // Left mouse, unshifted: rotation
                _mxCore->rotate( osg::Vec2d( _lastX, _lastY ),
                    osg::Vec2d( deltaX, deltaY ) );
                handled = true;
            }
            else if( shiftKey )
            {
                // Left mouse, shifted: pan
                _mxCore->pan( deltaX, deltaY );
                handled = true;
            }
        }
        else if( ( ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON ) != 0 )
        {
            // Right mouse: dolly forward/backward
            _mxCore->dolly( deltaY );
            handled = true;
        }
        _lastX = ea.getXnormalized();
        _lastY = ea.getYnormalized();
        break;
    }
    case osgGA::GUIEventAdapter::RELEASE:
    {
        if( _leftClick )
        {
            if( !_leftDragging && ctrlKey && shiftKey )
            {
                // Parameters are NDC coordinates in range -1.0,1.0.
                _mxCore->pickCenter(
                    ea.getXnormalized(),
                    ea.getYnormalized() );
                // Go to third person (orbit) view
                _mxCore->setMxCoreMode( MxCore::THIRD_PERSON );
            }
            _leftDragging = false;
            _leftClick = false;
        }
        break;
    }
    case osgGA::GUIEventAdapter::SCROLL:
    {
        // Scroll wheel: FOV
        if( ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP )
        {
            _mxCore->fovyScaleUp();
            handled = true;
        }
        else if( ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN )
        {
            _mxCore->fovyScaleDown();
            handled = true;
        }
        break;
    }
    case osgGA::GUIEventAdapter::KEYDOWN:
    {
        switch (ea.getKey())
        {
        case 'o': // toggle orthographic
            _mxCore->setOrtho( !( _mxCore->getOrtho() ) );
            handled = true;
            break;
        case 't': // Go to first person view.
            _mxCore->setMxCoreMode( MxCore::FIRST_PERSON );
            handled = true;
            break;
        case 'b': // View the bottom of the model.
            _mxCore->viewBottom();
            handled = true;
            break;
        case 'd': // Display current yaw / pitch / roll values
        {
            double y, p, r;
            _mxCore->getYawPitchRoll( y, p, r );
            osg::notify( osg::ALWAYS ) << "Yaw: " << y << "\tPitch: " << p << "\tRoll: " << r << std::endl;
            break;
        }
        case ' ': // Compute and go to initial view.
            _mxCore->computeInitialView();
            handled = true;
            break;
        }
        break;
    }
    }

    if( handled )
        us.requestRedraw();

    return( handled );
}


void MxEventHandler::setSceneData( osg::Node* scene )
{
    _mxCore->setSceneData( scene );
}

MxCore* MxEventHandler::getMxCore()
{
    return( _mxCore.get() );
}
osg::NodeCallback* MxEventHandler::getMatrixCallback()
{
    if( !( _cameraUpdateCallback.valid() ) )
        _cameraUpdateCallback = new CameraUpdateCallback( _mxCore.get() );
    return( _cameraUpdateCallback.get() );
}


// osgwMx
}
