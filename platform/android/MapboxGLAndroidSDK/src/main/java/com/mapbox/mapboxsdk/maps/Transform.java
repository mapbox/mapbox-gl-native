package com.mapbox.mapboxsdk.maps;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.animation.TimeAnimator;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.graphics.PointF;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.view.animation.LinearInterpolator;

import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;
import com.mapbox.mapboxsdk.utils.AnimatorUtils;
import com.mapbox.services.android.telemetry.utils.MathUtils;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.maps.MapView.REGION_DID_CHANGE_ANIMATED;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;

/**
 * Resembles the current Map transformation.
 * <p>
 * Responsible for synchronising {@link CameraPosition} state and notifying
 * {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraChangeListener}.
 * </p>
 */
final class Transform implements MapView.OnMapChangedListener {

  private final NativeMapView mapView;
  private final MarkerViewManager markerViewManager;
  private final TrackingSettings trackingSettings;
  private final MyLocationView myLocationView;

  private CameraPosition cameraPosition;
  private MapboxMap.CancelableCallback cameraCancelableCallback;

  private MapboxMap.OnCameraChangeListener onCameraChangeListener;

  private CameraChangeDispatcher cameraChangeDispatcher;

  Transform(NativeMapView mapView, MarkerViewManager markerViewManager, TrackingSettings trackingSettings,
            CameraChangeDispatcher cameraChangeDispatcher) {
    this.mapView = mapView;
    this.markerViewManager = markerViewManager;
    this.trackingSettings = trackingSettings;
    this.myLocationView = trackingSettings.getMyLocationView();
    this.cameraChangeDispatcher = cameraChangeDispatcher;
  }

  void initialise(@NonNull MapboxMap mapboxMap, @NonNull MapboxMapOptions options) {
    CameraPosition position = options.getCamera();
    if (position != null && !position.equals(CameraPosition.DEFAULT)) {
      moveCamera(mapboxMap, CameraUpdateFactory.newCameraPosition(position), null);
    }
    setMinZoom(options.getMinZoomPreference());
    setMaxZoom(options.getMaxZoomPreference());
  }

  //
  // Camera API
  //

  @UiThread
  public final CameraPosition getCameraPosition() {
    if (cameraPosition == null) {
      cameraPosition = invalidateCameraPosition();
    }
    return cameraPosition;
  }

  @UiThread
  void updateCameraPosition(@NonNull CameraPosition position) {
    if (myLocationView != null) {
      myLocationView.setCameraPosition(position);
    }
    markerViewManager.setTilt((float) position.tilt);
  }

  @Override
  public void onMapChanged(@MapView.MapChange int change) {
    if (change == REGION_DID_CHANGE_ANIMATED) {
      updateCameraPosition(invalidateCameraPosition());
      if (cameraCancelableCallback != null) {
        cameraCancelableCallback.onFinish();
        cameraCancelableCallback = null;
      }
      cameraChangeDispatcher.onCameraIdle();
      mapView.removeOnMapChangedListener(this);
    }
  }

  @UiThread
  final void moveCamera(MapboxMap mapboxMap, CameraUpdate update, MapboxMap.CancelableCallback callback) {
    CameraPosition cameraPosition = update.getCameraPosition(mapboxMap);
    if (isValidCameraPosition(cameraPosition)) {
      trackingSettings.resetTrackingModesIfRequired(this.cameraPosition, cameraPosition, false);
      cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(OnCameraMoveStartedListener.REASON_API_ANIMATION);
      mapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
      if (callback != null) {
        callback.onFinish();
      }
      cameraChangeDispatcher.onCameraIdle();
    }
  }

  @UiThread
  final void easeCamera(MapboxMap mapboxMap, CameraUpdate update, int durationMs, boolean easingInterpolator,
                        final MapboxMap.CancelableCallback callback, boolean isDismissable) {
    CameraPosition cameraPosition = update.getCameraPosition(mapboxMap);
    if (isValidCameraPosition(cameraPosition)) {
      trackingSettings.resetTrackingModesIfRequired(this.cameraPosition, cameraPosition, isDismissable);
      cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(OnCameraMoveStartedListener.REASON_API_ANIMATION);

      if (callback != null) {
        cameraCancelableCallback = callback;
      }
      mapView.addOnMapChangedListener(this);
      mapView.easeTo(cameraPosition.bearing, cameraPosition.target, durationMs, cameraPosition.tilt,
        cameraPosition.zoom, easingInterpolator);
    }
  }

