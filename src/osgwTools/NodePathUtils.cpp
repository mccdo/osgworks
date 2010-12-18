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

#include "osgwTools/NodePathUtils.h"
#include <osg/Node>
#include <osg/Group>
#include <osg/Notify>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>


namespace osgwTools
{


// A class to enclose a string in double quotes, preserving any double quotes that exist
// in the string. If a double quote exists in the string, this code converts it to a pair
// of double quotes. For example:
//    Input:  test"string
//    Output: "test""string"
class QuotedString
{
public:
    QuotedString()
    {
    }
    QuotedString( const std::string& unquotedString )
    {
        _unquoted = unquotedString;
        _quoted = addQuotes( _unquoted );
    }

    void setQuotedString( const std::string& quotedString )
    {
        _quoted = quotedString;
        _unquoted = removeQuotes( _quoted );
    }

    std::string getUnquoted() const
    {
        return( _unquoted );
    }
    std::string getQuoted() const
    {
        return( _quoted );
    }

protected:
    std::string _quoted, _unquoted;

    static std::string addQuotes( const std::string& in )
    {
        std::string quoted( "\"" );
        std::string::size_type lastPos( 0 );
        std::string::size_type pos = in.find( '"', lastPos );
        while( pos != std::string::npos )
        {
            quoted += in.substr( lastPos, pos-lastPos+1 );
            quoted += '"';
            lastPos = pos+1;
            pos = in.find( '"', lastPos );
        }
        quoted += in.substr( lastPos, in.length()-lastPos );
        quoted += '"';
        return( quoted );
    }
    static std::string removeQuotes( const std::string& in )
    {
        std::string unquoted;
        std::string::size_type lastPos( 1 );
        std::string::size_type pos = in.find( "\"\"", lastPos );
        while( pos != std::string::npos )
        {
            unquoted += in.substr( lastPos, pos-lastPos+1 );
            lastPos = pos+2;
            pos = in.find( "\"\"", lastPos );
        }
        unquoted += in.substr( lastPos, in.length()-lastPos-1 );
        return( unquoted );
    }
};
std::ostream& operator<<( std::ostream& ostr, const QuotedString& pstr )
{
    ostr << pstr.getQuoted();
    return( ostr );
}
std::istream& operator>>( std::istream& istr, QuotedString& pstr )
{
    std::string str;
    char ch;
    bool done( false );
    unsigned int quoteCount( 0 );
    while( !done )
    {
        ch = istr.peek();
        if( ch == '"' )
            quoteCount++;
        if( ( ch != '"' ) && ( ( quoteCount & 0x1) == 0 ) )
            done = true;
        else
        {
            istr.read( &ch, 1 );
            str += ch;
        }
    }
    pstr.setQuotedString( str );
    return( istr );
}

std::ostream& operator<<( std::ostream& ostr, const NodeData& nd )
{
    ostr << nd._index << "," <<
        QuotedString( nd._className ) << "," <<
        QuotedString( nd._objectName ) << ":";
    return( ostr );
}
std::istream& operator>>( std::istream& istr, NodeData& nd )
{
    QuotedString className, objectName;
    char sep;

    istr >> nd._index >> sep >>
        className >> sep >> objectName >> sep;
    nd._className = className.getUnquoted();
    nd._objectName = objectName.getUnquoted();

    return( istr );
}


IndexedNodePath nodePathToIndexed( const osg::NodePath& nodePath )
{
    IndexedNodePath inp;
    osg::Group* parent = NULL;
    osg::NodePath::const_iterator pathIter;
    for( pathIter = nodePath.begin(); pathIter != nodePath.end(); ++pathIter )
    {
        if( parent != NULL )
        {
            NodeData nd( parent->getChildIndex( *pathIter ),
                std::string( ( *pathIter )->className() ), std::string( ( *pathIter )->getName() ) );
            inp.push_back( nd );
        }
        parent = ( *pathIter )->asGroup();
    }
    return( inp );
}
osg::NodePath indexedToNodePath( const IndexedNodePath& indexedNodePath, osg::Group* root )
{
    osg::NodePath np;
    np.push_back( root );

    osg::Group* parent = root;
    IndexedNodePath::const_iterator pathIter;
    for( pathIter = indexedNodePath.begin(); pathIter != indexedNodePath.end(); ++pathIter )
    {
        const NodeData& nd = *pathIter;
        osg::Node* child = parent->getChild( nd._index );
        np.push_back( child );
        parent = child->asGroup();
    }
    return( np );
}


std::string indexedToString( const IndexedNodePath& indexedNodePath )
{
    std::ostringstream ostr;

    IndexedNodePath::const_iterator pathIter;
    for( pathIter = indexedNodePath.begin(); pathIter != indexedNodePath.end(); ++pathIter )
    {
        const NodeData& nd = *pathIter;
        ostr << nd;
    }
    return( ostr.str() );
}
IndexedNodePath stringToIndexed( const std::string& stringPath )
{
    IndexedNodePath inp;
    std::istringstream istr( stringPath );
    while( istr.good() )
    {
        NodeData nd;
        istr >> nd;
        inp.push_back( nd );
    }
    return( inp );
}


std::string nodePathToString( const osg::NodePath& nodePath )
{
    return( indexedToString( nodePathToIndexed( nodePath ) ) );
}
osg::NodePath stringToNodePath( const std::string& stringPath, osg::Group* root )
{
    return( indexedToNodePath( stringToIndexed( stringPath ), root ) );
}


osg::Node* findNode( const IndexedNodePath& indexedNodePath, osg::Group* root )
{
    osg::NodePath np = indexedToNodePath( indexedNodePath, root );
    return( np.back() );
}
osg::Node* findNode( const std::string& stringPath, osg::Group* root )
{
    return( findNode( stringToIndexed( stringPath ), root ) );
}


NodeData::NodeData()
{
    _index = 0;
}
NodeData::NodeData( unsigned int index, std::string& className, std::string& objectName )
  : _index( index ),
    _className( className ),
    _objectName( objectName )
{
}



#ifdef OSGWORKS_BUILD_TESTS

int testQuoting( const std::string& input, const std::string& expectedResult )
{
    QuotedString qs( input );
    if( qs.getQuoted() != expectedResult )
    {
        osg::notify( osg::FATAL ) << "QuotedString adding quotes: test failure." << std::endl;
        osg::notify( osg::FATAL ) << "  Expected: " << expectedResult << std::endl;
        osg::notify( osg::FATAL ) << "  Received: " << qs.getQuoted() << std::endl;
        return( 1 );
    }
    return( 0 );
}
int testUnquoting( const std::string& input, const std::string& expectedResult )
{
    QuotedString qs;
    qs.setQuotedString( input );
    if( qs.getUnquoted() != expectedResult )
    {
        osg::notify( osg::FATAL ) << "QuotedString removing quotes: test failure." << std::endl;
        osg::notify( osg::FATAL ) << "  Expected: " << expectedResult << std::endl;
        osg::notify( osg::FATAL ) << "  Received: " << qs.getUnquoted() << std::endl;
        return( 1 );
    }
    return( 0 );
}
int testQuotedOStream( const std::string& input, const std::string& expectedResult )
{
    QuotedString qs( input );
    std::ostringstream ostr;
    ostr << qs;
    if( ostr.str() != expectedResult )
    {
        osg::notify( osg::FATAL ) << "QuotedString writing to ostream: test failure." << std::endl;
        osg::notify( osg::FATAL ) << "  Expected: " << expectedResult << std::endl;
        osg::notify( osg::FATAL ) << "  Received: " << ostr.str() << std::endl;
        return( 1 );
    }
    return( 0 );
}
int testQuotedIStream( const std::string& input, const std::string& expectedResult )
{
    std::istringstream istr( input );
    QuotedString qs;
    istr >> qs;
    if( qs.getUnquoted() != expectedResult )
    {
        osg::notify( osg::FATAL ) << "QuotedString reading from istream: test failure." << std::endl;
        osg::notify( osg::FATAL ) << "  Expected: " << expectedResult << std::endl;
        osg::notify( osg::FATAL ) << "  Received: " << qs.getUnquoted() << std::endl;
        return( 1 );
    }
    return( 0 );
}


int testNodePathUtils()
{
    // tests for the QuotedString class
    {
        std::string input( "abc" );
        std::string expectedResult( "\"abc\"" );
        if( testQuoting( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "foo\"bar" );
        std::string expectedResult( "\"foo\"\"bar\"" );
        if( testQuoting( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "data type: \"unknown\"" );
        std::string expectedResult( "\"data type: \"\"unknown\"\"\"" );
        if( testQuoting( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "\"initial quote" );
        std::string expectedResult( "\"\"\"initial quote\"" );
        if( testQuoting( input, expectedResult ) == 1 )
            return( 1 );
    }

    {
        std::string input( "\"abc\"" );
        std::string expectedResult( "abc" );
        if( testUnquoting( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "\"foo\"\"bar\"" );
        std::string expectedResult( "foo\"bar" );
        if( testUnquoting( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "\"data type: \"\"unknown\"\"\"" );
        std::string expectedResult( "data type: \"unknown\"" );
        if( testUnquoting( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "\"\"\"initial quote\"" );
        std::string expectedResult( "\"initial quote" );
        if( testUnquoting( input, expectedResult ) == 1 )
            return( 1 );
    }

    {
        std::string input( "abc" );
        std::string expectedResult( "\"abc\"" );
        if( testQuotedOStream( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "data type: \"unknown\"" );
        std::string expectedResult( "\"data type: \"\"unknown\"\"\"" );
        if( testQuotedOStream( input, expectedResult ) == 1 )
            return( 1 );
    }

    {
        std::string input( "\"abc\"," );
        std::string expectedResult( "abc" );
        if( testQuotedIStream( input, expectedResult ) == 1 )
            return( 1 );
    }
    {
        std::string input( "\"\"\"initial quote\"" );
        std::string expectedResult( "\"initial quote" );
        if( testQuotedIStream( input, expectedResult ) == 1 )
            return( 1 );
    }

    return( 0 );
}

#endif


// osgwTools
}
