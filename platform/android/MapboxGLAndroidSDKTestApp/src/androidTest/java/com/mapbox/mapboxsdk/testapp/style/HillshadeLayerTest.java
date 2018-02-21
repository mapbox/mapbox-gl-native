// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.HillshadeLayer;
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
 * Basic smoke tests for HillshadeLayer
 */
@RunWith(AndroidJUnit4.class)
public class HillshadeLayerTest extends BaseActivityTest {

  private HillshadeLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new HillshadeLayer("my-layer", "composite");
        layer.setSourceLayer("composite");
        mapboxMap.addLayer(layer);
        // Layer reference is now stale, get new reference
        layer = mapboxMap.getLayerAs("my-layer");
      }
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
  public void testHillshadeIlluminationDirectionAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-direction");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeIlluminationDirection(0.3f));
      assertEquals((Float) layer.getHillshadeIlluminationDirection().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testHillshadeIlluminationAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeIlluminationAnchor(HILLSHADE_ILLUMINATION_ANCHOR_MAP));
      assertEquals((String) layer.getHillshadeIlluminationAnchor().getValue(), (String) HILLSHADE_ILLUMINATION_ANCHOR_MAP);
    });
  }

  @Test
  public void testHillshadeExaggerationTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-exaggerationTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHillshadeExaggerationTransition(options);
      assertEquals(layer.getHillshadeExaggerationTransition(), options);
    });
  }

  @Test
  public void testHillshadeExaggerationAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-exaggeration");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeExaggeration(0.3f));
      assertEquals((Float) layer.getHillshadeExaggeration().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testHillshadeShadowColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHillshadeShadowColorTransition(options);
      assertEquals(layer.getHillshadeShadowColorTransition(), options);
    });
  }

  @Test
  public void testHillshadeShadowColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeShadowColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getHillshadeShadowColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testHillshadeShadowColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-shadow-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeShadowColor(Color.RED));
      assertEquals(layer.getHillshadeShadowColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testHillshadeHighlightColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHillshadeHighlightColorTransition(options);
      assertEquals(layer.getHillshadeHighlightColorTransition(), options);
    });
  }

  @Test
  public void testHillshadeHighlightColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeHighlightColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getHillshadeHighlightColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testHillshadeHighlightColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-highlight-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeHighlightColor(Color.RED));
      assertEquals(layer.getHillshadeHighlightColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testHillshadeAccentColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHillshadeAccentColorTransition(options);
      assertEquals(layer.getHillshadeAccentColorTransition(), options);
    });
  }

  @Test
  public void testHillshadeAccentColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeAccentColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getHillshadeAccentColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testHillshadeAccentColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-accent-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(hillshadeAccentColor(Color.RED));
      assertEquals(layer.getHillshadeAccentColorAsInt(), Color.RED);
    });
  }
}