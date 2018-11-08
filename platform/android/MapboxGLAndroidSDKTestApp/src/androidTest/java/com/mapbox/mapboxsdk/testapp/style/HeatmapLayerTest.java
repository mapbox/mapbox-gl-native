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
      if ((layer = mapboxMap.getStyle().getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new HeatmapLayer("my-layer", "composite");
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

      // Set constant
      filter = literal(true);
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
      assertNull(layer.getHeatmapRadius().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(heatmapRadius(propertyValue));
      assertEquals(layer.getHeatmapRadius().getValue(), propertyValue);
    });
  }

  @Test
  public void testHeatmapRadiusAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-radius-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getHeatmapRadius().getExpression());

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
      assertNull(layer.getHeatmapWeight().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(heatmapWeight(propertyValue));
      assertEquals(layer.getHeatmapWeight().getValue(), propertyValue);
    });
  }

  @Test
  public void testHeatmapWeightAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("heatmap-weight-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getHeatmapWeight().getExpression());

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
      assertNull(layer.getHeatmapIntensity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(heatmapIntensity(propertyValue));
      assertEquals(layer.getHeatmapIntensity().getValue(), propertyValue);
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
      assertNull(layer.getHeatmapOpacity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(heatmapOpacity(propertyValue));
      assertEquals(layer.getHeatmapOpacity().getValue(), propertyValue);
    });
  }
}
