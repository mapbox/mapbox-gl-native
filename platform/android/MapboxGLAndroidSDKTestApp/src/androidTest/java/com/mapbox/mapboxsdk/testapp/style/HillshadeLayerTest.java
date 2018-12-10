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
      if ((layer = mapboxMap.getStyle().getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new HillshadeLayer("my-layer", "composite");
        layer.setSourceLayer("composite");
        mapboxMap.getStyle().addLayer(layer);
        // Layer reference is now stale, get new reference
        layer = mapboxMap.getStyle().getLayerAs("my-layer");
      }
    });
  }

  @Test
  public void testSourceId() {
    validateTestSetup();
    setupLayer();
    Timber.i("SourceId");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      // Get source id
      assertEquals(layer.getSourceId(), "composite");
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
      assertNull(layer.getHillshadeIlluminationDirection().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(hillshadeIlluminationDirection(propertyValue));
      assertEquals(layer.getHillshadeIlluminationDirection().getValue(), propertyValue);
    });
  }

  @Test
  public void testHillshadeIlluminationAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("hillshade-illumination-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getHillshadeIlluminationAnchor().getValue());

      // Set and Get
      String propertyValue = HILLSHADE_ILLUMINATION_ANCHOR_MAP;
      layer.setProperties(hillshadeIlluminationAnchor(propertyValue));
      assertEquals(layer.getHillshadeIlluminationAnchor().getValue(), propertyValue);
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
      assertNull(layer.getHillshadeExaggeration().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(hillshadeExaggeration(propertyValue));
      assertEquals(layer.getHillshadeExaggeration().getValue(), propertyValue);
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
      assertNull(layer.getHillshadeShadowColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(hillshadeShadowColor(propertyValue));
      assertEquals(layer.getHillshadeShadowColor().getValue(), propertyValue);
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
      assertNull(layer.getHillshadeHighlightColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(hillshadeHighlightColor(propertyValue));
      assertEquals(layer.getHillshadeHighlightColor().getValue(), propertyValue);
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
      assertNull(layer.getHillshadeAccentColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(hillshadeAccentColor(propertyValue));
      assertEquals(layer.getHillshadeAccentColor().getValue(), propertyValue);
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
