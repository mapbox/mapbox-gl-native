package com.mapbox.mapboxsdk.location;

import android.animation.Animator;
import android.location.Location;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.VisibleForTesting;
import android.util.SparseArray;
import android.view.animation.LinearInterpolator;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.ACCURACY_RADIUS_ANIMATION_DURATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.COMPASS_UPDATE_RATE_MS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.MAX_ANIMATION_DURATION_MS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.TRANSITION_ANIMATION_DURATION_MS;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_CAMERA_COMPASS_BEARING;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_CAMERA_GPS_BEARING;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_CAMERA_LATLNG;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_ACCURACY;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_COMPASS_BEARING;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_GPS_BEARING;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_LAYER_LATLNG;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_TILT;
import static com.mapbox.mapboxsdk.location.MapboxAnimator.ANIMATOR_ZOOM;
import static com.mapbox.mapboxsdk.location.Utils.immediateAnimation;

final class LocationAnimatorCoordinator {

  private static final String TAG = "Mbgl-LocationAnimatorCoordinator";

  @VisibleForTesting
  final SparseArray<MapboxAnimator> animatorArray = new SparseArray<>();

  private final Projection projection;
  private Location previousLocation;
  private float previousAccuracyRadius = -1;
  private float previousCompassBearing = -1;
  private long locationUpdateTimestamp = -1;
  private float durationMultiplier;
  private final MapboxAnimatorProvider animatorProvider;
  private final MapboxAnimatorSetProvider animatorSetProvider;
  private boolean compassAnimationEnabled;
  private boolean accuracyAnimationEnabled;

  @VisibleForTesting
  int maxAnimationFps = Integer.MAX_VALUE;

  @VisibleForTesting
  final SparseArray<MapboxAnimator.AnimationsValueChangeListener> listeners = new SparseArray<>();

  LocationAnimatorCoordinator(@NonNull Projection projection, @NonNull MapboxAnimatorSetProvider animatorSetProvider,
                              @NonNull MapboxAnimatorProvider animatorProvider) {
    this.projection = projection;
    this.animatorProvider = animatorProvider;
    this.animatorSetProvider = animatorSetProvider;
  }

  void updateAnimatorListenerHolders(@NonNull Set<AnimatorListenerHolder> listenerHolders) {
    listeners.clear();
    for (AnimatorListenerHolder holder : listenerHolders) {
      listeners.append(holder.getAnimatorType(), holder.getListener());
    }
  }

  void feedNewLocation(@NonNull Location newLocation, @NonNull CameraPosition currentCameraPosition,
                       boolean isGpsNorth) {
    if (previousLocation == null) {
      previousLocation = newLocation;
      locationUpdateTimestamp = SystemClock.elapsedRealtime() - TRANSITION_ANIMATION_DURATION_MS;
    }

    LatLng previousLayerLatLng = getPreviousLayerLatLng();
    float previousLayerBearing = getPreviousLayerGpsBearing();
    LatLng previousCameraLatLng = currentCameraPosition.target;
    float previousCameraBearing = (float) currentCameraPosition.bearing;

    LatLng targetLatLng = new LatLng(newLocation);
    float targetLayerBearing = newLocation.getBearing();
    float targetCameraBearing = newLocation.getBearing();
    targetCameraBearing = checkGpsNorth(isGpsNorth, targetCameraBearing);

    updateLayerAnimators(previousLayerLatLng, targetLatLng, previousLayerBearing, targetLayerBearing);
    updateCameraAnimators(previousCameraLatLng, previousCameraBearing, targetLatLng, targetCameraBearing);

    boolean snap = immediateAnimation(projection, previousCameraLatLng, targetLatLng)
      || immediateAnimation(projection, previousLayerLatLng, targetLatLng);

    long animationDuration = 0;
    if (!snap) {
      long previousUpdateTimeStamp = locationUpdateTimestamp;
      locationUpdateTimestamp = SystemClock.elapsedRealtime();

      if (previousUpdateTimeStamp == 0) {
        animationDuration = 0;
      } else {
        animationDuration = (long) ((locationUpdateTimestamp - previousUpdateTimeStamp) * durationMultiplier)
        /* make animation slightly longer with durationMultiplier, defaults to 1.1f */;
      }

      animationDuration = Math.min(animationDuration, MAX_ANIMATION_DURATION_MS);
    }

    playAnimators(animationDuration,
      ANIMATOR_LAYER_LATLNG,
      ANIMATOR_LAYER_GPS_BEARING,
      ANIMATOR_CAMERA_LATLNG,
      ANIMATOR_CAMERA_GPS_BEARING);

    previousLocation = newLocation;
  }

