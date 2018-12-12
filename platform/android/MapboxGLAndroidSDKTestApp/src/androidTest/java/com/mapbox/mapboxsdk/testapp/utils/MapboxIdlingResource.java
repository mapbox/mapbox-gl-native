package com.mapbox.mapboxsdk.testapp.utils;

import android.app.Activity;
import android.support.annotation.UiThread;
import android.support.test.espresso.IdlingResource;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

public abstract class MapboxIdlingResource implements IdlingResource {

  private MapboxMap mapboxMap;
  IdlingResource.ResourceCallback resourceCallback;

  @UiThread
  void inflateMap(Activity activity) {
    MapView mapView = activity.findViewById(R.id.mapView);
    if (mapView != null) {
      mapView.getMapAsync(this::initMap);
    }
  }

  @UiThread
  protected void initMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
  }

  @Override
  public String getName() {
    return getClass().getSimpleName();
  }

  @Override
  public void registerIdleTransitionCallback(ResourceCallback resourceCallback) {
    this.resourceCallback = resourceCallback;
  }

  public MapboxMap getMapboxMap() {
    return mapboxMap;
  }
}