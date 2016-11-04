package com.mapbox.mapboxsdk.maps;

import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;

import java.util.concurrent.TimeUnit;

import static com.mapbox.mapboxsdk.maps.MapView.REGION_DID_CHANGE_ANIMATED;

/**
 * Resembles the current Map transformation.
 * <p>
 * Responsible for synchronising {@link CameraPosition} state and notifying {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraChangeListener}.
 * </p>
 */
class Transform {

    private NativeMapView mapView;
    private MapboxMap mapboxMap;

    private CameraPosition cameraPosition;
    private MapboxMap.CancelableCallback cameraCancelableCallback;
    private MapboxMap.OnCameraChangeListener onCameraChangeListener;

    Transform(NativeMapView mapView, MapboxMap mapboxMap) {
        this.mapView = mapView;
        this.mapboxMap = mapboxMap;
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
    final void moveCamera(CameraUpdate update, MapboxMap.CancelableCallback callback) {
        cameraPosition = update.getCameraPosition(mapboxMap);
        mapboxMap.getTrackingSettings().resetTrackingModesIfRequired(cameraPosition);
        cancelTransitions();
        mapView.jumpTo(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);

        // MapChange.REGION_DID_CHANGE_ANIMATED is not called for `jumpTo`
        // invalidate camera position to provide OnCameraChange event.
        mapboxMap.invalidateCameraPosition();
        if (callback != null) {
            callback.onFinish();
        }
    }

    @UiThread
    final void easeCamera(CameraUpdate update, int durationMs, boolean easingInterpolator, boolean resetTrackingMode, final MapboxMap.CancelableCallback callback) {
        cameraPosition = update.getCameraPosition(mapboxMap);
        if (resetTrackingMode) {
            mapboxMap.getTrackingSettings().resetTrackingModesIfRequired(cameraPosition);
        }

        cancelTransitions();
        if (callback != null) {
            cameraCancelableCallback = callback;
            mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapView.MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
                        cameraCancelableCallback.onFinish();
                        cameraCancelableCallback = null;
                        mapView.removeOnMapChangedListener(this);
                    }
                }
            });
        }

        mapView.easeTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt, cameraPosition.zoom, easingInterpolator);
    }

    @UiThread
    final void animateCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
        cameraPosition = update.getCameraPosition(mapboxMap);
        mapboxMap.getTrackingSettings().resetTrackingModesIfRequired(cameraPosition);

        cancelTransitions();
        if (callback != null) {
            cameraCancelableCallback = callback;
            mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapView.MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
                        cameraCancelableCallback.onFinish();
                        cameraCancelableCallback = null;
                        mapView.removeOnMapChangedListener(this);
                    }
                }
            });
        }

        mapView.flyTo(cameraPosition.bearing, cameraPosition.target, getDurationNano(durationMs), cameraPosition.tilt, cameraPosition.zoom);
    }

    @UiThread
    @Nullable
    CameraPosition invalidateCameraPosition() {
        CameraPosition cameraPosition = null;
        if (mapView != null) {
            cameraPosition = new CameraPosition.Builder(mapView.getCameraValues()).build();
            this.cameraPosition = cameraPosition;
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
}
