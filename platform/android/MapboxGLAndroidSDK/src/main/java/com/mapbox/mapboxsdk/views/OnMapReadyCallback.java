package com.mapbox.mapboxsdk.views;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.views.MapboxMap;

public interface OnMapReadyCallback {

    void onMapReady(@NonNull MapboxMap mapboxMap);

}
