// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;

import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.expressions.Expression.*;
import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

/**
 * Basic smoke tests for BackgroundLayer
 */
@RunWith(AndroidJUnit4.class)
public class BackgroundLayerTest extends BaseActivityTest {

  private BackgroundLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      layer = mapboxMap.getLayerAs("background");
    });
  }

  @Test
  public void testSetVisibility() {
    validateTestSetup();
    setupLayer();
    Timber.i("Visibility");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Get initial
      assertEquals(layer.getVisibility().getValue(), VISIBLE);

      // Set
      layer.setProperties(visibility(NONE));
      assertEquals(layer.getVisibility().getValue(), NONE);
    });
  }

  @Test
  public void testBackgroundColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setBackgroundColorTransition(options);
      assertEquals(layer.getBackgroundColorTransition(), options);
    });
  }

  @Test
  public void testBackgroundColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(backgroundColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getBackgroundColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testBackgroundColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(backgroundColor(Color.RED));
      assertEquals(layer.getBackgroundColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testBackgroundPatternTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-patternTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setBackgroundPatternTransition(options);
      assertEquals(layer.getBackgroundPatternTransition(), options);
    });
  }

  @Test
  public void testBackgroundPatternAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-pattern");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(backgroundPattern("pedestrian-polygon"));
      assertEquals((String) layer.getBackgroundPattern().getValue(), (String) "pedestrian-polygon");
    });
  }

  @Test
  public void testBackgroundOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setBackgroundOpacityTransition(options);
      assertEquals(layer.getBackgroundOpacityTransition(), options);
    });
  }

  @Test
  public void testBackgroundOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("background-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(backgroundOpacity(0.3f));
      assertEquals((Float) layer.getBackgroundOpacity().getValue(), (Float) 0.3f);
    });
  }
}