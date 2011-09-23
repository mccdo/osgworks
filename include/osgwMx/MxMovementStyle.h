// Copyright (c) 2011 Digital Transforms.

#ifndef __OSGWMX_MX_MXMOVEMENTSTYLE_H__
#define __OSGWMX_MX_MXMOVEMENTSTYLE_H__

#include <osg/Referenced>

#include <osgwMx/MxInputAdapter.h>
#include <osgwMx/MxCore.h>
#include <osgwMx/CameraUpdateCallback.h>
#include <osgwMx/Export.h>

namespace osgwMx
{

class OSGWMX_EXPORT MxMovementStyle : public osg::Referenced
   {
   public:
      MxMovementStyle(MxInputAdapter *ia);
      virtual ~MxMovementStyle() {}

      // a necessary setup step for the MxCore matrix. Hand this the root node of the
      // scene or sub-scene to be manipulated.
      void setSceneData(osg::Node *scene) {if (_mxCore.valid()) _mxCore->setSceneData(scene);}

      // read the input adapter data, polling if necessary, and perform
      // any necessary transformations on the matrix.
      // the timestamp (ts) passed in is the ellapsed time in seconds and partial
      // seconds. The time start is irrelevant for this purpose. It is used only
      // to track the passage of time and manipulate the matrix at a predictable rate.
      void updateData(double ts) {if (_inputAdapter.valid()) {_inputAdapter->updateData(); matrixTransform(ts);}}

      // this resets the matrix to its original state.
      void resetMatrix() {if (_mxCore.valid()) _mxCore->computeInitialView();}

      // this is the rate of change in seconds and partial seconds.
      // Affect any change at this rate. Smaller values are faster. Default is 1 second.
      void setChangeRate(double rate) {_chgRate = rate;}
      double getChangeRate() const {return _chgRate;}

      MxCore *getMxCore() {return _mxCore;}
      MxInputAdapter *getInputAdapter() {return _inputAdapter.get();}
      osg::NodeCallback *getMatrixCallback();

      // for reversing the movement directions.
      // NOTE: it is up to the inheriting class to implement, but
      // there are here to dictate implementation.
      // set to 'true' to reverse the normal direction.
      void setReverseRotateX(bool state) {_reverseRotateX = state;}
      bool getReverseRotateX() const {return _reverseRotateX;}
      void setReverseRotateY(bool state) {_reverseRotateY = state;}
      bool getReverseRotateY() const {return _reverseRotateY;}
      void setReverseMoveX(bool state) {_reverseMoveX = state;}
      bool getReverseMoveX() const {return _reverseMoveX;}
      void setReverseMoveY(bool state) {_reverseMoveY = state;}
      bool getReverseMoveY() const {return _reverseMoveY;}
      void setReverseZoom(bool state) {_reverseZoom = state;}
      bool getReverseZoom() const {return _reverseZoom;}

   protected:
      // inheriting class must implement this function to perform transformations
      // on the matrix based on the MxInputAdapter data.
      virtual void matrixTransform(double ts) = 0;

      osg::ref_ptr<MxCore> _mxCore;
      osg::ref_ptr<MxInputAdapter> _inputAdapter;
      osg::ref_ptr<CameraUpdateCallback> _cameraUpdateCallback;

      double _chgRate;              // change rate, in seconds and partial seconds. Affect any change at this rate. Smaller values are faster.
      double _lastRotateTime;       // the last rotation timestamp when rotating is active.
      double _lastMoveTime;         // the last move timestamp when moving is active.
      double _lastZoomTime;         // the last zoom timestamp when zooming is active.
      bool _rotating;               // true if rotating
      bool _moving;                 // true if moving
      bool _zooming;                // true if zooming
      bool _reverseRotateX;         // true if should reverse normal X rotation
      bool _reverseRotateY;         // true if should reverse normal Y rotation
      bool _reverseMoveX;
      bool _reverseMoveY;
      bool _reverseZoom;

   private:
      MxMovementStyle() {}    // don't allow instantiation without an input adapter.
   };

// osgwMx
}

#endif