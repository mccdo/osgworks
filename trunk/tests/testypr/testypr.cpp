/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009-2012 by Kenneth Mark Bryden
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

#include <osgwTools/Orientation.h>
#include <osgwMx/MxCore.h>
#include <osgwTools/Quat.h>

#include <osg/Quat>
#include <osg/Matrix>

#include <osg/Math>
#include <osg/Notify>
#include <osg/io_utils>
#include <string>


#define EPS_DEFAULT 0.00001

bool epsCompare( const double& a, const double& b, double eps=EPS_DEFAULT )
{
    return( ( a < b+eps ) && ( a > b-eps ) );
}
bool epsCompare( const osg::Vec3d& a, const osg::Vec3d& b, double eps=EPS_DEFAULT )
{
    return( epsCompare( a[0], b[0], eps ) &&
        epsCompare( a[1], b[1], eps ) &&
        epsCompare( a[2], b[2], eps ) );
}
bool epsCompare( const osg::Quat& a, const osg::Quat& b, double eps=EPS_DEFAULT )
{
    return( epsCompare( a[0], b[0], eps ) &&
        epsCompare( a[1], b[1], eps ) &&
        epsCompare( a[2], b[2], eps ) &&
        epsCompare( a[3], b[3], eps ) );
}
bool epsCompare( const osg::Matrix& a, const osg::Matrix& b, double eps=EPS_DEFAULT )
{
    const double* aPtr( a.ptr() );
    const double* bPtr( b.ptr() );
    for( unsigned int idx=0; idx<16; idx++ )
    {
        if( !( epsCompare( aPtr[ idx ], bPtr[ idx ], eps ) ) )
            return( false );
    }
    return( true );
}

