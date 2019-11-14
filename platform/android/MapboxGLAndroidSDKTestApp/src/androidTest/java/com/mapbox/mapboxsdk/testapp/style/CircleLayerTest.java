// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import androidx.test.annotation.UiThreadTest;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.mapbox.mapboxsdk.maps.BaseLayerTest;
import org.junit.Before;
import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;

import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.expressions.Expression.*;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * Basic smoke tests for CircleLayer
 */
@RunWith(AndroidJUnit4.class)
public class CircleLayerTest extends BaseLayerTest {

  private CircleLayer layer;

  @Before
  @UiThreadTest
  public void beforeTest(){
    super.before();
    layer = new CircleLayer("my-layer", "composite");
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
  public void testCircleRadiusTransition() {
    Timber.i("circle-radiusTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleRadiusTransition(options);
    assertEquals(layer.getCircleRadiusTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleRadiusAsConstant() {
    Timber.i("circle-radius");
    assertNotNull(layer);
    assertNull(layer.getCircleRadius().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(circleRadius(propertyValue));
    assertEquals(layer.getCircleRadius().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleRadiusAsExpression() {
    Timber.i("circle-radius-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleRadius().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(circleRadius(expression));
    assertEquals(layer.getCircleRadius().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testCircleColorTransition() {
    Timber.i("circle-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleColorTransition(options);
    assertEquals(layer.getCircleColorTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleColorAsConstant() {
    Timber.i("circle-color");
    assertNotNull(layer);
    assertNull(layer.getCircleColor().getValue());

    // Set and Get
    String propertyValue = "rgba(255,128,0,0.7)";
    layer.setProperties(circleColor(propertyValue));
    assertEquals(layer.getCircleColor().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleColorAsExpression() {
    Timber.i("circle-color-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleColor().getExpression());

    // Set and Get
    Expression expression = toColor(Expression.get("undefined"));
    layer.setProperties(circleColor(expression));
    assertEquals(layer.getCircleColor().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testCircleColorAsIntConstant() {
    Timber.i("circle-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleColor(Color.argb(127, 255, 127, 0)));
    assertEquals(layer.getCircleColorAsInt(), Color.argb(127, 255, 127, 0));
  }

  @Test
  @UiThreadTest
  public void testCircleBlurTransition() {
    Timber.i("circle-blurTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleBlurTransition(options);
    assertEquals(layer.getCircleBlurTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleBlurAsConstant() {
    Timber.i("circle-blur");
    assertNotNull(layer);
    assertNull(layer.getCircleBlur().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(circleBlur(propertyValue));
    assertEquals(layer.getCircleBlur().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleBlurAsExpression() {
    Timber.i("circle-blur-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleBlur().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(circleBlur(expression));
    assertEquals(layer.getCircleBlur().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testCircleOpacityTransition() {
    Timber.i("circle-opacityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleOpacityTransition(options);
    assertEquals(layer.getCircleOpacityTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleOpacityAsConstant() {
    Timber.i("circle-opacity");
    assertNotNull(layer);
    assertNull(layer.getCircleOpacity().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(circleOpacity(propertyValue));
    assertEquals(layer.getCircleOpacity().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleOpacityAsExpression() {
    Timber.i("circle-opacity-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleOpacity().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(circleOpacity(expression));
    assertEquals(layer.getCircleOpacity().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testCircleTranslateTransition() {
    Timber.i("circle-translateTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleTranslateTransition(options);
    assertEquals(layer.getCircleTranslateTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleTranslateAsConstant() {
    Timber.i("circle-translate");
    assertNotNull(layer);
    assertNull(layer.getCircleTranslate().getValue());

    // Set and Get
    Float[] propertyValue = new Float[] {0f, 0f};
    layer.setProperties(circleTranslate(propertyValue));
    assertEquals(layer.getCircleTranslate().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleTranslateAnchorAsConstant() {
    Timber.i("circle-translate-anchor");
    assertNotNull(layer);
    assertNull(layer.getCircleTranslateAnchor().getValue());

    // Set and Get
    String propertyValue = CIRCLE_TRANSLATE_ANCHOR_MAP;
    layer.setProperties(circleTranslateAnchor(propertyValue));
    assertEquals(layer.getCircleTranslateAnchor().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCirclePitchScaleAsConstant() {
    Timber.i("circle-pitch-scale");
    assertNotNull(layer);
    assertNull(layer.getCirclePitchScale().getValue());

    // Set and Get
    String propertyValue = CIRCLE_PITCH_SCALE_MAP;
    layer.setProperties(circlePitchScale(propertyValue));
    assertEquals(layer.getCirclePitchScale().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCirclePitchAlignmentAsConstant() {
    Timber.i("circle-pitch-alignment");
    assertNotNull(layer);
    assertNull(layer.getCirclePitchAlignment().getValue());

    // Set and Get
    String propertyValue = CIRCLE_PITCH_ALIGNMENT_MAP;
    layer.setProperties(circlePitchAlignment(propertyValue));
    assertEquals(layer.getCirclePitchAlignment().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeWidthTransition() {
    Timber.i("circle-stroke-widthTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleStrokeWidthTransition(options);
    assertEquals(layer.getCircleStrokeWidthTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeWidthAsConstant() {
    Timber.i("circle-stroke-width");
    assertNotNull(layer);
    assertNull(layer.getCircleStrokeWidth().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(circleStrokeWidth(propertyValue));
    assertEquals(layer.getCircleStrokeWidth().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeWidthAsExpression() {
    Timber.i("circle-stroke-width-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleStrokeWidth().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(circleStrokeWidth(expression));
    assertEquals(layer.getCircleStrokeWidth().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeColorTransition() {
    Timber.i("circle-stroke-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleStrokeColorTransition(options);
    assertEquals(layer.getCircleStrokeColorTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeColorAsConstant() {
    Timber.i("circle-stroke-color");
    assertNotNull(layer);
    assertNull(layer.getCircleStrokeColor().getValue());

    // Set and Get
    String propertyValue = "rgba(255,128,0,0.7)";
    layer.setProperties(circleStrokeColor(propertyValue));
    assertEquals(layer.getCircleStrokeColor().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeColorAsExpression() {
    Timber.i("circle-stroke-color-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleStrokeColor().getExpression());

    // Set and Get
    Expression expression = toColor(Expression.get("undefined"));
    layer.setProperties(circleStrokeColor(expression));
    assertEquals(layer.getCircleStrokeColor().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeColorAsIntConstant() {
    Timber.i("circle-stroke-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(circleStrokeColor(Color.argb(127, 255, 127, 0)));
    assertEquals(layer.getCircleStrokeColorAsInt(), Color.argb(127, 255, 127, 0));
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeOpacityTransition() {
    Timber.i("circle-stroke-opacityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setCircleStrokeOpacityTransition(options);
    assertEquals(layer.getCircleStrokeOpacityTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeOpacityAsConstant() {
    Timber.i("circle-stroke-opacity");
    assertNotNull(layer);
    assertNull(layer.getCircleStrokeOpacity().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(circleStrokeOpacity(propertyValue));
    assertEquals(layer.getCircleStrokeOpacity().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testCircleStrokeOpacityAsExpression() {
    Timber.i("circle-stroke-opacity-expression");
    assertNotNull(layer);
    assertNull(layer.getCircleStrokeOpacity().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(circleStrokeOpacity(expression));
    assertEquals(layer.getCircleStrokeOpacity().getExpression(), expression);
  }
}
