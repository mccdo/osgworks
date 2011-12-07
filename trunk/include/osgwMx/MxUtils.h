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

#ifndef __OSGWMX_MX_UTILS_H__
#define __OSGWMX_MX_UTILS_H__ 1


#include <osgwMx/Export.h>
#include <osgwMx/MxCore.h>
#include <osg/BoundingSphere>
#include <osg/Vec2d>


namespace osgwMx
{


/** \defgroup mxutils Utilities for MxCore-based device interface classes.
*/
/*@{*/


/** Compute a field of view appropriate for rendering the entire bounding
sphere \c bs when viewed from the specified \c distance.
\return Field of view in radians.
*/
double OSGWMX_EXPORT computeInitialFOVYFromDistance( const osg::BoundingSphere& bs, const double distance );
/** Compute a viewing distance appropriate for viewing the entire bounding
sphere \c bs with a perspective projection that uses the specified \c fovy.
\param fovy Field of view in degrees.
*/
double OSGWMX_EXPORT computeInitialDistanceFromFOVY( const osg::BoundingSphere& bs, const double fovy );

osg::Vec2d OSGWMX_EXPORT computeOptimalNearFar( const osg::Vec3d& position,
    const osg::BoundingSphere& bs, bool ortho );

osg::Vec4d OSGWMX_EXPORT computePanPlane( osg::Node* scene, const osgwMx::MxCore* mxCore,
    const double ndcX, const double ndcY );
osg::Vec3d OSGWMX_EXPORT pan( const osg::Node* scene, const osgwMx::MxCore* mxCore,
    const osg::Vec4d panPlane, const double ndcX, const double ndcY );

bool OSGWMX_EXPORT intersect( osg::Vec3d& result, const osg::Vec3d& farPoint,
    osg::Node* scene, const osgwMx::MxCore* mxCore );

osg::Vec3d OSGWMX_EXPORT pickCenter( osg::Node* scene, const osgwMx::MxCore* mxCore,
    const double ndcX, const double ndcY );

/** Intersect a ray with a plane. OSG doesn't appear to have this utility. */
bool OSGWMX_EXPORT intersectRayPlane( osg::Vec3d& result, const osg::Vec4d& plane, const osg::Vec3d& p0, const osg::Vec3d& p1 );

/** \brief Computes an angle and axis of rotation for a simulated trackball.

The returned \c axis and \c angle values are intended to be passed to the MxCore::rotate() function.

\param start The xy start position in normalized (-1 to 1 ) windows coordinates.
\param delta The delta xy morion in normalized (-1 to 1 ) windows coordinates.
\param orientMat The current view orientation matrix.
\param sensitivity Controls simulated trackball sensitivity. Smaller values produce less
of a rool effect, while larger values produce a more pronounced roll effect. Valid range
is 0.0 to pi/2. Default is 1.3.
\return angle The computed rotation angle in radians.
\return axis The axis to rotate about.
*/
void OSGWMX_EXPORT computeTrackball( double& angle, osg::Vec3d& axis,
    const osg::Vec2d& start, const osg::Vec2d& delta,
    const osg::Matrix& orientMat, const double sensitivity=1.3 );


/** \class FunctionalMap MxUtils.h <osgwMx/MxUtils.h>
\brief Maps an unsigned int to a particular function.

This class is presently used for arbitrary mapping of MxGamePad buttons,
where each bit is a single bit in a 32-bit unsigned int. In this usage,
the class supports the dot OSG file format, which means a FunctionalMap
can be read from or written to a .osg file as an osg::Object, thus providing
"config file" support for game pads.

The class can be used to map arbitrary unsigned int values (not bit values),
but currently this usage is not supported with the dot OSG file format.

The MxGamePad base class owns an instance of FunctionalMap, which can be
shared amongst game pad instances. The copy constructor is fully supported.

Applications can set the function mapping in two ways:
\li FunctionalMap::configure(), passing in a specific unsigned int \c key value
and a FunctionalMap::FunctionType.
\li Read the FunctionalMap osg::Object from a .osg file (and set it in the game
pas with a call to MxGamePad::setFunctionalMap()).

Future work:
\li Support non-bitfield keys in dot OSG.
\li Possible changes to support devices other than game pad (such as space
ball, kbd/mouse, etc).
*/
class OSGWMX_EXPORT FunctionalMap : public osg::Object
{
public:
    FunctionalMap();
    FunctionalMap( const FunctionalMap& rhs, osg::CopyOp copyop=osg::CopyOp::SHALLOW_COPY );
    // Note that lib name="osgwTools" is currently required because we only have one dot OSG plugin.
    META_Object(osgwTools,FunctionalMap);