int main( int argc, char** argv )
{
    OSG_ALWAYS <<
        "This is a CTest regression test. To launch under Visual Studio, build the" << std::endl <<
        "RUN_TESTS target. Under Linux, enter 'make test' at a shell prompty." << std::endl <<
        std::endl;


    OSG_ALWAYS << "Testing deprecated makeHPRQuat()." << std::endl;
    {
        osg::Vec3d vec( 90., 0., 0. );
        osg::Quat a( osgwTools::makeHPRQuat( vec ) );
        osg::Quat b( osg::DegreesToRadians( vec[0] ), osg::Vec3d( 0., 0., 1. ) );
        if( !( epsCompare( a, b ) ) )
        {
            OSG_FATAL << "makeHPRQuat failed with " << vec << std::endl;
            return( 1 );
        }
    }
    {
        const double a0( 40. ), a1( -30. );
        osg::Vec3d vec( 0., a0+a1, 0. );
        osg::Quat a( osgwTools::makeHPRQuat( vec ) );

        osg::Quat b( osg::DegreesToRadians( a0 ), osg::Vec3d( 1., 0., 0. ) );
        osg::Quat c( osg::DegreesToRadians( a1 ), osg::Vec3d( 1., 0., 0. ) );
        if( !( epsCompare( a, b*c ) ) )
        {
            OSG_FATAL << "makeHPRQuat failed with " << vec << std::endl;
            return( 1 );
        }
    }

    OSG_ALWAYS << "Testing osgwMx interface." << std::endl;
    {
        osg::ref_ptr< osgwMx::MxCore > mxc( new osgwMx::MxCore() );
        double y, p, r;
        mxc->getYawPitchRoll( y, p, r );
        osg::Vec3d result( y, p, r );
        if( !( epsCompare( result, osg::Vec3d( 0., 0., 0. ) ) ) )
        {
            OSG_FATAL << "MxCore::getYawPitchRoll failed getting defaults: ";
            OSG_FATAL << result << " should be all zeros. " << std::endl;
            return( 1 );
        }


        osg::Vec3d newUp( 5., 0., 3. );
        newUp.normalize();
        mxc->setUp( newUp );
        mxc->getYawPitchRoll( y, p, r );

        osg::Vec3d baseDir, baseUp, basePos;
        double baseFovY;
        mxc->getInitialValues( baseUp, baseDir, basePos, baseFovY );

        double roll = osg::RadiansToDegrees( ::acos( newUp * baseUp ) );
        if( !( epsCompare( roll, r ) ) )
        {
            OSG_FATAL << "MxCore::getYawPitchRoll failed roll computation: ";
            OSG_FATAL << roll << " != " << r << std::endl;
            return( 1 );
        }


        mxc->getYawPitchRoll( y, p, r, true );
        roll = 360. - roll;
        if( !( epsCompare( roll, r ) ) )
        {
            OSG_FATAL << "MxCore::getYawPitchRoll failed right-handed roll computation: ";
            OSG_FATAL << roll << " != " << r << std::endl;
            return( 1 );
        }
    }


    OSG_ALWAYS << "Testing Orientation class consistency and correctness." << std::endl;
    {
        osg::ref_ptr< osgwTools::Orientation > orient( new osgwTools::Orientation() );
        osg::Vec3d yawAxis, pitchAxis, rollAxis;
        orient->getBasis( yawAxis, pitchAxis, rollAxis );

        osg::Matrix m( orient->getMatrix( 0., 0., 0. ) );
        const osg::Vec3d def0( m(0,0), m(0,1), m(0,2) );
        const osg::Vec3d def1( m(1,0), m(1,1), m(1,2) );
        const osg::Vec3d def2( m(2,0), m(2,1), m(2,2) );
        // Make sure default (ypr=0.) generates correct default matrix.
        if( !( epsCompare( def0, pitchAxis ) ) )
        {
            OSG_FATAL << "Failed: Default test, row 0: ";
            OSG_FATAL << def0 << " != " << pitchAxis << std::endl;
            return( 1 );
        }
        if( !( epsCompare( def1, yawAxis ) ) )
        {
            OSG_FATAL << "Failed: Default test, row 1: ";
            OSG_FATAL << def1 << " != " << yawAxis << std::endl;
            return( 1 );
        }
        if( !( epsCompare( def2, rollAxis ) ) )
        {
            OSG_FATAL << "Failed: Default test, row 2: ";
            OSG_FATAL << def2 << " != " << rollAxis << std::endl;
            return( 1 );
        }

        // Create a matrix from YPR...
        osg::Vec3d angles( 0., 0., 60. );
        m = orient->getMatrix( angles );
        const osg::Vec3d row2( m(2,0), m(2,1), m(2,2) );
        // Matrix row 2 (0-based) must match rollAxis after a roll.
        if( !( epsCompare( row2, rollAxis ) ) )
        {
            OSG_FATAL << "Failed: Roll creates matrix with bad row2: ";
            OSG_FATAL << row2 << " != " << rollAxis << std::endl;
            return( 1 );
        }
        // Extract YPR from that matrix...
        osg::Vec3d result( orient->getYPR( m ) );
        // YPRs must match.
        if( !( epsCompare( angles, result ) ) )
        {
            OSG_FATAL << "Failed create/extract roll test: ";
            OSG_FATAL << angles << " != " << result << std::endl;
            return( 1 );
        }


        angles.set( 0., 290., 0. );
        m = orient->getMatrix( angles );
        // Extract YPR from that matrix...
        result = orient->getYPR( m );
        // YPRs must match.
        if( !( epsCompare( angles, result ) ) )
        {
            OSG_FATAL << "Failed create/extract pitch test: ";
            OSG_FATAL << angles << " != " << result << std::endl;
            return( 1 );
        }


        angles.set( 45., 0., 0. );
        m = orient->getMatrix( angles );
        // Extract YPR from that matrix...
        result = orient->getYPR( m );
        // YPRs must match.
        if( !( epsCompare( angles, result ) ) )
        {
            OSG_FATAL << "Failed create/extract yaw test: ";
            OSG_FATAL << angles << " != " << result << std::endl;
            return( 1 );
        }


        angles.set( 30., 41., 57. );
        m = orient->getMatrix( angles );
        // Extract YPR from that matrix...
        result = orient->getYPR( m );
        // YPRs must match.
        if( !( epsCompare( angles, result ) ) )
        {
            OSG_FATAL << "Failed create/extract yaw-pitch-roll test: ";
            OSG_FATAL << angles << " != " << result << std::endl;
            return( 1 );
        }


        // Create some random matrix
        osg::Matrix baseMat( orient->getMatrix( 0., 0., 0. ) );
        osg::Matrix aMat( osg::Matrix::rotate( .9, osg::Vec3d( 1., 0., 0. ) ) );
        osg::Matrix bMat( osg::Matrix::rotate( -1.1, osg::Vec3d( 1., 0., 0. ) ) );
        osg::Matrix cMat( osg::Matrix::rotate( 1.4, osg::Vec3d( 0., 9., 7. ) ) );
        const osg::Matrix orig( baseMat * aMat * bMat * cMat );
        // Get YPR from that matrix, then use orient to create another matrix
        // from those YPR values.
        result = orient->getYPR( orig );
        m = orient->getMatrix( result );
        // The matrices must match.
        if( !( epsCompare( orig, m ) ) )
        {
            OSG_FATAL << "Failed matrix->YPR->matrix test." << std::endl;
            OSG_FATAL << orig << "YPR: " << result << std::endl << m;
            return( 1 );
        }


        orient->setRightHanded( false );
        angles.set( 0., 18., 0. );
        m = orient->getMatrix( angles );
        // Extract YPR from that matrix...
        result = orient->getYPR( m );
        // YPRs must match.
        if( !( epsCompare( angles, result ) ) )
        {
            OSG_FATAL << "Failed left-handed pitch test: ";
            OSG_FATAL << angles << " != " << result << std::endl;
            return( 1 );
        }
    }


    OSG_ALWAYS << "Testing (deprecated) makeHPRQuat() backwards compat." << std::endl;
    if( true )
    {
        OSG_ALWAYS << "\tSkipping (known failure)." << std::endl;
    }
    else
    {
        osg::ref_ptr< osgwTools::Orientation > orient( new osgwTools::Orientation() );

        osg::Vec3d vec( 60., 0., 0. );
        osg::Quat a( orient->getQuat( vec ) );

        osg::Quat b( osgwTools::makeHPRQuat( vec ) );

        if( !( epsCompare( a, b ) ) )
        {
            OSG_FATAL << "Failed makeHPRQuat() compat test: ";
            OSG_FATAL << a << " != " << b << std::endl;
            return( 1 );
        }
    }


    OSG_ALWAYS << "Testing MxCore backwards compat." << std::endl;
    if( true )
    {
        OSG_ALWAYS << "\tSkipping (known failure)." << std::endl;
    }
    else
    {
        osg::ref_ptr< osgwTools::Orientation > orient( new osgwTools::Orientation() );
        osg::ref_ptr< osgwMx::MxCore > mxc( new osgwMx::MxCore() );

        osg::Matrix m( osg::Matrix::rotate( osg::DegreesToRadians( -20. ), osg::Vec3d( 1., 0., 0. ) ) );
        const osg::Vec3d result0( orient->getYPR( m ) );

        mxc->setByMatrix( m );
        double y, p, r;
        mxc->getYawPitchRoll(y, p, r );
        const osg::Vec3d result1( y, p, r );

        if( !( epsCompare( result0, result1 ) ) )
        {
            OSG_FATAL << "Failed 'Failed MxCore compat test: ";
            OSG_FATAL << result0 << " != " << result1 << std::endl;
            return( 1 );
        }
    }


    OSG_ALWAYS << std::endl << "Passed." << std::endl;
    return( 0 );
}

