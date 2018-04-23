// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
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
 * Basic smoke tests for CircleLayer
 */
@RunWith(AndroidJUnit4.class)
public class CircleLayerTest extends BaseActivityTest {

  private CircleLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new CircleLayer("my-layer", "composite");
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
  public void testCircleRadiusTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-radiusTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleRadiusTransition(options);
      assertEquals(layer.getCircleRadiusTransition(), options);
    });
  }

  @Test
  public void testCircleRadiusAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-radius");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleRadius(0.3f));
      assertEquals((Float) layer.getCircleRadius().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testCircleRadiusAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-radius-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(circleRadius(expression));
      assertEquals(layer.getCircleRadius().getExpression(), expression);
    });
  }


  @Test
  public void testCircleColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleColorTransition(options);
      assertEquals(layer.getCircleColorTransition(), options);
    });
  }

  @Test
  public void testCircleColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getCircleColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testCircleColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(circleColor(expression));
      assertEquals(layer.getCircleColor().getExpression(), expression);
    });
  }


  @Test
  public void testCircleColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleColor(Color.RED));
      assertEquals(layer.getCircleColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testCircleBlurTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-blurTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleBlurTransition(options);
      assertEquals(layer.getCircleBlurTransition(), options);
    });
  }

  @Test
  public void testCircleBlurAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-blur");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleBlur(0.3f));
      assertEquals((Float) layer.getCircleBlur().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testCircleBlurAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-blur-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(circleBlur(expression));
      assertEquals(layer.getCircleBlur().getExpression(), expression);
    });
  }


  @Test
  public void testCircleOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleOpacityTransition(options);
      assertEquals(layer.getCircleOpacityTransition(), options);
    });
  }

  @Test
  public void testCircleOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleOpacity(0.3f));
      assertEquals((Float) layer.getCircleOpacity().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testCircleOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(circleOpacity(expression));
      assertEquals(layer.getCircleOpacity().getExpression(), expression);
    });
  }


  @Test
  public void testCircleTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-translateTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleTranslateTransition(options);
      assertEquals(layer.getCircleTranslateTransition(), options);
    });
  }

  @Test
  public void testCircleTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-translate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleTranslate(new Float[] {0f, 0f}));
      assertEquals((Float[]) layer.getCircleTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
    });
  }

  @Test
  public void testCircleTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleTranslateAnchor(CIRCLE_TRANSLATE_ANCHOR_MAP));
      assertEquals((String) layer.getCircleTranslateAnchor().getValue(), (String) CIRCLE_TRANSLATE_ANCHOR_MAP);
    });
  }

  @Test
  public void testCirclePitchScaleAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-pitch-scale");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circlePitchScale(CIRCLE_PITCH_SCALE_MAP));
      assertEquals((String) layer.getCirclePitchScale().getValue(), (String) CIRCLE_PITCH_SCALE_MAP);
    });
  }

  @Test
  public void testCirclePitchAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-pitch-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circlePitchAlignment(CIRCLE_PITCH_ALIGNMENT_MAP));
      assertEquals((String) layer.getCirclePitchAlignment().getValue(), (String) CIRCLE_PITCH_ALIGNMENT_MAP);
    });
  }

  @Test
  public void testCircleStrokeWidthTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-widthTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleStrokeWidthTransition(options);
      assertEquals(layer.getCircleStrokeWidthTransition(), options);
    });
  }

  @Test
  public void testCircleStrokeWidthAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-width");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleStrokeWidth(0.3f));
      assertEquals((Float) layer.getCircleStrokeWidth().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testCircleStrokeWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(circleStrokeWidth(expression));
      assertEquals(layer.getCircleStrokeWidth().getExpression(), expression);
    });
  }


  @Test
  public void testCircleStrokeColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleStrokeColorTransition(options);
      assertEquals(layer.getCircleStrokeColorTransition(), options);
    });
  }

  @Test
  public void testCircleStrokeColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleStrokeColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getCircleStrokeColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testCircleStrokeColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(circleStrokeColor(expression));
      assertEquals(layer.getCircleStrokeColor().getExpression(), expression);
    });
  }


  @Test
  public void testCircleStrokeColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleStrokeColor(Color.RED));
      assertEquals(layer.getCircleStrokeColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testCircleStrokeOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setCircleStrokeOpacityTransition(options);
      assertEquals(layer.getCircleStrokeOpacityTransition(), options);
    });
  }

  @Test
  public void testCircleStrokeOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(circleStrokeOpacity(0.3f));
      assertEquals((Float) layer.getCircleStrokeOpacity().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testCircleStrokeOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("circle-stroke-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(circleStrokeOpacity(expression));
      assertEquals(layer.getCircleStrokeOpacity().getExpression(), expression);
    });
  }

}