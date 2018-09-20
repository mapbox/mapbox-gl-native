package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.graphics.PointF;
import android.support.annotation.NonNull;
import android.support.annotation.VisibleForTesting;
import android.view.MotionEvent;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.maps.MapboxMap;

final class LocationCameraController implements MapboxAnimator.OnCameraAnimationsValuesChangeListener {

  @CameraMode.Mode
  private int cameraMode;

  private final MapboxMap mapboxMap;
  private final OnCameraTrackingChangedListener internalCameraTrackingChangedListener;
  private LocationComponentOptions options;
  private boolean adjustFocalPoint;

  private final MoveGestureDetector moveGestureDetector;
  private final OnCameraMoveInvalidateListener onCameraMoveInvalidateListener;

  private final AndroidGesturesManager initialGesturesManager;
  private final AndroidGesturesManager internalGesturesManager;

  LocationCameraController(
    Context context,
    MapboxMap mapboxMap,
    OnCameraTrackingChangedListener internalCameraTrackingChangedListener,
    LocationComponentOptions options,
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener) {
    this.mapboxMap = mapboxMap;

    initialGesturesManager = mapboxMap.getGesturesManager();
    internalGesturesManager = new LocationGesturesManager(context);
    moveGestureDetector = internalGesturesManager.getMoveGestureDetector();
    mapboxMap.addOnRotateListener(onRotateListener);
    mapboxMap.addOnFlingListener(onFlingListener);
    mapboxMap.addOnMoveListener(onMoveListener);

    this.internalCameraTrackingChangedListener = internalCameraTrackingChangedListener;
    this.onCameraMoveInvalidateListener = onCameraMoveInvalidateListener;
    initializeOptions(options);
  }

  // Package private for testing purposes
  LocationCameraController(MapboxMap mapboxMap,
                           MoveGestureDetector moveGestureDetector,
                           OnCameraTrackingChangedListener internalCameraTrackingChangedListener,
                           OnCameraMoveInvalidateListener onCameraMoveInvalidateListener,
                           AndroidGesturesManager initialGesturesManager,
                           AndroidGesturesManager internalGesturesManager) {
    this.mapboxMap = mapboxMap;
    this.moveGestureDetector = moveGestureDetector;
    this.internalCameraTrackingChangedListener = internalCameraTrackingChangedListener;
    this.onCameraMoveInvalidateListener = onCameraMoveInvalidateListener;
    this.internalGesturesManager = internalGesturesManager;
    this.initialGesturesManager = initialGesturesManager;
  }

  void initializeOptions(LocationComponentOptions options) {
    this.options = options;
    if (options.trackingGesturesManagement()) {
      mapboxMap.setGesturesManager(internalGesturesManager, true, true);
      adjustGesturesThresholds();
    } else {
      mapboxMap.setGesturesManager(initialGesturesManager, true, true);
    }
  }

  void setCameraMode(@CameraMode.Mode int cameraMode) {
    final boolean wasTracking = isLocationTracking();
    this.cameraMode = cameraMode;
    mapboxMap.cancelTransitions();
    adjustGesturesThresholds();
    notifyCameraTrackingChangeListener(wasTracking);
  }

  int getCameraMode() {
    return cameraMode;
  }

