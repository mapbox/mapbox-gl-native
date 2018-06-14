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
      if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new FillExtrusionLayer("my-layer", "composite");
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

      // Set and Get
      layer.setProperties(fillExtrusionOpacity(0.3f));
      assertEquals((Float) layer.getFillExtrusionOpacity().getValue(), (Float) 0.3f);
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

      // Set and Get
      layer.setProperties(fillExtrusionColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getFillExtrusionColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testFillExtrusionColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(fillExtrusionTranslate(new Float[] {0f, 0f}));
      assertEquals((Float[]) layer.getFillExtrusionTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
    });
  }

  @Test
  public void testFillExtrusionTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(fillExtrusionTranslateAnchor(FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP));
      assertEquals((String) layer.getFillExtrusionTranslateAnchor().getValue(), (String) FILL_EXTRUSION_TRANSLATE_ANCHOR_MAP);
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

      // Set and Get
      layer.setProperties(fillExtrusionPattern("pedestrian-polygon"));
      assertEquals((String) layer.getFillExtrusionPattern().getValue(), (String) "pedestrian-polygon");
    });
  }

  @Test
  public void testFillExtrusionPatternAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-pattern-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(fillExtrusionHeight(0.3f));
      assertEquals((Float) layer.getFillExtrusionHeight().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testFillExtrusionHeightAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-height-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(fillExtrusionBase(0.3f));
      assertEquals((Float) layer.getFillExtrusionBase().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testFillExtrusionBaseAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("fill-extrusion-base-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(fillExtrusionBase(expression));
      assertEquals(layer.getFillExtrusionBase().getExpression(), expression);
    });
  }

}