  @UiThread
  @SuppressLint("NewApi")
  final void animateCamera(MapboxMap mapboxMap, CameraUpdate update, final int durationMs,
                           final MapboxMap.CancelableCallback callback) {

    // TODO
    // correct rotation value
    // correct fixed 512 value for pixel points
    // integrate callback & MapboxMap#cancelTransitions
    // integrate padding (aka edge insets)
    // integrate unwrap for shortest path
    // handle edge cases for NaN and infinite values
    // reimplement ValueAnimator with an actual value
    // handle not provided values of CameraPosition, -1 = not provided

    CameraPosition camera = update.getCameraPosition(mapboxMap);
    final LatLng latLng = camera.target;
    final double zoom = camera.zoom;
    final double angle = Math.toRadians(-camera.bearing);
    final double pitch = Math.toRadians(camera.tilt);

    if (Double.isNaN(zoom)) {
      return;
    }

    // Determine endpoints.
    //EdgeInsets padding = camera.padding;
    LatLng startLatLng = getLatLng();//getLatLng(padding).wrapped();
    //startLatLng.unwrapForShortestPath(latLng);

    final PointF startPoint = mapView.pixelForLatLng2(startLatLng);
    startPoint.y = 512 - startPoint.y;
    final PointF endPoint = mapView.pixelForLatLng2(latLng);
    endPoint.y = 512 - endPoint.y;

    final double startZoom = mapView.getZoom();
    final double startTilt = Math.toRadians(mapView.getPitch());
    final double startBearing = Math.toRadians(getBearing());

    // Minimize rotation by taking the shorter path around the circle.
    final double angleNormal = normalizeAngle(angle, startBearing);

    final double w0 = 512;
    final double w1 = 256;
    final double u1 = Math.hypot(endPoint.x - startPoint.x, endPoint.y - startPoint.y);
    final double rho = 1.42;
    final double rho2 = rho * rho;

    final double r0 = calculateZoomFactor(0, w1, w0, rho2, u1);
    final double r1 = calculateZoomFactor(1, w1, w0, rho2, u1);

    final boolean isClose = false;

    final double S = (isClose ? (Math.abs(Math.log(w1 / w0)) / rho) : ((r1 - r0) / rho));

    final double startScale = Math.abs(mapView.getScale());

    ValueAnimator valueAnimator = ValueAnimator.ofObject(new TypeEvaluator() {
      @Override
      public Object evaluate(float k, Object startValue, Object endValue) {
        double s = k * S;
        float us = k == 1.0 ? 1.0f : (float) u(s, isClose, w0, r0, rho, rho2, u1);

        float framePointX = interpolate(startPoint.x, endPoint.x, us);
        float framePointY = interpolate(startPoint.y, endPoint.y, us);
        PointF framePoint = new PointF(framePointX, framePointY);
        double frameZoom = startZoom + scaleZoom(1 / w(s, isClose, w1, w0, rho, r0));
        double frameTilt = interpolate(startTilt, pitch, us);
        double frameBearing = interpolate(startBearing, angleNormal, us);

        // Zoom can be NaN if size is empty.
        if (Double.isNaN(frameZoom)) {
          frameZoom = zoom;
        }

        LatLng frameLatLng = mapView.latLngForPixel(framePoint, startScale);
        mapView.setLatLng(frameLatLng);
        mapView.setZoom(frameZoom, new PointF(mapView.getWidth() / 2, mapView.getHeight() / 2));
        mapView.setBearing(Math.toDegrees(frameBearing));
        mapView.setPitch(Math.toDegrees(frameTilt));
        return new Object();
      }
    }, new Object(), new Object());
    valueAnimator.setDuration(durationMs);
    valueAnimator.start();
  }

