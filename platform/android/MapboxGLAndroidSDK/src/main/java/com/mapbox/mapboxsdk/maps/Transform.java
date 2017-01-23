package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.util.concurrent.TimeUnit;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.maps.MapView.REGION_DID_CHANGE_ANIMATED;

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

  Transform(NativeMapView mapView, MarkerViewManager markerViewManager, TrackingSettings trackingSettings) {
    this.mapView = mapView;
    this.markerViewManager = markerViewManager;
    this.trackingSettings = trackingSettings;
    this.myLocationView = trackingSettings.getMyLocationView();
  }

  void initialise(@NonNull MapboxMap mapboxMap, @NonNull MapboxMapOptions options) {
    CameraPosition position = options.getCamera();
    if (position != null && !position.equals(CameraPosition.DEFAULT)) {
      moveCamera(mapboxMap, CameraUpdateFactory.newCameraPosition(position), null);
    }
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
    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
      updateCameraPosition(invalidateCameraPosition());
      if (cameraCancelableCallback != null) {
        cameraCancelableCallback.onFinish();
        cameraCancelableCallback = null;
      }
      mapView.removeOnMapChangedListener(this);
    }
  }

  @UiThread
  final void moveCamera(MapboxMap mapboxMap, CameraUpdate update, MapboxMap.CancelableCallback callback) {
    cameraPosition = update.getCameraPosition(mapboxMap);
    trackingSettings.resetTrackingModesIfRequired(cameraPosition);
    cancelTransitions();
    mapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
    if (callback != null) {
      callback.onFinish();
    }
  }

  @UiThread
  final void easeCamera(MapboxMap mapboxMap, CameraUpdate update, int durationMs, boolean easingInterpolator,
                        boolean resetTrackingMode, final MapboxMap.CancelableCallback callback) {
    cameraPosition = update.getCameraPosition(mapboxMap);
    if (resetTrackingMode) {
      trackingSettings.resetTrackingModesIfRequired(cameraPosition);
    }

    cancelTransitions();
    if (callback != null) {
      cameraCancelableCallback = callback;
      mapView.addOnMapChangedListener(this);
    }

    mapView.easeTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt,
      cameraPosition.zoom, easingInterpolator);
  }

  @UiThread
  final void animateCamera(MapboxMap mapboxMap, CameraUpdate update, int durationMs,
                           final MapboxMap.CancelableCallback callback) {
    cameraPosition = update.getCameraPosition(mapboxMap);
    trackingSettings.resetTrackingModesIfRequired(cameraPosition);

    cancelTransitions();
    if (callback != null) {
      cameraCancelableCallback = callback;
      mapView.addOnMapChangedListener(this);
    }

    mapView.flyTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt,
      cameraPosition.zoom);
  }

  @UiThread
  @Nullable
  CameraPosition invalidateCameraPosition() {
    if (mapView != null) {
      cameraPosition = new CameraPosition.Builder(mapView.getCameraValues()).build();
      if (onCameraChangeListener != null) {
        onCameraChangeListener.onCameraChange(this.cameraPosition);
      }
    }
    return cameraPosition;
  }

  void cancelTransitions() {
    if (cameraCancelableCallback != null) {
      cameraCancelableCallback.onCancel();
      cameraCancelableCallback = null;
    }
    mapView.cancelTransitions();
  }

  @UiThread
  void resetNorth() {
    cancelTransitions();
    mapView.resetNorth();
  }

  void setOnCameraChangeListener(@Nullable MapboxMap.OnCameraChangeListener listener) {
    this.onCameraChangeListener = listener;
  }

  private long getDurationNano(long durationMs) {
    return durationMs > 0 ? TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS) : 0;
  }

  //
  // non Camera API
  //

  // Zoom in or out

  double getZoom() {
    return cameraPosition.zoom;
  }

  void zoom(boolean zoomIn) {
    zoom(zoomIn, -1.0f, -1.0f);
  }

  void zoom(boolean zoomIn, float x, float y) {
    // Cancel any animation
    cancelTransitions();

    if (zoomIn) {
      mapView.scaleBy(2.0, x, y, MapboxConstants.ANIMATION_DURATION);
    } else {
      mapView.scaleBy(0.5, x, y, MapboxConstants.ANIMATION_DURATION);
    }
  }

  void setZoom(double zoom) {
    mapView.setZoom(zoom);
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
    mapView.setPitch(pitch, 0);
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

  void zoomBy(double pow, float x, float y) {
    mapView.scaleBy(pow, x, y);
  }

  void moveBy(double offsetX, double offsetY, long duration) {
    mapView.moveBy(offsetX, offsetY, duration);
  }

  //
  // Min & Max ZoomLevel
  //

  void setMinZoom(double minZoom) {
    if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Timber.e("Not setting minZoomPreference, value is in unsupported range: " + minZoom);
      return;
    }
    mapView.setMinZoom(minZoom);
  }

  double getMinZoom() {
    return mapView.getMinZoom();
  }

  void setMaxZoom(double maxZoom) {
    if ((maxZoom < MapboxConstants.MINIMUM_ZOOM) || (maxZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Timber.e("Not setting maxZoomPreference, value is in unsupported range: " + maxZoom);
      return;
    }
    mapView.setMaxZoom(maxZoom);
  }

  double getMaxZoom() {
    return mapView.getMaxZoom();
  }
}