  private void setBearing(float bearing) {
    mapboxMap.moveCamera(CameraUpdateFactory.bearingTo(bearing));
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  private void setLatLng(LatLng latLng) {
    mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(latLng));
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  private void setZoom(float zoom) {
    mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(zoom));
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  private void setTilt(float tilt) {
    mapboxMap.moveCamera(CameraUpdateFactory.tiltTo(tilt));
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  @Override
  public void onNewLatLngValue(LatLng latLng) {
    if (cameraMode == CameraMode.TRACKING
      || cameraMode == CameraMode.TRACKING_COMPASS
      || cameraMode == CameraMode.TRACKING_GPS
      || cameraMode == CameraMode.TRACKING_GPS_NORTH) {
      setLatLng(latLng);

      if (adjustFocalPoint) {
        PointF focalPoint = mapboxMap.getProjection().toScreenLocation(latLng);
        mapboxMap.getUiSettings().setFocalPoint(focalPoint);
        adjustFocalPoint = false;
      }
    }
  }

  @Override
  public void onNewGpsBearingValue(float gpsBearing) {
    boolean trackingNorth = cameraMode == CameraMode.TRACKING_GPS_NORTH
      && mapboxMap.getCameraPosition().bearing != 0;

    if (cameraMode == CameraMode.TRACKING_GPS
      || cameraMode == CameraMode.NONE_GPS
      || trackingNorth) {
      setBearing(gpsBearing);
    }
  }

  @Override
  public void onNewCompassBearingValue(float compassBearing) {
    if (cameraMode == CameraMode.TRACKING_COMPASS
      || cameraMode == CameraMode.NONE_COMPASS) {
      setBearing(compassBearing);
    }
  }

  @Override
  public void onNewZoomValue(float zoom) {
    setZoom(zoom);
  }

  @Override
  public void onNewTiltValue(float tilt) {
    setTilt(tilt);
  }

  private void adjustGesturesThresholds() {
    if (options.trackingGesturesManagement()) {
      if (isLocationTracking()) {
        adjustFocalPoint = true;
        moveGestureDetector.setMoveThreshold(options.trackingInitialMoveThreshold());
      } else {
        moveGestureDetector.setMoveThreshold(0f);
      }
    }
  }

  private boolean isLocationTracking() {
    return cameraMode == CameraMode.TRACKING
      || cameraMode == CameraMode.TRACKING_COMPASS
      || cameraMode == CameraMode.TRACKING_GPS
      || cameraMode == CameraMode.TRACKING_GPS_NORTH;
  }

  private boolean isBearingTracking() {
    return cameraMode == CameraMode.NONE_COMPASS
      || cameraMode == CameraMode.TRACKING_COMPASS
      || cameraMode == CameraMode.NONE_GPS
      || cameraMode == CameraMode.TRACKING_GPS
      || cameraMode == CameraMode.TRACKING_GPS_NORTH;
  }

  private void notifyCameraTrackingChangeListener(boolean wasTracking) {
    internalCameraTrackingChangedListener.onCameraTrackingChanged(cameraMode);
    if (wasTracking && !isLocationTracking()) {
      mapboxMap.getUiSettings().setFocalPoint(null);
      internalCameraTrackingChangedListener.onCameraTrackingDismissed();
    }
  }

  @VisibleForTesting
  MapboxMap.OnMoveListener onMoveListener = new MapboxMap.OnMoveListener() {
    private boolean interrupt;

    @Override
    public void onMoveBegin(@NonNull MoveGestureDetector detector) {
      if (options.trackingGesturesManagement()
        && detector.getPointersCount() > 1
        && detector.getMoveThreshold() != options.trackingMultiFingerMoveThreshold()
        && isLocationTracking()) {
        detector.setMoveThreshold(options.trackingMultiFingerMoveThreshold());
        interrupt = true;
      } else {
        setCameraMode(CameraMode.NONE);
      }
    }

    @Override
    public void onMove(@NonNull MoveGestureDetector detector) {
      if (interrupt) {
        detector.interrupt();
        return;
      }

      if (isLocationTracking() || isBearingTracking()) {
        setCameraMode(CameraMode.NONE);
        detector.interrupt();
      }
    }

    @Override
    public void onMoveEnd(@NonNull MoveGestureDetector detector) {
      if (options.trackingGesturesManagement() && !interrupt && isLocationTracking()) {
        detector.setMoveThreshold(options.trackingInitialMoveThreshold());
      }
      interrupt = false;
    }
  };

  private MapboxMap.OnRotateListener onRotateListener = new MapboxMap.OnRotateListener() {
    @Override
    public void onRotateBegin(@NonNull RotateGestureDetector detector) {
      if (isBearingTracking()) {
        setCameraMode(CameraMode.NONE);
      }
    }

    @Override
    public void onRotate(@NonNull RotateGestureDetector detector) {
      // no implementation
    }

    @Override
    public void onRotateEnd(@NonNull RotateGestureDetector detector) {
      // no implementation
    }
  };

  private MapboxMap.OnFlingListener onFlingListener = new MapboxMap.OnFlingListener() {
    @Override
    public void onFling() {
      setCameraMode(CameraMode.NONE);
    }
  };

  private class LocationGesturesManager extends AndroidGesturesManager {

    LocationGesturesManager(Context context) {
      super(context);
    }

    @Override
    public boolean onTouchEvent(MotionEvent motionEvent) {
      if (motionEvent != null) {
        int action = motionEvent.getActionMasked();
        if (action == MotionEvent.ACTION_UP) {
          adjustGesturesThresholds();
        }
      }
      return super.onTouchEvent(motionEvent);
    }
  }
}