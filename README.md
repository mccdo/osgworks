# osgworks
Automatically exported from code.google.com/p/osgworks

osgWorks
What is osgWorks?
osgWorks is a set of applications and software tools that add capability for OpenSceneGraph (OSG) software developers. osgWorks is open source and is available under the GNU LGPL v2.1 software license for all to use.

Who should use osgWorks?
OSG application developers that do not want to be dependenant on a specific version of OSG and who do not want to be tied to the OSG release schedule. All of the osgWorks utilities are tested against a broad range of OSG versions.

Who should not use osgWorks?
OSG application developers that do not want to deal with dependencies for their OSG applications.

Documentation
Online Doxygen documentation.

Building, Running, and More
Lots of additional information is available at the wiki.

Features
Applications
osgwbvv: A bounding volume visualizer to display bounding spheres and boxes.

osgwcomp: A scene graph comparison utility to compare the hierarchical structure of two scene graphs and display them for side-by-side comparison.

osgwnames: Dumps a scene graph to standard output, showing the node names, node class names, and hierarchical structure.

osgwwhich: A data and library file locator that uses the OSG data and library file search paths to locate data files, OSG libraries, and OSG plugins, and display their absolute paths.

Libraries
osgwTools: Includes the following features:

an AbsoluteModelTransform node to ignore parent model transforms
GeometryModifier infrastructure for performing operations on osg::Geometry objects, including several GeometryOperation geometry reduction operations
a RefID string-based reference-counted ID object
a screen capture utility that writes image files in a separate thread
utility functions...
functions to transform osg::BoundingSphere and osg::BoundingBox classes
functions to create HPR osg::Quat objects
osg::Geometry-based utility functions to render rudimentary shapes such as planes, boxes, spheres, and arrows.
functions for node insertion and removal
visitors...
FindNamedNode, to look for nodes with specific names
ParallelVisitor, to compare two scene graphs
RemoveData, to strip data from a scene graph, leaving only the hierarchical structure
RemoveLOD, to disable LOD nodes
osgwControls: Includes a 3D slider control to allow interactive manipulation of a stored value.

Plugins
osgdb_osgwTools: Dot OSG file format support for the nodes and objects in the osgwTools library.

osgdb_skeleton: A pseudowriter that runs the RemoveData visitor on the scene graph prior to output.

Tests
The project contains several test programs to ensure correct functionality.

Support
For support requests beyond issuing bugs through google code please contact Skew Matrix Software LLC.

Contribute
All contributions are welcome and will be considered for inclusion in the project. Please contribute any enhancements or bug fixes by opening an issue and use the Contribution from user issue template. Create and attach a compressed patch file containing your changes.
