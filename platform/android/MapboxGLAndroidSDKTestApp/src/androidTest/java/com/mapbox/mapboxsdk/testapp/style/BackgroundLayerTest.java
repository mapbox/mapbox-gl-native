package com.mapbox.mapboxsdk.testapp.style;
// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.backgroundColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.backgroundOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.backgroundPattern;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Basic smoke tests for BackgroundLayer
 */
@RunWith(AndroidJUnit4.class)
public class BackgroundLayerTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private BackgroundLayer layer;

  private OnMapReadyIdlingResource idlingResource;

  private MapboxMap mapboxMap;

  @Before
  public void setup() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  public void testSetVisibility() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    Timber.i("Retrieving layer");
    layer = mapboxMap.getLayerAs("background");
    Timber.i("visibility");
    assertNotNull(layer);

    // Get initial
    assertEquals(layer.getVisibility().getValue(), VISIBLE);

    // Set
    layer.setProperties(visibility(NONE));
    assertEquals(layer.getVisibility().getValue(), NONE);
  }

  @Test
  public void testBackgroundColor() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    Timber.i("Retrieving layer");
    layer = mapboxMap.getLayerAs("background");
    Timber.i("background-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getBackgroundColor().getValue(),
                 (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testBackgroundColorAsInt() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    Timber.i("Retrieving layer");
    layer = mapboxMap.getLayerAs("background");
    Timber.i("background-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundColor(Color.RED));
    assertEquals(layer.getBackgroundColorAsInt(), Color.RED);
  }

  @Test
  public void testBackgroundPattern() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    Timber.i("Retrieving layer");
    layer = mapboxMap.getLayerAs("background");
    Timber.i("background-pattern");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundPattern("pedestrian-polygon"));
    assertEquals((String) layer.getBackgroundPattern().getValue(),
                 (String) "pedestrian-polygon");
  }

  @Test
  public void testBackgroundOpacity() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    Timber.i("Retrieving layer");
    layer = mapboxMap.getLayerAs("background");
    Timber.i("background-opacity");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(backgroundOpacity(0.3f));
    assertEquals((Float) layer.getBackgroundOpacity().getValue(),
                 (Float) 0.3f);
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
