package com.mapbox.mapboxsdk.camera;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.MapboxMap;

public interface  CameraUpdate {

    CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap);

}
