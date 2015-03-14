

# v2.1 #
**Status:** Ongoing development on svn trunk.

This release will introduce the new osgwMx library containing support for matrix and view manipulation, a game pad device (via DirectInput), and a space ball device (via VRPN).


# v2.0 #

**Released 26 Oct 2011**

This release is not backwards-compatible with osgWorks v1.xx-based applications.

  * Compatibility with osgBullet v2.0.

  * Deprecated functionality, such as the **Grid** class, has been removed.

# v1.1 #

**Released 11 Sept 2010**

  * **Shapes** - This release adds several convenience routines for creating basic shapes such as boxes, circles, planes, spheres, and arrows. These are intended to be osg::Geometry-based alternatives to using osg::ShapeDrawable. ShapeDrawable preserves meta data about the shape, but the **osgWorks** variants provide direct access to vertex data and are more likely to be supported by OSG plugins and third party OSG-based libraries. Shapes support for drawing planes deprecates the Grid class.

  * **RemoveLOD** - A visitor to disable LOD nodes by rendering only the highest LOD child.

  * **Build fixes** - This release contains build fixes for a number of platforms.

# v1.0 #

**Released 4 Nov 2009**