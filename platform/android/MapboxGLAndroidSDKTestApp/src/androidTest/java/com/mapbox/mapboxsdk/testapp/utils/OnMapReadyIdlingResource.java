package com.mapbox.mapboxsdk.testapp.utils;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.WorkerThread;
import android.support.test.espresso.IdlingResource;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class OnMapReadyIdlingResource implements IdlingResource, OnMapReadyCallback {

  private MapboxMap mapboxMap;
  private IdlingResource.ResourceCallback resourceCallback;
  private final Handler handler = new Handler(Looper.getMainLooper());

  @WorkerThread
  public OnMapReadyIdlingResource(final Activity activity) {
    handler.post(new Runnable() {
      @Override
      public void run() {
        MapView mapView = (MapView) activity.findViewById(R.id.mapView);
        if (mapView != null) {
          mapView.getMapAsync(OnMapReadyIdlingResource.this);
        }
      }
    });
  }

  @Override
  public String getName() {
    return getClass().getSimpleName();
  }

  @Override
  public boolean isIdleNow() {
    return mapboxMap != null;
  }

  @Override
  public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
    this.resourceCallback = resourceCallback;
  }

  public MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    if (resourceCallback != null) {
      resourceCallback.onTransitionToIdle();
    }
  }
}