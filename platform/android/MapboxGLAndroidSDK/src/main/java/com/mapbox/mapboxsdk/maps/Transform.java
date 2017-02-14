package com.mapbox.mapboxsdk.maps;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.annotation.WorkerThread;

import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.util.concurrent.TimeUnit;

import timber.log.Timber;

/**
 * Resembles the current Map transformation.
 * <p>
 * Responsible for synchronising {@link CameraPosition} state and notifying
 * {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraChangeListener}.
 * </p>
 */
// FIXME use MapState instead of exending MapThreadExecutor
final class Transform extends MapThreadExecutor implements State {

  private final MarkerViewManager markerViewManager;
  private final TrackingSettings trackingSettings;
  private final MyLocationView myLocationView;

  private MapboxMap.CancelableCallback cameraCancelableCallback;
  private MapboxMap.OnCameraChangeListener onCameraChangeListener;

  private LatLng centerLatLng;
  private double bearing;
  private double tilt;
  private double zoom;

  Transform(NativeMapView mapView, ThreadExecutor threadExecutor, MarkerViewManager markerViewManager,
            TrackingSettings trackingSettings) {
    super(mapView, threadExecutor);
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
  // State
  //

  @Override
  public void onSaveInstanceState(Bundle outState) {
    outState.putDouble(MapboxConstants.STATE_CAMERA_LAT, centerLatLng.getLatitude());
    outState.putDouble(MapboxConstants.STATE_CAMERA_LNG, centerLatLng.getLongitude());
    outState.putDouble(MapboxConstants.STATE_CAMERA_BEARING, bearing);
    outState.putDouble(MapboxConstants.STATE_CAMERA_TILT, tilt);
    outState.putDouble(MapboxConstants.STATE_CAMERA_ZOOM, zoom);
  }

  @Override
  public void onRestoreInstanceState(Bundle savedInstanceState) {
    double latitude = savedInstanceState.getDouble(MapboxConstants.STATE_CAMERA_LAT, 0);
    double longitude = savedInstanceState.getDouble(MapboxConstants.STATE_CAMERA_LNG, 0);
    centerLatLng = new LatLng(latitude, longitude);
    bearing = savedInstanceState.getDouble(MapboxConstants.STATE_CAMERA_BEARING, 0);
    tilt = savedInstanceState.getDouble(MapboxConstants.STATE_CAMERA_TILT, 0);
    zoom = savedInstanceState.getDouble(MapboxConstants.STATE_CAMERA_ZOOM, 0);
    // FIXME: 10/02/2017 ACTUALLY RESTORE STATE
  }

  //
  // Camera API
  //

  @UiThread
  public final CameraPosition getCameraPosition() {
    return new CameraPosition.Builder(centerLatLng, zoom, tilt, bearing).build();
  }

  void setCameraPosition(CameraPosition cameraPostion) {
    centerLatLng = cameraPostion.target;
    tilt = cameraPostion.tilt;
    bearing = cameraPostion.bearing;
    zoom = cameraPostion.zoom;
  }

  @UiThread
  void updateCameraPosition(@NonNull CameraPosition position) {
    if (myLocationView != null) {
      myLocationView.setCameraPosition(position);
    }
    markerViewManager.setTilt((float) position.tilt);
  }

  @UiThread
  final void moveCamera(final MapboxMap mapboxMap, final CameraUpdate update,
                        final MapboxMap.CancelableCallback callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        CameraPosition cameraPosition = update.getCameraPosition(mapboxMap);
        setCameraPosition(cameraPosition);
        cancelTransitions(nativeMapView);
        nativeMapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
        trackingSettings.resetTrackingModesIfRequired(cameraPosition);
        if (callback != null) {
          callback.onFinish();
        }
      }
    });

  }

  @UiThread
  final void easeCamera(final MapboxMap mapboxMap, final CameraUpdate update, final int durationMs,
                        final boolean easingInterpolator, final MapboxMap.CancelableCallback callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        CameraPosition position = update.getCameraPosition(mapboxMap);
        trackingSettings.resetTrackingModesIfRequired(position);

        cancelTransitions(nativeMapView);
        if (callback != null) {
          cameraCancelableCallback = callback;
          // nativeMapView.addOnMapChangedListener(Transform.this);
        }

        nativeMapView.easeTo(position.bearing, position.target, getDurationNano(durationMs), position.tilt,
          position.zoom, easingInterpolator);
      }
    });
  }

  @UiThread
  final void animateCamera(final MapboxMap mapboxMap, final CameraUpdate update, final int durationMs,
                           final MapboxMap.CancelableCallback callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        CameraPosition position = update.getCameraPosition(mapboxMap);
        trackingSettings.resetTrackingModesIfRequired(position);

        cancelTransitions(nativeMapView);
        if (callback != null) {
          cameraCancelableCallback = callback;
          // nativeMapView.addOnMapChangedListener(Transform.this);
        }

        nativeMapView.flyTo(position.bearing, position.target, getDurationNano(durationMs), position.tilt,
          position.zoom);
      }
    });
  }

  @UiThread
  void invalidateCameraPosition() {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final CameraPosition cameraPosition = new CameraPosition.Builder(nativeMapView.getCameraValues()).build();
        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            updateCameraPosition(cameraPosition);
            if (onCameraChangeListener != null) {
              // post camera change event on ui Thread
              onCameraChangeListener.onCameraChange(cameraPosition);
            }
          }
        });
      }
    });
  }

  @UiThread
  void cancelTransitions() {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        cancelTransitions(nativeMapView);
      }
    });
  }

  @WorkerThread
  void cancelTransitions(NativeMapView nativeMapView) {
    if (cameraCancelableCallback != null) {
      queueUiEvent(new Runnable() {
        @Override
        public void run() {
          cameraCancelableCallback.onCancel();
          cameraCancelableCallback = null;
        }
      });
    }
    nativeMapView.cancelTransitions();
  }

  @UiThread
  void resetNorth() {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        cancelTransitions(nativeMapView);
        nativeMapView.resetNorth();
      }
    });
  }

  @UiThread
  void setOnCameraChangeListener(@Nullable MapboxMap.OnCameraChangeListener listener) {
    this.onCameraChangeListener = listener;
  }

  private long getDurationNano(long durationMs) {
    return durationMs > 0 ? TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS) : 0;
  }

  @UiThread
  double getZoom() {
    return zoom;
  }

  @UiThread
  void zoom(boolean zoomIn) {
    zoom = zoomIn ? zoom + 1 : zoom - 1;
    zoom(zoomIn, -1.0f, -1.0f);
  }

  @UiThread
  void zoom(final boolean zoomIn, final float x, final float y) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        cancelTransitions(nativeMapView);

        if (zoomIn) {
          nativeMapView.scaleBy(2.0, x, y, MapboxConstants.ANIMATION_DURATION);
        } else {
          nativeMapView.scaleBy(0.5, x, y, MapboxConstants.ANIMATION_DURATION);
        }
      }
    });

  }

  @UiThread
  void setZoom(final double zoom) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setZoom(zoom);
      }
    });
  }

  @UiThread
  double getBearing() {
    return bearing;
  }

  @UiThread
  void setBearing(final double bearing) {
    if (myLocationView != null) {
      myLocationView.setBearing(bearing);
    }
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setBearing(bearing);
      }
    });
  }

  @UiThread
  void setBearing(final double bearing, final float focalX, final float focalY) {
    this.bearing = bearing;
    if (myLocationView != null) {
      myLocationView.setBearing(bearing);
    }
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setBearing(bearing, focalX, focalY);
      }
    });
  }

  //
  // Pitch / Tilt
  //

  @UiThread
  double getTilt() {
    return tilt;
  }

  @UiThread
  void setTilt(final Double pitch) {
    tilt = pitch;
    if (myLocationView != null) {
      myLocationView.setTilt(pitch);
    }
    markerViewManager.setTilt(pitch.floatValue());
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setPitch(pitch, 0);
      }
    });
  }

  //
  // Center coordinate
  //

  void setGestureInProgress(final boolean gestureInProgress) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setGestureInProgress(gestureInProgress);
      }
    });
    if (!gestureInProgress) {
      invalidateCameraPosition();
    }
  }

  void zoomBy(final double pow, final float x, final float y) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.scaleBy(pow, x, y);
      }
    });
  }

  void moveBy(final double offsetX, final double offsetY, final long duration) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.moveBy(offsetX, offsetY, duration);
      }
    });
  }

  //
  // Min & Max ZoomLevel
  //

  private double minZoom;
  private double maxZoom;

  void setMinZoom(final double minZoom) {
    if ((minZoom < MapboxConstants.MINIMUM_ZOOM) || (minZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Timber.e("Not setting minZoomPreference, value is in unsupported range: " + minZoom);
      return;
    }
    this.minZoom = minZoom;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setMinZoom(minZoom);
      }
    });
  }

  double getMinZoom() {
    return minZoom;
  }

  void setMaxZoom(final double maxZoom) {
    if ((maxZoom < MapboxConstants.MINIMUM_ZOOM) || (maxZoom > MapboxConstants.MAXIMUM_ZOOM)) {
      Timber.e("Not setting maxZoomPreference, value is in unsupported range: " + maxZoom);
      return;
    }

    this.maxZoom = maxZoom;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setMaxZoom(maxZoom);
      }
    });
  }

  double getMaxZoom() {
    return maxZoom;
  }
}
