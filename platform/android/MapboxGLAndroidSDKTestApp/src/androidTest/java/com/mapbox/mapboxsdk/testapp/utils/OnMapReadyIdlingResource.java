package com.mapbox.mapboxsdk.testapp.utils;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.annotation.WorkerThread;
import android.support.test.espresso.IdlingResource;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

import junit.framework.Assert;

public class OnMapReadyIdlingResource implements IdlingResource, OnMapReadyCallback {

  private boolean styleLoaded;
  private MapboxMap mapboxMap;
  private IdlingResource.ResourceCallback resourceCallback;

  @WorkerThread
  public OnMapReadyIdlingResource(final Activity activity) {
    Handler handler = new Handler(Looper.getMainLooper());
    handler.post(() -> {
      MapView mapView = activity.findViewById(R.id.mapView);
      if (mapView != null) {
        mapView.addOnDidFinishLoadingStyleListener(() -> {
          styleLoaded = true;
          if (resourceCallback != null) {
            resourceCallback.onTransitionToIdle();
          }
        });
        mapView.getMapAsync(this::initMap);
      }
    });
  }

  private void initMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    mapboxMap.setStyle(Style.MAPBOX_STREETS);
  }

  @Override
  public String getName() {
    return getClass().getSimpleName();
  }

  @Override
  public boolean isIdleNow() {
    return styleLoaded;
  }

  @Override
  public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
    this.resourceCallback = resourceCallback;
  }

  public MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    Assert.assertNotNull("MapboxMap should not be null", mapboxMap);
    this.mapboxMap = mapboxMap;
  }
}