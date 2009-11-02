@echo on

geometryop %1 dectest00.osg
geometryop %1 dectest01.osg
geometryop %1 dectest02.osg

geometryop %1 dectest10.osg
geometryop %1 dectest11.osg
geometryop %1 dectest12.osg

rem degenerate triangle
geometryop %1 dectest13.osg
rem redundant triangle
geometryop %1 dectest14.osg

rem solidworks door and knob
geometryop %1 dectest20.ive
rem knob only
geometryop %1 dectest21.osg
rem simple case demonstrating an issue with ReducerOp
geometryop %1 dectest22.osg

rem test grouping ability, don't reduce past groups of nearly coplanar tris.
geometryop %1 dectest30.osg
geometryop %1 dectest31.osg
geometryop %1 dectest32.osg

geometryop %1 pliers-big.osg
