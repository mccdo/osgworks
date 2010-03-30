/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009 by Kenneth Mark Bryden
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

#ifndef __OSGWTOOLS_OSGDB_SKELETON__
#define __OSGWTOOLS_OSGDB_SKELETON__ 1


#include <osgDB/ReaderWriter>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>


class ReaderWriterSkeleton : public osgDB::ReaderWriter
{
public:
    ReaderWriterSkeleton();
    ~ReaderWriterSkeleton();

    const char* className() const;

    virtual osgDB::ReaderWriter::ReadResult readNode( const std::string& fileName, const Options* options=NULL ) const;
    virtual osgDB::ReaderWriter::WriteResult writeNode( const osg::Node& node, const std::string& fileName, const Options* options=NULL ) const;

protected:
};


// __OSGWTOOLS_OSGDB_SKELETON__
#endif
