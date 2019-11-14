// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import androidx.test.annotation.UiThreadTest;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.mapbox.mapboxsdk.maps.BaseLayerTest;
import org.junit.Before;
import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.HeatmapLayer;

import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.expressions.Expression.*;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * Basic smoke tests for HeatmapLayer
 */
@RunWith(AndroidJUnit4.class)
public class HeatmapLayerTest extends BaseLayerTest {

  private HeatmapLayer layer;

  @Before
  @UiThreadTest
  public void beforeTest(){
    super.before();
    layer = new HeatmapLayer("my-layer", "composite");
    layer.setSourceLayer("composite");
    setupLayer(layer);
  }

  @Test
  @UiThreadTest
  public void testSourceId() {
    Timber.i("SourceId");
    assertNotNull(layer);
    assertEquals(layer.getSourceId(), "composite");
  }

  @Test
  @UiThreadTest
  public void testSetVisibility() {
    Timber.i("Visibility");
    assertNotNull(layer);

    // Get initial
    assertEquals(layer.getVisibility().getValue(), VISIBLE);

    // Set
    layer.setProperties(visibility(NONE));
    assertEquals(layer.getVisibility().getValue(), NONE);
  }

  @Test
  @UiThreadTest
  public void testSourceLayer() {
    Timber.i("SourceLayer");
    assertNotNull(layer);

    // Get initial
    assertEquals(layer.getSourceLayer(), "composite");

    // Set
    final String sourceLayer = "test";
    layer.setSourceLayer(sourceLayer);
    assertEquals(layer.getSourceLayer(), sourceLayer);
  }

  @Test
  @UiThreadTest
  public void testFilter() {
    Timber.i("Filter");
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
  }



  @Test
  @UiThreadTest
  public void testHeatmapRadiusTransition() {
    Timber.i("heatmap-radiusTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setHeatmapRadiusTransition(options);
    assertEquals(layer.getHeatmapRadiusTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testHeatmapRadiusAsConstant() {
    Timber.i("heatmap-radius");
    assertNotNull(layer);
    assertNull(layer.getHeatmapRadius().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(heatmapRadius(propertyValue));
    assertEquals(layer.getHeatmapRadius().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testHeatmapRadiusAsExpression() {
    Timber.i("heatmap-radius-expression");
    assertNotNull(layer);
    assertNull(layer.getHeatmapRadius().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(heatmapRadius(expression));
    assertEquals(layer.getHeatmapRadius().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testHeatmapWeightAsConstant() {
    Timber.i("heatmap-weight");
    assertNotNull(layer);
    assertNull(layer.getHeatmapWeight().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(heatmapWeight(propertyValue));
    assertEquals(layer.getHeatmapWeight().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testHeatmapWeightAsExpression() {
    Timber.i("heatmap-weight-expression");
    assertNotNull(layer);
    assertNull(layer.getHeatmapWeight().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(heatmapWeight(expression));
    assertEquals(layer.getHeatmapWeight().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testHeatmapIntensityTransition() {
    Timber.i("heatmap-intensityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setHeatmapIntensityTransition(options);
    assertEquals(layer.getHeatmapIntensityTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testHeatmapIntensityAsConstant() {
    Timber.i("heatmap-intensity");
    assertNotNull(layer);
    assertNull(layer.getHeatmapIntensity().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(heatmapIntensity(propertyValue));
    assertEquals(layer.getHeatmapIntensity().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testHeatmapOpacityTransition() {
    Timber.i("heatmap-opacityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setHeatmapOpacityTransition(options);
    assertEquals(layer.getHeatmapOpacityTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testHeatmapOpacityAsConstant() {
    Timber.i("heatmap-opacity");
    assertNotNull(layer);
    assertNull(layer.getHeatmapOpacity().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(heatmapOpacity(propertyValue));
    assertEquals(layer.getHeatmapOpacity().getValue(), propertyValue);
  }
}
