// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
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
 * Basic smoke tests for FillLayer
 */
@RunWith(AndroidJUnit4.class)
public class FillLayerTest extends BaseActivityTest {

  private FillLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getStyle().getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new FillLayer("my-layer", "composite");
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
  public void testFillAntialiasAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-antialias");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillAntialias().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(fillAntialias(propertyValue));
      assertEquals(layer.getFillAntialias().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillOpacityTransition(options);
      assertEquals(layer.getFillOpacityTransition(), options);
    });
  }

  @Test
  public void testFillOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillOpacity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(fillOpacity(propertyValue));
      assertEquals(layer.getFillOpacity().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillOpacity().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(fillOpacity(expression));
      assertEquals(layer.getFillOpacity().getExpression(), expression);
    });
  }

  @Test
  public void testFillColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillColorTransition(options);
      assertEquals(layer.getFillColorTransition(), options);
    });
  }

  @Test
  public void testFillColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(fillColor(propertyValue));
      assertEquals(layer.getFillColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(fillColor(expression));
      assertEquals(layer.getFillColor().getExpression(), expression);
    });
  }

  @Test
  public void testFillColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(fillColor(Color.RED));
      assertEquals(layer.getFillColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testFillOutlineColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillOutlineColorTransition(options);
      assertEquals(layer.getFillOutlineColorTransition(), options);
    });
  }

  @Test
  public void testFillOutlineColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillOutlineColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(fillOutlineColor(propertyValue));
      assertEquals(layer.getFillOutlineColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillOutlineColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillOutlineColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(fillOutlineColor(expression));
      assertEquals(layer.getFillOutlineColor().getExpression(), expression);
    });
  }

  @Test
  public void testFillOutlineColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-outline-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(fillOutlineColor(Color.RED));
      assertEquals(layer.getFillOutlineColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testFillTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translateTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillTranslateTransition(options);
      assertEquals(layer.getFillTranslateTransition(), options);
    });
  }

  @Test
  public void testFillTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillTranslate().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f};
      layer.setProperties(fillTranslate(propertyValue));
      assertEquals(layer.getFillTranslate().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillTranslateAnchor().getValue());

      // Set and Get
      String propertyValue = FILL_TRANSLATE_ANCHOR_MAP;
      layer.setProperties(fillTranslateAnchor(propertyValue));
      assertEquals(layer.getFillTranslateAnchor().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillPatternTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-patternTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillPatternTransition(options);
      assertEquals(layer.getFillPatternTransition(), options);
    });
  }

  @Test
  public void testFillPatternAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-pattern");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillPattern().getValue());

      // Set and Get
      String propertyValue = "pedestrian-polygon";
      layer.setProperties(fillPattern(propertyValue));
      assertEquals(layer.getFillPattern().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillPatternAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-pattern-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillPattern().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(fillPattern(expression));
      assertEquals(layer.getFillPattern().getExpression(), expression);
    });
  }
}