  public static double normalizeAngle(double angle, double anchorAngle) {
    if (Double.isNaN(angle) || Double.isNaN(anchorAngle)) {
      return 0;
    }

    angle = MathUtils.wrap(angle, -Math.PI, Math.PI);
    if (angle == -Math.PI) {
      angle = Math.PI;
    }
    double diff = Math.abs(angle - anchorAngle);
    if (Math.abs(angle - Math.PI * 2 - anchorAngle) < diff) {
      angle -= Math.PI * 2;
    }
    if (Math.abs(angle + Math.PI * 2 - anchorAngle) < diff) {
      angle += Math.PI * 2;
    }

    return angle;
  }

  private double scaleZoom(double scale) {
    return log(scale, 2);
  }

  static double log(double x, int base) {
    return (Math.log(x) / Math.log(base));
  }

  private float interpolate(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
  }

  private double interpolate(double a, double b, double t) {
    return a * (1.0 - t) + b * t;
  }


  private double calculateZoomFactor(double i, double w1, double w0, double rho2, double u1) {
    double b = (w1 * w1 - w0 * w0 + (i == 1 ? -1 : 1) * rho2 * rho2 * u1 * u1) / (2 * (i == 1 ? w1 : w0) * rho2 * u1);
    return Math.log(Math.sqrt(b * b + 1) - b);
  }

  private double w(double s, boolean isClose, double w1, double w0, double rho, double r0) {
    return (isClose ? Math.exp((w1 < w0 ? -1 : 1) * rho * s)
      : (Math.cosh(r0) / Math.cosh(r0 + rho * s)));
  }

  private double u(double s, boolean isClose, double w0, double r0, double rho, double rho2, double u1) {
    return (isClose ? 0.
      : (w0 * (Math.cosh(r0) * Math.tanh(r0 + rho * s) - Math.sinh(r0)) / rho2 / u1));
  }

  private boolean isValidCameraPosition(@Nullable CameraPosition cameraPosition) {
    return cameraPosition != null && !cameraPosition.equals(this.cameraPosition);
  }

  @UiThread
  @Nullable
  CameraPosition invalidateCameraPosition() {
    if (mapView != null) {
      CameraPosition cameraPosition = mapView.getCameraPosition();
      if (this.cameraPosition != null && !this.cameraPosition.equals(cameraPosition)) {
        cameraChangeDispatcher.onCameraMove();
      }

      if (isComponentUpdateRequired(cameraPosition)) {
        updateCameraPosition(cameraPosition);
      }

      this.cameraPosition = cameraPosition;
      if (onCameraChangeListener != null) {
        onCameraChangeListener.onCameraChange(this.cameraPosition);
      }
    }
    return cameraPosition;
  }

  private boolean isComponentUpdateRequired(@NonNull CameraPosition cameraPosition) {
    return this.cameraPosition != null && (this.cameraPosition.tilt != cameraPosition.tilt
      || this.cameraPosition.bearing != cameraPosition.bearing);
  }

  void cancelTransitions() {
    // notify user about cancel
    cameraChangeDispatcher.onCameraMoveCanceled();

    // notify animateCamera and easeCamera about cancelling
    if (cameraCancelableCallback != null) {
      cameraChangeDispatcher.onCameraIdle();
      cameraCancelableCallback.onCancel();
      cameraCancelableCallback = null;
    }

    // cancel ongoing transitions
    mapView.cancelTransitions();
  }

  @UiThread
  void resetNorth() {
    cancelTransitions();
    mapView.resetNorth();
  }

  //
  // Camera change listener API
  //

  void setOnCameraChangeListener(@Nullable MapboxMap.OnCameraChangeListener listener) {
    this.onCameraChangeListener = listener;
  }

  //
  // non Camera API
  //

  // Zoom in or out

  double getZoom() {
    return cameraPosition.zoom;
  }

  double getRawZoom() {
    return mapView.getZoom();
  }

  void zoom(boolean zoomIn, @NonNull PointF focalPoint) {
    CameraPosition cameraPosition = invalidateCameraPosition();
    if (cameraPosition != null) {
      int newZoom = (int) Math.round(cameraPosition.zoom + (zoomIn ? 1 : -1));
      setZoom(newZoom, focalPoint, MapboxConstants.ANIMATION_DURATION);
    } else {
      // we are not transforming, notify about being idle
      cameraChangeDispatcher.onCameraIdle();
    }
  }

