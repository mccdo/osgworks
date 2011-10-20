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

#ifndef __OSGWMX_MX_CORE_H__
#define __OSGWMX_MX_CORE_H__ 1


#include <osgwMx/Export.h>
#include <osg/Object>
#include <osg/Node>
#include <osg/Matrixd>
#include <cmath>


namespace osgwMx
{


/** \class MxCore MxCore.h <osgwMx/MxCore.h>
\brief A GUI-independent class for maintaining view and projection matrix parameters.
*/
class OSGWMX_EXPORT MxCore : public osg::Object
{
public:
    MxCore();
    MxCore( const MxCore& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );

    META_Object(osgwMx,MxCore);


    /** Returns a matrix suitable for orienting/positioning an object in the
    world. The object's +z axis will be aligned with the view direction, and
    the object's +y axis will be aligned with the view up vector. */
    osg::Matrixd getMatrix() const;
    /** Same as getMatrix() except without the translation. */
    osg::Matrixd getOrientationMatrix() const;
    /** Returns a matrix suitable for use as a view matrix. The view's
    position, direction, and up vector are taken from this class. */
    osg::Matrixd getInverseMatrix() const;

    /** Sets the initial values for the up, dir, and position vectors.
    \c fovy is in degrees. These values are used in the reset() and
    getYawPitchRoll() functions.

    \c up defaults to (0., 0., 1.), \c dir defaults to (0., 1., 0.),
    \c pos defaults to (0., 0., 0.), and \c fovy defaults to 30.0. */
    void setInitialValues( const osg::Vec3d& up, const osg::Vec3d& dir,
        const osg::Vec3d& pos, double fovy=30. );
    void getInitialValues( osg::Vec3d& up, osg::Vec3d& dir,
        osg::Vec3d& pos, double& fovy );

    /** Sets the view up, view dir, view position, and fovy values to their initial
    values (see setInitialValues()), and disables orthographic projection. */
    void reset();


    /** Access the current view up vector. */
    void setUp( const osg::Vec3d& up ) { _viewUp = up; }
    osg::Vec3d getUp() { return( _viewUp ); }
    const osg::Vec3d& getUp() const { return( _viewUp ); }

    /** Access the current view direction vector. */
    void setDir( const osg::Vec3d& dir ) { _viewDir = dir; }
    osg::Vec3d getDir() { return( _viewDir ); }
    const osg::Vec3d& getDir() const { return( _viewDir ); }

    /** Access the current view position). */
    void setPosition( const osg::Vec3d& newPos ) { _position = newPos; }
    osg::Vec3d getPosition() { return( _position ); }
    const osg::Vec3d& getPosition() const { return( _position ); }

    /** Convenience routine to return the cross product of \c _viewDir and \c _viewUp.
    */
    osg::Vec3d getCross() const { return( _viewDir ^ _viewUp ); }

    /** Changes the view direction (and possibly the up vector), but
    keeps the view position constant. To keep the up vector constant,
    pass the current up vector as the \c axis parameter. This function
    supports first person viewing. \c angle is in radians.
    See setRotateScale(). */
    void rotate( double angle, const osg::Vec3d& axis );
    /** Rotates the view position about a point in world coordinates. This
    function supports an orbit-type view. \c angle is in radians.
    See setRotateScale(). */
    void rotate( double angle, const osg::Vec3d& axis, const osg::Vec3d& point );

    /** Sets the rotation angle scale. The rotate() functions multiply their \c angle
    parameter by the specified \c rotateScale value before performing the rotation.
    The default rotate scale value is 1.0 (no scaling). */
    void setRotateScale( double rotateScale ) { _rotateScale = rotateScale; }
    double getRotateScale() const { return( _rotateScale ); }


    /** Move the view position by a delta amount in world coordinate space.
    See setMoveScale(). */
    void moveWorldCoords( osg::Vec3d delta );
    /** Move the view position by a delta amount in left-handed view-relative coordinates.
    \c delta[0] is movement to the right (+) or left (-), \c delta[1] is movement
    up (+) or down (-), and \c delta[2] is movement backward (+) or forward (-).
    See setMoveScale(). */
    void move( osg::Vec3d delta );

