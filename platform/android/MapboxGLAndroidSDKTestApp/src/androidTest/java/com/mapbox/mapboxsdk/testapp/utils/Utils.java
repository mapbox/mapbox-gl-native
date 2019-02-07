package com.mapbox.mapboxsdk.testapp.utils;

import android.support.test.espresso.UiController;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import static org.junit.Assert.assertFalse;

public class Utils {

  private static final long WAIT_TIMEOUT = 5000;
  private static final long WAIT_DELAY = 150;

  public static void waitForLayer(UiController uiController, MapboxMap mapboxMap, LatLng latLng, String... layerIds) {
    int i = 0;
    while (mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng), layerIds).isEmpty()) {
      i++;
      assertFalse("Waiting for layer timed out", i * WAIT_DELAY > WAIT_TIMEOUT);
      uiController.loopMainThreadForAtLeast(WAIT_DELAY);
    }
  }
}