package com.mapbox.mapboxsdk.testapp.utils;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.UiThread;
import android.support.annotation.WorkerThread;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class LoadStyleIdlingResource extends MapboxIdlingResource {

  private Style style;

  @WorkerThread
  public LoadStyleIdlingResource(final Activity activity) {
    new Handler(Looper.getMainLooper()).post(() -> inflateMap(activity));
  }

  @UiThread
  public void initMap(MapboxMap mapboxMap) {
    super.initMap(mapboxMap);
    mapboxMap.setStyle("asset://streets.json", style -> {
      assertNotNull(style);
      assertTrue(style.isFullyLoaded());
      this.style = style;
      if (resourceCallback != null) {
        resourceCallback.onTransitionToIdle();
      }
    });
  }

  @Override
  public boolean isIdleNow() {
    return style != null;
  }
}