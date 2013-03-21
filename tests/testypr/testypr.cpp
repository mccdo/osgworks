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

#include <osgwTools/Quat.h>
#include <osgwMx/MxCore.h>

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
    return( ( a[0] < b[0]+eps ) && ( a[0] > b[0]-eps ) &&
        ( a[1] < b[1]+eps ) && ( a[1] > b[1]-eps ) &&
        ( a[2] < b[2]+eps ) && ( a[2] > b[2]-eps ) );
}
bool epsCompare( const osg::Quat& a, const osg::Quat& b, double eps=EPS_DEFAULT )
{
    return( ( a[0] < b[0]+eps ) && ( a[0] > b[0]-eps ) &&
        ( a[1] < b[1]+eps ) && ( a[1] > b[1]-eps ) &&
        ( a[2] < b[2]+eps ) && ( a[2] > b[2]-eps ) &&
        ( a[3] < b[3]+eps ) && ( a[3] > b[3]-eps ) );
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
            OSG_FATAL << "MxCore::getYawPitchRoll failed getting defaults:";
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


    OSG_ALWAYS << std::endl << "Passed." << std::endl;
    return( 0 );
}