  void feedNewCompassBearing(float targetCompassBearing, @NonNull CameraPosition currentCameraPosition) {
    if (previousCompassBearing < 0) {
      previousCompassBearing = targetCompassBearing;
    }

    float previousLayerBearing = getPreviousLayerCompassBearing();
    float previousCameraBearing = (float) currentCameraPosition.bearing;

    updateCompassAnimators(targetCompassBearing, previousLayerBearing, previousCameraBearing);
    playAnimators(
      compassAnimationEnabled ? COMPASS_UPDATE_RATE_MS : 0,
      ANIMATOR_LAYER_COMPASS_BEARING,
      ANIMATOR_CAMERA_COMPASS_BEARING);

    previousCompassBearing = targetCompassBearing;
  }

  void feedNewAccuracyRadius(float targetAccuracyRadius, boolean noAnimation) {
    if (previousAccuracyRadius < 0) {
      previousAccuracyRadius = targetAccuracyRadius;
    }

    float previousAccuracyRadius = getPreviousAccuracyRadius();
    updateAccuracyAnimators(targetAccuracyRadius, previousAccuracyRadius);
    playAnimators(
      noAnimation || !accuracyAnimationEnabled ? 0 : ACCURACY_RADIUS_ANIMATION_DURATION,
      ANIMATOR_LAYER_ACCURACY);

    this.previousAccuracyRadius = targetAccuracyRadius;
  }

  void feedNewZoomLevel(double targetZoomLevel, @NonNull CameraPosition currentCameraPosition, long animationDuration,
                        @Nullable MapboxMap.CancelableCallback callback) {
    updateZoomAnimator((float) targetZoomLevel, (float) currentCameraPosition.zoom, callback);
    playAnimators(animationDuration, ANIMATOR_ZOOM);
  }

  void feedNewTilt(double targetTilt, @NonNull CameraPosition currentCameraPosition, long animationDuration,
                   @Nullable MapboxMap.CancelableCallback callback) {
    updateTiltAnimator((float) targetTilt, (float) currentCameraPosition.tilt, callback);
    playAnimators(animationDuration, ANIMATOR_TILT);
  }

  private LatLng getPreviousLayerLatLng() {
    LatLng previousLatLng;
    MapboxAnimator latLngAnimator = animatorArray.get(ANIMATOR_LAYER_LATLNG);
    if (latLngAnimator != null) {
      previousLatLng = (LatLng) latLngAnimator.getAnimatedValue();
    } else {
      previousLatLng = new LatLng(previousLocation);
    }
    return previousLatLng;
  }

  private float getPreviousLayerGpsBearing() {
    MapboxFloatAnimator animator = (MapboxFloatAnimator) animatorArray.get(ANIMATOR_LAYER_GPS_BEARING);
    float previousBearing;
    if (animator != null) {
      previousBearing = (float) animator.getAnimatedValue();
    } else {
      previousBearing = previousLocation.getBearing();
    }
    return previousBearing;
  }

  private float getPreviousLayerCompassBearing() {
    MapboxFloatAnimator animator = (MapboxFloatAnimator) animatorArray.get(ANIMATOR_LAYER_COMPASS_BEARING);

    float previousBearing;
    if (animator != null) {
      previousBearing = (float) animator.getAnimatedValue();
    } else {
      previousBearing = previousCompassBearing;
    }
    return previousBearing;
  }

  private float getPreviousAccuracyRadius() {
    MapboxAnimator animator = animatorArray.get(ANIMATOR_LAYER_ACCURACY);
    float previousRadius;
    if (animator != null) {
      previousRadius = (float) animator.getAnimatedValue();
    } else {
      previousRadius = previousAccuracyRadius;
    }
    return previousRadius;
  }

  private void updateLayerAnimators(LatLng previousLatLng, LatLng targetLatLng,
                                    float previousBearing, float targetBearing) {
    createNewLatLngAnimator(ANIMATOR_LAYER_LATLNG, previousLatLng, targetLatLng);

    // Because Location bearing values are normalized to [0, 360]
    // we need to do the same for the previous bearing value to determine the shortest path
    previousBearing = Utils.normalize(previousBearing);
    float normalizedLayerBearing = Utils.shortestRotation(targetBearing, previousBearing);
    createNewFloatAnimator(ANIMATOR_LAYER_GPS_BEARING, previousBearing, normalizedLayerBearing);
  }

  private void updateCameraAnimators(LatLng previousCameraLatLng, float previousCameraBearing,
                                     LatLng targetLatLng, float targetBearing) {
    createNewLatLngAnimator(ANIMATOR_CAMERA_LATLNG, previousCameraLatLng, targetLatLng);

    float normalizedCameraBearing = Utils.shortestRotation(targetBearing, previousCameraBearing);
    createNewFloatAnimator(ANIMATOR_CAMERA_GPS_BEARING, previousCameraBearing, normalizedCameraBearing);
  }

