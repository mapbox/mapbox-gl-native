// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import androidx.test.annotation.UiThreadTest;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.mapbox.mapboxsdk.maps.BaseLayerTest;
import org.junit.Before;
import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.LineLayer;

import org.junit.Test;
import org.junit.runner.RunWith;

import static com.mapbox.mapboxsdk.style.expressions.Expression.*;
import static org.junit.Assert.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * Basic smoke tests for LineLayer
 */
@RunWith(AndroidJUnit4.class)
public class LineLayerTest extends BaseLayerTest {

  private LineLayer layer;

  @Before
  @UiThreadTest
  public void beforeTest(){
    super.before();
    layer = new LineLayer("my-layer", "composite");
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
  public void testLineCapAsConstant() {
    Timber.i("line-cap");
    assertNotNull(layer);
    assertNull(layer.getLineCap().getValue());

    // Set and Get
    String propertyValue = LINE_CAP_BUTT;
    layer.setProperties(lineCap(propertyValue));
    assertEquals(layer.getLineCap().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineJoinAsConstant() {
    Timber.i("line-join");
    assertNotNull(layer);
    assertNull(layer.getLineJoin().getValue());

    // Set and Get
    String propertyValue = LINE_JOIN_BEVEL;
    layer.setProperties(lineJoin(propertyValue));
    assertEquals(layer.getLineJoin().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineJoinAsExpression() {
    Timber.i("line-join-expression");
    assertNotNull(layer);
    assertNull(layer.getLineJoin().getExpression());

    // Set and Get
    Expression expression = string(Expression.get("undefined"));
    layer.setProperties(lineJoin(expression));
    assertEquals(layer.getLineJoin().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testLineMiterLimitAsConstant() {
    Timber.i("line-miter-limit");
    assertNotNull(layer);
    assertNull(layer.getLineMiterLimit().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineMiterLimit(propertyValue));
    assertEquals(layer.getLineMiterLimit().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineRoundLimitAsConstant() {
    Timber.i("line-round-limit");
    assertNotNull(layer);
    assertNull(layer.getLineRoundLimit().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineRoundLimit(propertyValue));
    assertEquals(layer.getLineRoundLimit().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineOpacityTransition() {
    Timber.i("line-opacityTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineOpacityTransition(options);
    assertEquals(layer.getLineOpacityTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineOpacityAsConstant() {
    Timber.i("line-opacity");
    assertNotNull(layer);
    assertNull(layer.getLineOpacity().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineOpacity(propertyValue));
    assertEquals(layer.getLineOpacity().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineOpacityAsExpression() {
    Timber.i("line-opacity-expression");
    assertNotNull(layer);
    assertNull(layer.getLineOpacity().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(lineOpacity(expression));
    assertEquals(layer.getLineOpacity().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testLineColorTransition() {
    Timber.i("line-colorTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineColorTransition(options);
    assertEquals(layer.getLineColorTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineColorAsConstant() {
    Timber.i("line-color");
    assertNotNull(layer);
    assertNull(layer.getLineColor().getValue());

    // Set and Get
    String propertyValue = "rgba(255,128,0,0.7)";
    layer.setProperties(lineColor(propertyValue));
    assertEquals(layer.getLineColor().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineColorAsExpression() {
    Timber.i("line-color-expression");
    assertNotNull(layer);
    assertNull(layer.getLineColor().getExpression());

    // Set and Get
    Expression expression = toColor(Expression.get("undefined"));
    layer.setProperties(lineColor(expression));
    assertEquals(layer.getLineColor().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testLineColorAsIntConstant() {
    Timber.i("line-color");
    assertNotNull(layer);

    // Set and Get
    layer.setProperties(lineColor(Color.argb(127, 255, 127, 0)));
    assertEquals(layer.getLineColorAsInt(), Color.argb(127, 255, 127, 0));
  }

  @Test
  @UiThreadTest
  public void testLineTranslateTransition() {
    Timber.i("line-translateTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineTranslateTransition(options);
    assertEquals(layer.getLineTranslateTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineTranslateAsConstant() {
    Timber.i("line-translate");
    assertNotNull(layer);
    assertNull(layer.getLineTranslate().getValue());

    // Set and Get
    Float[] propertyValue = new Float[] {0f, 0f};
    layer.setProperties(lineTranslate(propertyValue));
    assertEquals(layer.getLineTranslate().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineTranslateAnchorAsConstant() {
    Timber.i("line-translate-anchor");
    assertNotNull(layer);
    assertNull(layer.getLineTranslateAnchor().getValue());

    // Set and Get
    String propertyValue = LINE_TRANSLATE_ANCHOR_MAP;
    layer.setProperties(lineTranslateAnchor(propertyValue));
    assertEquals(layer.getLineTranslateAnchor().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineWidthTransition() {
    Timber.i("line-widthTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineWidthTransition(options);
    assertEquals(layer.getLineWidthTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineWidthAsConstant() {
    Timber.i("line-width");
    assertNotNull(layer);
    assertNull(layer.getLineWidth().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineWidth(propertyValue));
    assertEquals(layer.getLineWidth().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineWidthAsExpression() {
    Timber.i("line-width-expression");
    assertNotNull(layer);
    assertNull(layer.getLineWidth().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(lineWidth(expression));
    assertEquals(layer.getLineWidth().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testLineGapWidthTransition() {
    Timber.i("line-gap-widthTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineGapWidthTransition(options);
    assertEquals(layer.getLineGapWidthTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineGapWidthAsConstant() {
    Timber.i("line-gap-width");
    assertNotNull(layer);
    assertNull(layer.getLineGapWidth().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineGapWidth(propertyValue));
    assertEquals(layer.getLineGapWidth().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineGapWidthAsExpression() {
    Timber.i("line-gap-width-expression");
    assertNotNull(layer);
    assertNull(layer.getLineGapWidth().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(lineGapWidth(expression));
    assertEquals(layer.getLineGapWidth().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testLineOffsetTransition() {
    Timber.i("line-offsetTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineOffsetTransition(options);
    assertEquals(layer.getLineOffsetTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineOffsetAsConstant() {
    Timber.i("line-offset");
    assertNotNull(layer);
    assertNull(layer.getLineOffset().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineOffset(propertyValue));
    assertEquals(layer.getLineOffset().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineBlurTransition() {
    Timber.i("line-blurTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineBlurTransition(options);
    assertEquals(layer.getLineBlurTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineBlurAsConstant() {
    Timber.i("line-blur");
    assertNotNull(layer);
    assertNull(layer.getLineBlur().getValue());

    // Set and Get
    Float propertyValue = 0.3f;
    layer.setProperties(lineBlur(propertyValue));
    assertEquals(layer.getLineBlur().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLineBlurAsExpression() {
    Timber.i("line-blur-expression");
    assertNotNull(layer);
    assertNull(layer.getLineBlur().getExpression());

    // Set and Get
    Expression expression = number(Expression.get("undefined"));
    layer.setProperties(lineBlur(expression));
    assertEquals(layer.getLineBlur().getExpression(), expression);
  }

  @Test
  @UiThreadTest
  public void testLineDasharrayTransition() {
    Timber.i("line-dasharrayTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLineDasharrayTransition(options);
    assertEquals(layer.getLineDasharrayTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLineDasharrayAsConstant() {
    Timber.i("line-dasharray");
    assertNotNull(layer);
    assertNull(layer.getLineDasharray().getValue());

    // Set and Get
    Float[] propertyValue = new Float[] {};
    layer.setProperties(lineDasharray(propertyValue));
    assertEquals(layer.getLineDasharray().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLinePatternTransition() {
    Timber.i("line-patternTransitionOptions");
    assertNotNull(layer);

    // Set and Get
    TransitionOptions options = new TransitionOptions(300, 100);
    layer.setLinePatternTransition(options);
    assertEquals(layer.getLinePatternTransition(), options);
  }

  @Test
  @UiThreadTest
  public void testLinePatternAsConstant() {
    Timber.i("line-pattern");
    assertNotNull(layer);
    assertNull(layer.getLinePattern().getValue());

    // Set and Get
    String propertyValue = "pedestrian-polygon";
    layer.setProperties(linePattern(propertyValue));
    assertEquals(layer.getLinePattern().getValue(), propertyValue);
  }

  @Test
  @UiThreadTest
  public void testLinePatternAsExpression() {
    Timber.i("line-pattern-expression");
    assertNotNull(layer);
    assertNull(layer.getLinePattern().getExpression());

    // Set and Get
    Expression expression = image(string(Expression.get("undefined")));
    layer.setProperties(linePattern(expression));
    assertEquals(layer.getLinePattern().getExpression(), expression);
  }
}
