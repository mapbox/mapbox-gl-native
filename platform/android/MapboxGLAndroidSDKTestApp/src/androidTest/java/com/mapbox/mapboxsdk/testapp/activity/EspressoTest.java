package com.mapbox.mapboxsdk.testapp.activity;

import androidx.annotation.UiThread;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;


/**
 * Base class for all tests using EspressoTestActivity as wrapper.
 * <p>
 * Loads "assets/streets.json" as style.
 * </p>
 */
public class EspressoTest extends BaseTest {

  @Override
  protected final Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @UiThread
  @Override
  protected void initMap(MapboxMap mapboxMap) {
    mapboxMap.setStyle(new Style.Builder().fromUri("asset://streets.json"));
    super.initMap(mapboxMap);
  }
}