    /** Sets the movement scale. The move() and moveWorldCoords functions perform an
    element-wise multiplication between their \c delta parameter and the specified
    \c moveScale vector before performing the translatkion. The default movement
    scale vector is (1., 1., 1.) (no scaling). */
    void setMoveScale( const osg::Vec3d& moveScale ) { _moveScale = moveScale; }
    osg::Vec3d getMoveScale() { return( _moveScale ); }
    const osg::Vec3d& getMoveScale() const { return( _moveScale ); }

    /** Get current yaw/pitch/roll angles for the current view.
    Values are computed relative to the initial up and dir vectors
    (see setInitialValues() ). All return values are in degrees.
    \param yaw Heading value. 0.0 <= yaw < 360.0.
    \param pitch Elevation value. -(pi/2.0) <= pitch <= (pi/2.0).
    \param roll Twist value. 0.0 <= roll < 360.0.
    \param rightHanded Use a right-handed coordinate system to compute
    yaw and roll angles. Turning left increases the yaw angle when rightHanded=true,
    and decreases the yaw angle when rightHanded=false. Likewise, counter-clockwise
    roll increases the roll angle when rightHanded=true, but decreases the roll angle
    when rightHanded=false. */
    void getYawPitchRoll( double& yaw, double& pitch, double& roll, bool rightHanded=false ) const;


    /** Specify whether or not to use an orthographic projection.
    Specify \c true to enable orthographic mode, and false to disable
    orthographic mode (and use perspective instead). The default is
    false (perspective). */
    void setOrtho( bool ortho, const double viewDistance=1. );
    bool getOrtho() const { return( _ortho ); }

    /** Set the aspect ratio. This value is used in the computation of the projection
    matrix. */
    void setAspect( double aspect ) { _aspect = aspect; }

    /** Modify \c proj so that it uses \c _fovy for its field of view in y,
    maintaining the same aspect ratio and near and far plane values. This
    function works for both symmetrical and assymetrical view volumes. */
    void updateFovy( osg::Matrixd& proj ) const;
    /** Conpute a symmetrical projection matrix using the specified zNear and
    zFar planes. */
    osg::Matrixd computeProjection( const osg::Vec2d& nearFar ) const;

    /** Set the field of view in y (fovy) in degrees. Default is 30.0 degrees. */
    void setFovy( double fovy );
    double getFovy() const { return( _fovy ); }
    double getFovyRadians() const;

    /** Scale fovy up or down, using the scale value set with setFovyScale(). */
    void fovyScaleUp();
    void fovyScaleDown();

    /** Percentage to increase the fovy in a fovyScaleUp() call.
    For example, to increase fovy by 120% in that call, pass 1.2. Default is
    1.1 (110%). The inverse (1.0 / \c fovyScale) is used in the fovyScaleDown()
    call. */
    void setFovyScale( double fovyScale ) { _fovyScale = fovyScale; }
    double getFovyScale() const { return( _fovyScale ); }

    /** Default is to clamp fovy scaling to the range (1.0,160.0). Pass \c false
    as first paramter to disable clamping. */
    void setClampFovyScale( bool clamp, osg::Vec2d clampFovyRange=osg::Vec2d(1.0,140.0) );


protected:
    ~MxCore();
    void orthonormalize();

    osg::Vec3d _viewUp, _viewDir, _position;
    osg::Vec3d _initialUp, _initialDir, _initialPosition;

    double _rotateScale;
    osg::Vec3d _moveScale;

    bool _ortho;
    double _aspect;

    double _fovy;
    double _initialFovy;
    double _fovyScale;
    bool _clampFovyScale;
    osg::Vec2d _clampFovyRange;
    double _orthoBottom, _orthoTop;
};


// osgwMx
}


// __OSGWMX_MX_CORE_H__
#endif
