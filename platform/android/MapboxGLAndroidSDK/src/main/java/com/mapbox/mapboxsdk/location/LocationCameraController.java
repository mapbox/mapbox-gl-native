package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.graphics.PointF;
import android.location.Location;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.VisibleForTesting;
import android.view.MotionEvent;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Transform;

import java.util.HashSet;
import java.util.Set;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.TRANSITION_ANIMATION_DURATION_MS;

final class LocationCameraController {

  @CameraMode.Mode
  private int cameraMode;

  private final MapboxMap mapboxMap;
  private final Transform transform;
  private final OnCameraTrackingChangedListener internalCameraTrackingChangedListener;
  private LocationComponentOptions options;
  private boolean adjustFocalPoint;

  private final MoveGestureDetector moveGestureDetector;
  private final OnCameraMoveInvalidateListener onCameraMoveInvalidateListener;

  private final AndroidGesturesManager initialGesturesManager;
  private final AndroidGesturesManager internalGesturesManager;

  private boolean isTransitioning;

  LocationCameraController(
    Context context,
    MapboxMap mapboxMap,
    Transform transform,
    OnCameraTrackingChangedListener internalCameraTrackingChangedListener,
    @NonNull LocationComponentOptions options,
    OnCameraMoveInvalidateListener onCameraMoveInvalidateListener) {
    this.mapboxMap = mapboxMap;
    this.transform = transform;

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
                           Transform transform,
                           MoveGestureDetector moveGestureDetector,
                           OnCameraTrackingChangedListener internalCameraTrackingChangedListener,
                           OnCameraMoveInvalidateListener onCameraMoveInvalidateListener,
                           AndroidGesturesManager initialGesturesManager,
                           AndroidGesturesManager internalGesturesManager) {
    this.mapboxMap = mapboxMap;
    this.transform = transform;
    this.moveGestureDetector = moveGestureDetector;
    this.internalCameraTrackingChangedListener = internalCameraTrackingChangedListener;
    this.onCameraMoveInvalidateListener = onCameraMoveInvalidateListener;
    this.internalGesturesManager = internalGesturesManager;
    this.initialGesturesManager = initialGesturesManager;
  }

  void initializeOptions(LocationComponentOptions options) {
    this.options = options;
    if (options.trackingGesturesManagement()) {
      if (mapboxMap.getGesturesManager() != internalGesturesManager) {
        mapboxMap.setGesturesManager(internalGesturesManager, true, true);
      }
      adjustGesturesThresholds();
    } else if (mapboxMap.getGesturesManager() != initialGesturesManager) {
      mapboxMap.setGesturesManager(initialGesturesManager, true, true);
    }
  }

  void setCameraMode(@CameraMode.Mode int cameraMode) {
    setCameraMode(cameraMode, null, TRANSITION_ANIMATION_DURATION_MS, null, null, null, null);
  }

  void setCameraMode(@CameraMode.Mode final int cameraMode, @Nullable Location lastLocation,
                     long transitionDuration,
                     @Nullable Double zoom, @Nullable Double bearing, @Nullable Double tilt,
                     @Nullable OnLocationCameraTransitionListener internalTransitionListener) {
    if (this.cameraMode == cameraMode) {
      return;
    }

    final boolean wasTracking = isLocationTracking();
    this.cameraMode = cameraMode;

    if (cameraMode != CameraMode.NONE) {
      mapboxMap.cancelTransitions();
    }

    adjustGesturesThresholds();
    notifyCameraTrackingChangeListener(wasTracking);
    transitionToCurrentLocation(
      wasTracking, lastLocation, transitionDuration, zoom, bearing, tilt, internalTransitionListener);
  }