    /** Various game pad functions. */
    typedef enum {
        /** Set the view direction so it's level with the ground plane, defined
        by the initial up vector. */
        LevelView=0,

        /** Warp the position to the world coordinate origin. */
        JumpToWorldOrigin,
        /** Warp the position to the initial / home position, resetting the view
        direction and up vector. */
        JumpToHomePosition,

        /** Scale movement down (slow down). */
        MoveModifyScaleSpeedDown,
        /** Scale movement up (speed up). */
        MoveModifyScaleSpeedUp,

        /** Move in literal coordinates. */
        MoveModeLiteral,
        /** Move in eye local coordinates. */
        MoveModeLocal,
        /** Move in the ground plane (defined by the initial up vector). */
        MoveModeConstrained,
        /** Move in world coordinates. */
        MoveModeWorld,
        /** Cycle through the available move modes. This function is not yet
        implemented. */
        CycleMoveMode,

        /** \brief Movement modifier for up/down motion.
        \detail When this function is active, left stick y-axis maps to up/down
        movement. The operation of the up/down movement varies by the current move mode. */
        MoveModifyUpDown,

        /** A button interface for moving up at the gamepad left stick rate. */
        MoveUpAtRate,
        /** A button interface for moving down at the gamepad left stick rate. */
        MoveDownAtRate,

        /** Rotate from the viewpoint (turn the viewer's head). */
        RotateModeLocal,
        /** Orbit around a point. */
        RotateModeOrbit,
        /** Non-Euclidean arcball. This function is not yet implemented. */
        RotateModeArcball,
        /** Cycle through the available rotation modes. This function is not yet
        implemented. */
        CycleRotateMode,

        /** \brief Rotation modifier for barrel roll.
        \detail When this function is active, left stick x-axis maps to rotation around the
        view direction, and left stick y-axis becomes a no-op. */
        RotateModifyRoll,

        /** For unused entries in the function map. This enum should always
        be last in the list of enums; it is used in the std::vector::resize()
        call at init time. */
        NoOp
    } FunctionType;
    static std::string asString( FunctionType func );
    static FunctionType asFunctionType( const std::string& str );

    static bool validMoveMode( const FunctionType func );
    static bool validRotateMode( const FunctionType func );

    static FunctionType cycleMoveMode( const FunctionType func );
    static FunctionType cycleRotateMode( const FunctionType func );

    /** Set the function for a specific unsigned int \c key. */
    void configure( const unsigned int key, FunctionType func );
    FunctionType getConfiguration( const unsigned int key ) const;

    /** Specify that the unsigned int \c key is enabled or disabled. This is
    generally called in response to a game pad button press. */
    void set( const unsigned int key, bool enable=true );
    /** Interpret the mask as a bitfield where each bit position corresponds to
    an unsigned int bit value key, and whether that position is set or not determines
    whether the corresponding function is enabled or disabled. */
    void setFromBitmask( const unsigned int mask );

    bool isSet( const unsigned int key ) const;
    bool isSet( const FunctionType func ) const;

    /** Clear the mapping and the current state. */
    void reset();

protected:
    ~FunctionalMap();

    typedef std::map< unsigned int, FunctionType > FunctionMapType;
    FunctionMapType _map;

    std::vector< bool > _state;
};


/*@}*/


// osgwMx
}


// __OSGWMX_MX_UTILS_H__
#endif
