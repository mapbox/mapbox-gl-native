// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.HeatmapLayer;
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
 * Basic smoke tests for HeatmapLayer
 */
@RunWith(AndroidJUnit4.class)
public class HeatmapLayerTest extends BaseActivityTest {

  private HeatmapLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new HeatmapLayer("my-layer", "composite");
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
  public void testSourceLayer() {
    validateTestSetup();
    setupLayer();
    Timber.i("SourceLayer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Get initial
      assertEquals(layer.getSourceLayer(), "composite");

      // Set
      final String sourceLayer = "test";
      layer.setSourceLayer(sourceLayer);
      assertEquals(layer.getSourceLayer(), sourceLayer);
    });
  }

  @Test
  public void testFilter() {
    validateTestSetup();
    setupLayer();
    Timber.i("Filter");
    invoke(mapboxMap, (uiController, mapboxMap1) -> {
      assertNotNull(layer);

      // Get initial
      assertEquals(layer.getFilter(), null);

      // Set
      Expression filter = eq(get("undefined"), literal(1.0));
      layer.setFilter(filter);
      assertEquals(layer.getFilter().toString(), filter.toString());
    });
  }



  @Test
  public void testHeatmapRadiusTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radiusTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHeatmapRadiusTransition(options);
      assertEquals(layer.getHeatmapRadiusTransition(), options);
    });
  }

  @Test
  public void testHeatmapRadiusAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(heatmapRadius(0.3f));
      assertEquals((Float) layer.getHeatmapRadius().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testHeatmapRadiusAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(heatmapRadius(expression));
      assertEquals(layer.getHeatmapRadius().getExpression(), expression);
    });
  }


  @Test
  public void testHeatmapWeightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(heatmapWeight(0.3f));
      assertEquals((Float) layer.getHeatmapWeight().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testHeatmapWeightAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(heatmapWeight(expression));
      assertEquals(layer.getHeatmapWeight().getExpression(), expression);
    });
  }


  @Test
  public void testHeatmapIntensityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-intensityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHeatmapIntensityTransition(options);
      assertEquals(layer.getHeatmapIntensityTransition(), options);
    });
  }

  @Test
  public void testHeatmapIntensityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-intensity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(heatmapIntensity(0.3f));
      assertEquals((Float) layer.getHeatmapIntensity().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testHeatmapOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setHeatmapOpacityTransition(options);
      assertEquals(layer.getHeatmapOpacityTransition(), options);
    });
  }

  @Test
  public void testHeatmapOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(heatmapOpacity(0.3f));
      assertEquals((Float) layer.getHeatmapOpacity().getValue(), (Float) 0.3f);
    });
  }
}