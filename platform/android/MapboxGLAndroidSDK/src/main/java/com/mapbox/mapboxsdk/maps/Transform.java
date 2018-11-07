package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.log.Logger;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;

/**
 * Resembles the current Map transformation.
 * <p>
 * Responsible for synchronising {@link CameraPosition} state and notifying camera change listeners.
 * </p>
 */
final class Transform implements MapView.OnCameraDidChangeListener {

  private static final String TAG = "Mbgl-Transform";

  private final NativeMapView nativeMapView;
  private final MapView mapView;
  private final MarkerViewManager markerViewManager;
  private final Handler handler = new Handler();

  @Nullable
  private CameraPosition cameraPosition;
  @Nullable
  private MapboxMap.CancelableCallback cameraCancelableCallback;

  private CameraChangeDispatcher cameraChangeDispatcher;

  private final MapView.OnCameraDidChangeListener moveByChangeListener = new MapView.OnCameraDidChangeListener() {
    @Override
    public void onCameraDidChange(boolean animated) {
      if (animated) {
        cameraChangeDispatcher.onCameraIdle();
        mapView.removeOnCameraDidChangeListener(this);
      }
    }
  };

  Transform(MapView mapView, NativeMapView nativeMapView, MarkerViewManager markerViewManager,
            CameraChangeDispatcher cameraChangeDispatcher) {
    this.mapView = mapView;
    this.nativeMapView = nativeMapView;
    this.markerViewManager = markerViewManager;
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

  @Nullable
  @UiThread
  public final CameraPosition getCameraPosition() {
    if (cameraPosition == null) {
      cameraPosition = invalidateCameraPosition();
    }
    return cameraPosition;
  }

  @UiThread
  void updateCameraPosition(@NonNull CameraPosition position) {
    markerViewManager.setTilt((float) position.tilt);
  }

  @Override
  public void onCameraDidChange(boolean animated) {
    if (animated) {
      updateCameraPosition(invalidateCameraPosition());
      if (cameraCancelableCallback != null) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            if (cameraCancelableCallback != null) {
              cameraCancelableCallback.onFinish();
              cameraCancelableCallback = null;
            }
          }
        });
      }
      cameraChangeDispatcher.onCameraIdle();
      mapView.removeOnCameraDidChangeListener(this);
    }
  }

  @UiThread
  final void moveCamera(@NonNull MapboxMap mapboxMap, CameraUpdate update,
                        @Nullable final MapboxMap.CancelableCallback callback) {
    CameraPosition cameraPosition = update.getCameraPosition(mapboxMap);
    if (isValidCameraPosition(cameraPosition)) {
      cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(OnCameraMoveStartedListener.REASON_API_ANIMATION);
      nativeMapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
      cameraChangeDispatcher.onCameraIdle();
      invalidateCameraPosition();
      handler.post(new Runnable() {
        @Override
        public void run() {
          if (callback != null) {
            callback.onFinish();
          }
        }
      });
    }
  }

  @UiThread
  final void easeCamera(@NonNull MapboxMap mapboxMap, CameraUpdate update, int durationMs, boolean easingInterpolator,
                        @Nullable final MapboxMap.CancelableCallback callback) {
    CameraPosition cameraPosition = update.getCameraPosition(mapboxMap);
    if (isValidCameraPosition(cameraPosition)) {
      cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(OnCameraMoveStartedListener.REASON_API_ANIMATION);

      if (callback != null) {
        cameraCancelableCallback = callback;
      }
      mapView.addOnCameraDidChangeListener(this);
      nativeMapView.easeTo(cameraPosition.bearing, cameraPosition.target, durationMs, cameraPosition.tilt,
        cameraPosition.zoom, easingInterpolator);
    }
  }

  @UiThread
  final void animateCamera(@NonNull MapboxMap mapboxMap, CameraUpdate update, int durationMs,
                           @Nullable final MapboxMap.CancelableCallback callback) {
    CameraPosition cameraPosition = update.getCameraPosition(mapboxMap);
    if (isValidCameraPosition(cameraPosition)) {
      cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(OnCameraMoveStartedListener.REASON_API_ANIMATION);

      if (callback != null) {
        cameraCancelableCallback = callback;
      }
      mapView.addOnCameraDidChangeListener(this);
      nativeMapView.flyTo(cameraPosition.bearing, cameraPosition.target, durationMs, cameraPosition.tilt,
        cameraPosition.zoom);
    }
  }

  private boolean isValidCameraPosition(@Nullable CameraPosition cameraPosition) {
    return cameraPosition != null && !cameraPosition.equals(this.cameraPosition);
  }

  @UiThread
  @Nullable
  CameraPosition invalidateCameraPosition() {
    if (nativeMapView != null) {
      CameraPosition cameraPosition = nativeMapView.getCameraPosition();
      if (this.cameraPosition != null && !this.cameraPosition.equals(cameraPosition)) {
        cameraChangeDispatcher.onCameraMove();
      }

      if (isComponentUpdateRequired(cameraPosition)) {
        updateCameraPosition(cameraPosition);
      }

      this.cameraPosition = cameraPosition;
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
      final MapboxMap.CancelableCallback callback = cameraCancelableCallback;
      cameraChangeDispatcher.onCameraIdle();
      handler.post(new Runnable() {
        @Override
        public void run() {
          callback.onCancel();
        }
      });
      cameraCancelableCallback = null;
    }

    // cancel ongoing transitions
    nativeMapView.cancelTransitions();

    cameraChangeDispatcher.onCameraIdle();
  }

  @UiThread
  void resetNorth() {
    cancelTransitions();
    nativeMapView.resetNorth();
  }

  //
  // non Camera API
  //

  // Zoom in or out

  double getRawZoom() {
    return nativeMapView.getZoom();
  }

  void zoomBy(double zoomAddition, @NonNull PointF focalPoint) {
    setZoom(nativeMapView.getZoom() + zoomAddition, focalPoint);
  }

  void setZoom(double zoom, @NonNull PointF focalPoint) {
    nativeMapView.setZoom(zoom, focalPoint, 0);
  }

  // Direction
  double getBearing() {
    double direction = -nativeMapView.getBearing();

    while (direction > 360) {
      direction -= 360;
    }
    while (direction < 0) {
      direction += 360;
    }

    return direction;
  }

  double getRawBearing() {
    return nativeMapView.getBearing();
  }

  void setBearing(double bearing) {
    nativeMapView.setBearing(bearing);
  }

  void setBearing(double bearing, float focalX, float focalY) {
    nativeMapView.setBearing(bearing, focalX, focalY);
  }

  void setBearing(double bearing, float focalX, float focalY, long duration) {
    nativeMapView.setBearing(bearing, focalX, focalY, duration);
  }


  //
  // LatLng / CenterCoordinate
  //

  LatLng getLatLng() {
    return nativeMapView.getLatLng();
  }

  //
  // Pitch / Tilt
  //

  double getTilt() {
    return nativeMapView.getPitch();
  }

  void setTilt(Double pitch) {
    markerViewManager.setTilt(pitch.floatValue());
    nativeMapView.setPitch(pitch, 0);
  }

  //
  // Center coordinate
  //

  LatLng getCenterCoordinate() {
    return nativeMapView.getLatLng();
  }

  void setCenterCoordinate(LatLng centerCoordinate) {
    nativeMapView.setLatLng(centerCoordinate);
  }

  void setGestureInProgress(boolean gestureInProgress) {
    nativeMapView.setGestureInProgress(gestureInProgress);
    if (!gestureInProgress) {
      invalidateCameraPosition();
    }
  }

  void moveBy(double offsetX, double offsetY, long duration) {
    if (duration > 0) {
      mapView.addOnCameraDidChangeListener(moveByChangeListener);
    }
    nativeMapView.moveBy(offsetX, offsetY, duration);
  }

  //
  // Min & Max ZoomLevel
  //

  void setMinZoom(double minZoom) {
    if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Logger.e(TAG, String.format("Not setting minZoomPreference, value is in unsupported range: %s", minZoom));
      return;
    }
    nativeMapView.setMinZoom(minZoom);
  }

  double getMinZoom() {
    return nativeMapView.getMinZoom();
  }

  void setMaxZoom(double maxZoom) {
    if ((maxZoom < MapboxConstants.MINIMUM_ZOOM) || (maxZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Logger.e(TAG, String.format("Not setting maxZoomPreference, value is in unsupported range: %s", maxZoom));
      return;
    }
    nativeMapView.setMaxZoom(maxZoom);
  }

  double getMaxZoom() {
    return nativeMapView.getMaxZoom();
  }
}
