package com.mapbox.mapboxsdk.testapp.style;
// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.Property.CIRCLE_PITCH_SCALE_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.CIRCLE_TRANSLATE_ANCHOR_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleBlur;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circlePitchScale;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleTranslate;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleTranslateAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Basic smoke tests for CircleLayer
 */
@RunWith(AndroidJUnit4.class)
public class CircleLayerStyleTest extends BaseStyleTest {

  @Rule
  public final ActivityTestRule<RuntimeStyleTestActivity> rule = new ActivityTestRule<>(RuntimeStyleTestActivity.class);

  private CircleLayer layer;

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

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("visibility");
    assertNotNull(layer);

    //Get initial
    assertEquals(layer.getVisibility().getValue(), VISIBLE);

    //Set
    layer.setProperties(visibility(NONE));
    assertEquals(layer.getVisibility().getValue(), NONE);
  }

  @Test
  public void testCircleRadius() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-radius");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleRadius(0.3f));
    assertEquals((Float) layer.getCircleRadius().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleColor() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-color");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleColor("rgba(0, 0, 0, 1)"));
    assertEquals((String) layer.getCircleColor().getValue(), (String) "rgba(0, 0, 0, 1)");
  }

  @Test
  public void testCircleColorAsInt() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-color");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleColor(Color.RED));
    assertEquals(layer.getCircleColorAsInt(), Color.RED);
  }

  @Test
  public void testCircleBlur() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-blur");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleBlur(0.3f));
    assertEquals((Float) layer.getCircleBlur().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleOpacity() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-opacity");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleOpacity(0.3f));
    assertEquals((Float) layer.getCircleOpacity().getValue(), (Float) 0.3f);
  }

  @Test
  public void testCircleTranslate() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-translate");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleTranslate(new Float[] {0f, 0f}));
    assertEquals((Float[]) layer.getCircleTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
  }

  @Test
  public void testCircleTranslateAnchor() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-translate-anchor");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circleTranslateAnchor(CIRCLE_TRANSLATE_ANCHOR_MAP));
    assertEquals((String) layer.getCircleTranslateAnchor().getValue(), (String) CIRCLE_TRANSLATE_ANCHOR_MAP);
  }

  @Test
  public void testCirclePitchScale() {
    checkViewIsDisplayed(R.id.mapView);

    mapboxMap = rule.getActivity().getMapboxMap();

    if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
      Timber.i("Adding layer");
      layer = new CircleLayer("my-layer", "composite");
      layer.setSourceLayer("composite");
      mapboxMap.addLayer(layer);
      //Layer reference is now stale, get new reference
      layer = mapboxMap.getLayerAs("my-layer");
    }
    Timber.i("circle-pitch-scale");
    assertNotNull(layer);

    //Set and Get
    layer.setProperties(circlePitchScale(CIRCLE_PITCH_SCALE_MAP));
    assertEquals((String) layer.getCirclePitchScale().getValue(), (String) CIRCLE_PITCH_SCALE_MAP);
  }


  @After
  public void unregisterIntentServiceIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
