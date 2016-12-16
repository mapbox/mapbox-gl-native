package com.mapbox.mapboxsdk.camera;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * Interface definition for camera position changes.
 */
public interface CameraUpdate {

  CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap);

}