  void setZoom(double zoom, @NonNull PointF focalPoint) {
    mapView.setZoom(zoom, focalPoint);
  }

  void setZoom(double zoom, @NonNull final PointF focalPoint, long duration) {
    ValueAnimator animator = ValueAnimator.ofFloat(
      (float) getRawZoom(),
      (float) zoom
    );
    animator.setDuration(duration);
    animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        mapView.setZoom((Float) animation.getAnimatedValue(), focalPoint);
      }
    });
    animator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationEnd(Animator animation) {
        super.onAnimationEnd(animation);
        cameraChangeDispatcher.onCameraIdle();
      }
    });
    animator.start();
  }

  // Direction
  double getBearing() {
    double direction = -mapView.getBearing();

    while (direction > 360) {
      direction -= 360;
    }
    while (direction < 0) {
      direction += 360;
    }

    return direction;
  }

  double getRawBearing() {
    return mapView.getBearing();
  }

  void setBearing(double bearing) {
    if (myLocationView != null) {
      myLocationView.setBearing(bearing);
    }
    mapView.setBearing(bearing);
  }

  void setBearing(double bearing, float focalX, float focalY) {
    if (myLocationView != null) {
      myLocationView.setBearing(bearing);
    }
    mapView.setBearing(bearing, focalX, focalY);
  }

  void setBearing(double bearing, final float focalX, final float focalY, long duration) {
    ValueAnimator animator = ValueAnimator.ofFloat(
      (float) getRawBearing(),
      (float) bearing
    );
    animator.setDuration(duration);
    animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        setBearing((Float) animation.getAnimatedValue(), focalX, focalY);
      }
    });
    animator.start();
  }

  //
  // LatLng / CenterCoordinate
  //

  LatLng getLatLng() {
    return mapView.getLatLng();
  }

  //
  // Pitch / Tilt
  //

  double getTilt() {
    return mapView.getPitch();
  }

  void setTilt(Double pitch) {
    if (myLocationView != null) {
      myLocationView.setTilt(pitch);
    }
    markerViewManager.setTilt(pitch.floatValue());
    mapView.setPitch(pitch);
  }

  //
  // Center coordinate
  //

  LatLng getCenterCoordinate() {
    return mapView.getLatLng();
  }

  void setCenterCoordinate(LatLng centerCoordinate) {
    mapView.setLatLng(centerCoordinate);
  }

  void setGestureInProgress(boolean gestureInProgress) {
    mapView.setGestureInProgress(gestureInProgress);
    if (!gestureInProgress) {
      invalidateCameraPosition();
    }
  }

  void zoomBy(double z, float x, float y) {
    mapView.setZoom(getRawZoom() + z, new PointF(x, y));
  }

  void moveBy(double offsetX, double offsetY, long duration) {
    PointF offset = new PointF((float) offsetX, (float) offsetY);
    ValueAnimator animator = ValueAnimator.ofObject(new AnimatorUtils.PointFEvaluator(),
      new PointF(),
      offset
    );
    animator.setDuration(duration);
    animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        PointF pointF = (PointF) animation.getAnimatedValue();
        mapView.moveBy(pointF.x, pointF.y);
      }
    });
    animator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationEnd(Animator animation) {
        super.onAnimationEnd(animation);
        cameraChangeDispatcher.onCameraIdle();
      }
    });
    animator.start();
  }

  //
  // Min & Max ZoomLevel
  //

  void setMinZoom(double minZoom) {
    if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Timber.e("Not setting minZoomPreference, value is in unsupported range: %s", minZoom);
      return;
    }
    mapView.setMinZoom(minZoom);
  }

  double getMinZoom() {
    return mapView.getMinZoom();
  }

  void setMaxZoom(double maxZoom) {
    if ((maxZoom < MapboxConstants.MINIMUM_ZOOM) || (maxZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Timber.e("Not setting maxZoomPreference, value is in unsupported range: %s", maxZoom);
      return;
    }
    mapView.setMaxZoom(maxZoom);
  }

  double getMaxZoom() {
    return mapView.getMaxZoom();
  }
}