  /**
   * Initiates a camera animation to the current location if location tracking was engaged.
   * Notifies an internal listener when the transition's finished to invalidate animators and notify external listeners.
   */
  private void transitionToCurrentLocation(boolean wasTracking, Location lastLocation,
                                           long transitionDuration,
                                           Double zoom, Double bearing, Double tilt,
                                           final OnLocationCameraTransitionListener internalTransitionListener) {
    if (!wasTracking && isLocationTracking() && lastLocation != null) {
      isTransitioning = true;
      LatLng target = new LatLng(lastLocation);

      CameraPosition.Builder builder = new CameraPosition.Builder().target(target);
      if (zoom != null) {
        builder.zoom(zoom);
      }
      if (tilt != null) {
        builder.tilt(tilt);
      }
      if (bearing != null) {
        builder.bearing(bearing);
      } else {
        if (isLocationBearingTracking()) {
          builder.bearing(cameraMode == CameraMode.TRACKING_GPS_NORTH ? 0 : lastLocation.getBearing());
        }
      }

      CameraUpdate update = CameraUpdateFactory.newCameraPosition(builder.build());
      MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
        @Override
        public void onCancel() {
          isTransitioning = false;
          if (internalTransitionListener != null) {
            internalTransitionListener.onLocationCameraTransitionCanceled(cameraMode);
          }
        }

        @Override
        public void onFinish() {
          isTransitioning = false;
          if (internalTransitionListener != null) {
            internalTransitionListener.onLocationCameraTransitionFinished(cameraMode);
          }
        }
      };

      CameraPosition currentPosition = mapboxMap.getCameraPosition();
      if (Utils.immediateAnimation(mapboxMap.getProjection(), currentPosition.target, target)) {
        transform.moveCamera(
          mapboxMap,
          update,
          callback);
      } else {
        transform.animateCamera(
          mapboxMap,
          update,
          (int) transitionDuration,
          callback);
      }
    } else {
      if (internalTransitionListener != null) {
        internalTransitionListener.onLocationCameraTransitionFinished(cameraMode);
      }
    }
  }

  int getCameraMode() {
    return cameraMode;
  }

  private void setBearing(float bearing) {
    if (isTransitioning) {
      return;
    }

    transform.moveCamera(mapboxMap, CameraUpdateFactory.bearingTo(bearing), null);
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  private void setLatLng(@NonNull LatLng latLng) {
    if (isTransitioning) {
      return;
    }

    transform.moveCamera(mapboxMap, CameraUpdateFactory.newLatLng(latLng), null);
    onCameraMoveInvalidateListener.onInvalidateCameraMove();

    if (adjustFocalPoint) {
      PointF focalPoint = mapboxMap.getProjection().toScreenLocation(latLng);
      mapboxMap.getUiSettings().setFocalPoint(focalPoint);
      adjustFocalPoint = false;
    }
  }

  private void setZoom(float zoom) {
    if (isTransitioning) {
      return;
    }

    transform.moveCamera(mapboxMap, CameraUpdateFactory.zoomTo(zoom), null);
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  private void setTilt(float tilt) {
    if (isTransitioning) {
      return;
    }

    transform.moveCamera(mapboxMap, CameraUpdateFactory.tiltTo(tilt), null);
    onCameraMoveInvalidateListener.onInvalidateCameraMove();
  }

  private final MapboxAnimator.AnimationsValueChangeListener<LatLng> latLngValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<LatLng>() {
      @Override
      public void onNewAnimationValue(LatLng value) {
        setLatLng(value);
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> gpsBearingValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        boolean trackingNorth = cameraMode == CameraMode.TRACKING_GPS_NORTH
          && mapboxMap.getCameraPosition().bearing == 0;

        if (!trackingNorth) {
          setBearing(value);
        }
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> compassBearingValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        if (cameraMode == CameraMode.TRACKING_COMPASS
          || cameraMode == CameraMode.NONE_COMPASS) {
          setBearing(value);
        }
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> zoomValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        setZoom(value);
      }
    };

  private final MapboxAnimator.AnimationsValueChangeListener<Float> tiltValueListener =
    new MapboxAnimator.AnimationsValueChangeListener<Float>() {
      @Override
      public void onNewAnimationValue(Float value) {
        setTilt(value);
      }
    };

  Set<AnimatorListenerHolder> getAnimationListeners() {
    Set<AnimatorListenerHolder> holders = new HashSet<>();
    if (isLocationTracking()) {
      holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_CAMERA_LATLNG, latLngValueListener));
    }

    if (isLocationBearingTracking()) {
      holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_CAMERA_GPS_BEARING, gpsBearingValueListener));
    }

    if (isConsumingCompass()) {
      holders.add(new AnimatorListenerHolder(
        MapboxAnimator.ANIMATOR_CAMERA_COMPASS_BEARING,
        compassBearingValueListener));
    }

    holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_ZOOM, zoomValueListener));
    holders.add(new AnimatorListenerHolder(MapboxAnimator.ANIMATOR_TILT, tiltValueListener));
    return holders;
  }

  boolean isTransitioning() {
    return isTransitioning;
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

  boolean isConsumingCompass() {
    return cameraMode == CameraMode.TRACKING_COMPASS
      || cameraMode == CameraMode.NONE_COMPASS;
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

  private boolean isLocationBearingTracking() {
    return cameraMode == CameraMode.TRACKING_GPS
      || cameraMode == CameraMode.TRACKING_GPS_NORTH
      || cameraMode == CameraMode.NONE_GPS;
  }

  private void notifyCameraTrackingChangeListener(boolean wasTracking) {
    internalCameraTrackingChangedListener.onCameraTrackingChanged(cameraMode);
    if (wasTracking && !isLocationTracking()) {
      mapboxMap.getUiSettings().setFocalPoint(null);
      internalCameraTrackingChangedListener.onCameraTrackingDismissed();
    }
  }

  @NonNull
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

  @NonNull
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

  @NonNull
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
    public boolean onTouchEvent(@Nullable MotionEvent motionEvent) {
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