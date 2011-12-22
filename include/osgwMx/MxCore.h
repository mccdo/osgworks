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
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
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

    /** \brief Sets the current up, dir, and position from \c m. */
    void setByMatrix( const osg::Matrix& m );
    /** \brief Same as setByMatrix(), but assume \c m is an inverse or view matrix. */
    void setByInverseMatrix( const osg::Matrix& m );
    /** \brief Sets the current up and dir (position is ignored) from \c m. */
    void setOrientationByMatrix( const osg::Matrix& m );
    /** \brief Same as setOrientationByMatrix(), but assume \c m is an inverse or view matrix. */
    void setOrientationByInverseMatrix( const osg::Matrix& m );

    /** Convenience routine to return the cross product of \c _viewDir and \c _viewUp.
    */
    osg::Vec3d getCross() const { return( _viewDir ^ _viewUp ); }

    /** Sets the current up vector to the initial up vector, with
    a minimum change to the view direction vector. If initial up
    vector and current view direction are nearly coincident, the
    view dir is set to the initial view dir. */
    void level();

    /** Change the view direction to look at the \c bs center.
    Then move the view position closer to or further from the
    \c bs center in order to fit it to the current view (based
    on \c _fovy and \c _aspect). */
    void lookAtAndFit( const osg::BoundingSphere& bs );
    /** Not yet implemented. */
    void lookAtAndFit( const osg::BoundingBox& bb );

    /** Change the view direction vector to look directly at the orbit
    center point. */
    void lookAtOrbitCenter();

    /** Changes the view direction (and possibly the up vector), but
    keeps the view position constant. To keep the up vector constant,
    pass the current up vector as the \c axis parameter. This function
    supports first person viewing. \c angle is in radians.
    See setRotateScale(). */
    void rotateLocal( double angle, const osg::Vec3d& axis );
    /** \deprecated Synonym for moveLiteral(), provided for backwards compatibility. */
    void rotate( double angle, const osg::Vec3d& axis );
    /** Rotates the view position about a point in world coordinates. This
    function supports an orbit-type view. \c angle is in radians.
    See setRotateScale(). */
    void rotateOrbit( double angle, const osg::Vec3d& axis );
    /** \deprecated Synonym for moveLiteral(), provided for backwards compatibility. */
    void rotate( double angle, const osg::Vec3d& axis, const osg::Vec3d& point );

    /** Sets the orbit center point used by rotateOrbit(). The default is
    (0,0,0), the origin. Note: The orbit center point is in world coordinates.*/
    void setOrbitCenterPoint( const osg::Vec3d& orbitCenter ) { _orbitCenter = orbitCenter; }
    osg::Vec3d getOrbitCenterPoint() const { return( _orbitCenter ); }

    /** Sets the rotation angle scale. The rotate() functions multiply their \c angle
    parameter by the specified \c rotateScale value before performing the rotation.
    The default rotate scale value is 1.0 (no scaling). */
    void setRotateScale( double rotateScale ) { _rotateScale = rotateScale; }
    double getRotateScale() const { return( _rotateScale ); }


    /** \brief Move by the scaled parameter amount.
    \detailss Scale \c delta by \c _moveScale (setMoveScale()) then add it directly
    to the current view position \c _position. This interface is used by the kbd /
    mouse MxEventHandler to effect panning tied to the mouse position. */
    void moveLiteral( const osg::Vec3d& delta );
    /** \deprecated Synonym for moveLiteral(), provided for backwards compatibility. */
    void moveWorldCoords( const osg::Vec3d& delta );

    /** \brief Move the view position in view-relative coordinates.
    \details This is an intuitive and generally useful for movement in an arbitrary
    view-centric coordinate system. See also moveConstrainted() for a variant to allow
    movement in a typical up-oriented 3D environment.

    \c delta[0] is movement to the right (+) or left (-), \c delta[1] is movement
    up (+) or down (-), and \c delta[2] is movement backward (+) or forward (-).
    See setMoveScale().
    
    Note that movement is scaled (see setMoveScale()). */
    void moveLocal( const osg::Vec3d& delta );
    /** \deprecated Synonym for moveLocal(), provided for backwards compatibility. */
    void move( const osg::Vec3d& delta );

    /** \brief Move in local coordinates constrained by the world (initial) up vector.
    \details This is probably the most useful of the move function variants, as
    it allows the user to move in a world defined by the \c _initialUp vector and
    its implied ground plane. As an example, this function allows flat horizontal
    movement even when the view is looking up or down relative to the horizon.

    Uses the initial up vector \c _initialUp to define a ground plan", and then
    moves in that environment as follows: \c delta[0] moves in the ground plane
    right (+) or left (-); \c delta[1] moves along \c _initialUp in the up (+) and
    down (-) directions; \c delta[2] moves in the ground plane backward (+) or
    forward (-).

    Note that movement is scaled (see setMoveScale()). */
    void moveConstrained( const osg::Vec3d& delta );

    /** \brief Move the view position by a delta amount in world coordinate space.
    \details "World coordinates" means the coordinate space defined with setInitialValues():
    \c _initialDir is the negative z axis, \c _initialUp is the y axis, and the x axis
    is \c _initialDir ^ \c _initialUp (cross product).
    
    Note that movement is scaled (see setMoveScale()). */
    void moveWorld( const osg::Vec3d& delta );

    /** \brief Move closer to (-) or further away from (+) the orbit center point.
    \details Movement automaticall slows as a function of distance to \c _orbitCenter,
    alower when close and faster when further away.

    Note that movement is also scaled by  setMoveScale(). */
    void moveOrbit( const float distance );


    /** Sets the movement scale. The move*() family of functions perform an
    element-wise multiplication between their \c delta parameter and the specified
    \c moveScale vector before performing the translatkion. The default movement
    scale vector is (1., 1., 1.) (no scaling). */
    void setMoveScale( const osg::Vec3d& moveScale ) { _moveScale = moveScale; }
    osg::Vec3d getMoveScale() { return( _moveScale ); }
    const osg::Vec3d& getMoveScale() const { return( _moveScale ); }

    /** Get current yaw/pitch/roll angles for the current view.
    Values are computed relative to the initial up and dir vectors
    (see setInitialValues()). All return values are in degrees.
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

    osg::Vec3d _orbitCenter;
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
