// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.FillExtrusionLayer;
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
 * Basic smoke tests for FillExtrusionLayer
 */
@RunWith(AndroidJUnit4.class)
public class FillExtrusionLayerTest extends BaseActivityTest {

  private FillExtrusionLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getStyle().getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new FillExtrusionLayer("my-layer", "composite");
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
  public void testFillExtrusionOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillExtrusionOpacityTransition(options);
      assertEquals(layer.getFillExtrusionOpacityTransition(), options);
    });
  }

  @Test
  public void testFillExtrusionOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionOpacity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(fillExtrusionOpacity(propertyValue));
      assertEquals(layer.getFillExtrusionOpacity().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillExtrusionColorTransition(options);
      assertEquals(layer.getFillExtrusionColorTransition(), options);
    });
  }

  @Test
  public void testFillExtrusionColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(fillExtrusionColor(propertyValue));
      assertEquals(layer.getFillExtrusionColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(fillExtrusionColor(expression));
      assertEquals(layer.getFillExtrusionColor().getExpression(), expression);
    });
  }

  @Test
  public void testFillExtrusionColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(fillExtrusionColor(Color.RED));
      assertEquals(layer.getFillExtrusionColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testFillExtrusionTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-translateTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillExtrusionTranslateTransition(options);
      assertEquals(layer.getFillExtrusionTranslateTransition(), options);
    });
  }

  @Test
  public void testFillExtrusionTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-translate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionTranslate().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f};
      layer.setProperties(fillExtrusionTranslate(propertyValue));
      assertEquals(layer.getFillExtrusionTranslate().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionTranslateAnchor().getValue());

      // Set and Get
      String propertyValue = FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP;
      layer.setProperties(fillExtrusionTranslateAnchor(propertyValue));
      assertEquals(layer.getFillExtrusionTranslateAnchor().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionPatternTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-patternTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillExtrusionPatternTransition(options);
      assertEquals(layer.getFillExtrusionPatternTransition(), options);
    });
  }

  @Test
  public void testFillExtrusionPatternAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-pattern");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionPattern().getValue());

      // Set and Get
      String propertyValue = "pedestrian-polygon";
      layer.setProperties(fillExtrusionPattern(propertyValue));
      assertEquals(layer.getFillExtrusionPattern().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionPatternAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-pattern-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionPattern().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(fillExtrusionPattern(expression));
      assertEquals(layer.getFillExtrusionPattern().getExpression(), expression);
    });
  }

  @Test
  public void testFillExtrusionHeightTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-heightTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillExtrusionHeightTransition(options);
      assertEquals(layer.getFillExtrusionHeightTransition(), options);
    });
  }

  @Test
  public void testFillExtrusionHeightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-height");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionHeight().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(fillExtrusionHeight(propertyValue));
      assertEquals(layer.getFillExtrusionHeight().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionHeightAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-height-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionHeight().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(fillExtrusionHeight(expression));
      assertEquals(layer.getFillExtrusionHeight().getExpression(), expression);
    });
  }

  @Test
  public void testFillExtrusionBaseTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-baseTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setFillExtrusionBaseTransition(options);
      assertEquals(layer.getFillExtrusionBaseTransition(), options);
    });
  }

  @Test
  public void testFillExtrusionBaseAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-base");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionBase().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(fillExtrusionBase(propertyValue));
      assertEquals(layer.getFillExtrusionBase().getValue(), propertyValue);
    });
  }

  @Test
  public void testFillExtrusionBaseAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-base-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionBase().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(fillExtrusionBase(expression));
      assertEquals(layer.getFillExtrusionBase().getExpression(), expression);
    });
  }

  @Test
  public void testFillExtrusionVerticalGradientAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-vertical-gradient");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getFillExtrusionVerticalGradient().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(fillExtrusionVerticalGradient(propertyValue));
      assertEquals(layer.getFillExtrusionVerticalGradient().getValue(), propertyValue);
    });
  }
}