  private void updateCompassAnimators(float targetCompassBearing, float previousLayerBearing,
                                      float previousCameraBearing) {
    float normalizedLayerBearing = Utils.shortestRotation(targetCompassBearing, previousLayerBearing);
    createNewFloatAnimator(ANIMATOR_LAYER_COMPASS_BEARING, previousLayerBearing, normalizedLayerBearing);

    float normalizedCameraBearing = Utils.shortestRotation(targetCompassBearing, previousCameraBearing);
    createNewFloatAnimator(ANIMATOR_CAMERA_COMPASS_BEARING, previousCameraBearing, normalizedCameraBearing);
  }

  private void updateAccuracyAnimators(float targetAccuracyRadius, float previousAccuracyRadius) {
    createNewFloatAnimator(ANIMATOR_LAYER_ACCURACY, previousAccuracyRadius, targetAccuracyRadius);
  }

  private void updateZoomAnimator(float targetZoomLevel, float previousZoomLevel,
                                  @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    createNewCameraAdapterAnimator(ANIMATOR_ZOOM, previousZoomLevel, targetZoomLevel, cancelableCallback);
  }

  private void updateTiltAnimator(float targetTilt, float previousTiltLevel,
                                  @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    createNewCameraAdapterAnimator(ANIMATOR_TILT, previousTiltLevel, targetTilt, cancelableCallback);
  }

  private void createNewLatLngAnimator(@MapboxAnimator.Type int animatorType, LatLng previous, LatLng target) {
    cancelAnimator(animatorType);
    MapboxAnimator.AnimationsValueChangeListener listener = listeners.get(animatorType);
    if (listener != null) {
      animatorArray.put(animatorType, animatorProvider.latLngAnimator(previous, target, listener, maxAnimationFps));
    }
  }

  private void createNewFloatAnimator(@MapboxAnimator.Type int animatorType, float previous, float target) {
    cancelAnimator(animatorType);
    MapboxAnimator.AnimationsValueChangeListener listener = listeners.get(animatorType);
    if (listener != null) {
      animatorArray.put(animatorType, animatorProvider.floatAnimator(previous, target, listener, maxAnimationFps));
    }
  }

  private void createNewCameraAdapterAnimator(@MapboxAnimator.Type int animatorType, float previous, float target,
                                              @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    cancelAnimator(animatorType);
    MapboxAnimator.AnimationsValueChangeListener listener = listeners.get(animatorType);
    if (listener != null) {
      animatorArray.put(animatorType, animatorProvider.cameraAnimator(previous, target, listener, cancelableCallback));
    }
  }

  private float checkGpsNorth(boolean isGpsNorth, float targetCameraBearing) {
    if (isGpsNorth) {
      targetCameraBearing = 0;
    }
    return targetCameraBearing;
  }

  private void playAnimators(long duration, @MapboxAnimator.Type int... animatorTypes) {
    List<Animator> animators = new ArrayList<>();
    for (@MapboxAnimator.Type int animatorType : animatorTypes) {
      Animator animator = animatorArray.get(animatorType);
      if (animator != null) {
        animators.add(animator);
      }
    }
    animatorSetProvider.startAnimation(animators, new LinearInterpolator(), duration);
  }

  void resetAllCameraAnimations(@NonNull CameraPosition currentCameraPosition, boolean isGpsNorth) {
    resetCameraCompassAnimation(currentCameraPosition);
    boolean snap = resetCameraLocationAnimations(currentCameraPosition, isGpsNorth);
    playAnimators(
      snap ? 0 : TRANSITION_ANIMATION_DURATION_MS,
      ANIMATOR_CAMERA_LATLNG,
      ANIMATOR_CAMERA_GPS_BEARING);
  }

  private boolean resetCameraLocationAnimations(@NonNull CameraPosition currentCameraPosition, boolean isGpsNorth) {
    resetCameraGpsBearingAnimation(currentCameraPosition, isGpsNorth);
    return resetCameraLatLngAnimation(currentCameraPosition);
  }

  private boolean resetCameraLatLngAnimation(@NonNull CameraPosition currentCameraPosition) {
    MapboxLatLngAnimator animator = (MapboxLatLngAnimator) animatorArray.get(ANIMATOR_CAMERA_LATLNG);
    if (animator == null) {
      return false;
    }

    LatLng currentTarget = animator.getTarget();
    LatLng previousCameraTarget = currentCameraPosition.target;
    createNewLatLngAnimator(ANIMATOR_CAMERA_LATLNG, previousCameraTarget, currentTarget);

    return immediateAnimation(projection, previousCameraTarget, currentTarget);
  }

  private void resetCameraGpsBearingAnimation(@NonNull CameraPosition currentCameraPosition, boolean isGpsNorth) {
    MapboxFloatAnimator animator = (MapboxFloatAnimator) animatorArray.get(ANIMATOR_CAMERA_GPS_BEARING);
    if (animator == null) {
      return;
    }

    float currentTargetBearing = animator.getTarget();
    currentTargetBearing = checkGpsNorth(isGpsNorth, currentTargetBearing);
    float previousCameraBearing = (float) currentCameraPosition.bearing;
    float normalizedCameraBearing = Utils.shortestRotation(currentTargetBearing, previousCameraBearing);
    createNewFloatAnimator(ANIMATOR_CAMERA_GPS_BEARING, previousCameraBearing, normalizedCameraBearing);
  }

  private void resetCameraCompassAnimation(@NonNull CameraPosition currentCameraPosition) {
    MapboxFloatAnimator animator =
      (MapboxFloatAnimator) animatorArray.get(ANIMATOR_CAMERA_COMPASS_BEARING);
    if (animator == null) {
      return;
    }

    float currentTargetBearing = animator.getTarget();
    float previousCameraBearing = (float) currentCameraPosition.bearing;
    float normalizedCameraBearing = Utils.shortestRotation(currentTargetBearing, previousCameraBearing);
    createNewFloatAnimator(ANIMATOR_CAMERA_COMPASS_BEARING, previousCameraBearing, normalizedCameraBearing);
  }

  void resetAllLayerAnimations() {
    MapboxLatLngAnimator latLngAnimator = (MapboxLatLngAnimator) animatorArray.get(ANIMATOR_LAYER_LATLNG);
    MapboxFloatAnimator gpsBearingAnimator = (MapboxFloatAnimator) animatorArray.get(ANIMATOR_LAYER_GPS_BEARING);
    MapboxFloatAnimator compassBearingAnimator =
      (MapboxFloatAnimator) animatorArray.get(ANIMATOR_LAYER_COMPASS_BEARING);

    if (latLngAnimator != null && gpsBearingAnimator != null) {
      LatLng currentLatLng = (LatLng) latLngAnimator.getAnimatedValue();
      LatLng currentLatLngTarget = latLngAnimator.getTarget();
      createNewLatLngAnimator(ANIMATOR_LAYER_LATLNG, currentLatLng, currentLatLngTarget);

      float currentGpsBearing = (float) gpsBearingAnimator.getAnimatedValue();
      float currentGpsBearingTarget = gpsBearingAnimator.getTarget();
      createNewFloatAnimator(ANIMATOR_LAYER_GPS_BEARING, currentGpsBearing, currentGpsBearingTarget);

      long duration = latLngAnimator.getDuration() - latLngAnimator.getCurrentPlayTime();

      playAnimators(duration, ANIMATOR_LAYER_LATLNG, ANIMATOR_LAYER_GPS_BEARING);
    }

    if (compassBearingAnimator != null) {
      float currentLayerBearing = getPreviousLayerCompassBearing();
      float currentLayerBearingTarget = compassBearingAnimator.getTarget();
      createNewFloatAnimator(ANIMATOR_LAYER_COMPASS_BEARING, currentLayerBearing, currentLayerBearingTarget);
      playAnimators(
        compassAnimationEnabled ? COMPASS_UPDATE_RATE_MS : 0,
        ANIMATOR_LAYER_COMPASS_BEARING);
    }
  }

  void cancelZoomAnimation() {
    cancelAnimator(ANIMATOR_ZOOM);
  }

  void cancelTiltAnimation() {
    cancelAnimator(ANIMATOR_TILT);
  }

  void cancelAllAnimations() {
    for (int i = 0; i < animatorArray.size(); i++) {
      @MapboxAnimator.Type int animatorType = animatorArray.keyAt(i);
      cancelAnimator(animatorType);
    }
  }

  private void cancelAnimator(@MapboxAnimator.Type int animatorType) {
    MapboxAnimator animator = animatorArray.get(animatorType);
    if (animator != null) {
      animator.cancel();
      animator.removeAllUpdateListeners();
      animator.removeAllListeners();
      animatorArray.put(animatorType, null);
    }
  }

  void setTrackingAnimationDurationMultiplier(float trackingAnimationDurationMultiplier) {
    this.durationMultiplier = trackingAnimationDurationMultiplier;
  }

  void setCompassAnimationEnabled(boolean compassAnimationEnabled) {
    this.compassAnimationEnabled = compassAnimationEnabled;
  }

  void setAccuracyAnimationEnabled(boolean accuracyAnimationEnabled) {
    this.accuracyAnimationEnabled = accuracyAnimationEnabled;
  }

  void setMaxAnimationFps(int maxAnimationFps) {
    if (maxAnimationFps <= 0) {
      Logger.e(TAG, "Max animation FPS cannot be less or equal to 0.");
      return;
    }
    this.maxAnimationFps = maxAnimationFps;
  }
}
