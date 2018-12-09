// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.types.Formatted;
import com.mapbox.mapboxsdk.style.types.FormattedSection;
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
 * Basic smoke tests for SymbolLayer
 */
@RunWith(AndroidJUnit4.class)
public class SymbolLayerTest extends BaseActivityTest {

  private SymbolLayer layer;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private void setupLayer() {
    Timber.i("Retrieving layer");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      if ((layer = mapboxMap.getLayerAs("my-layer")) == null) {
        Timber.i("Adding layer");
        layer = new SymbolLayer("my-layer", "composite");
        layer.setSourceLayer("composite");
        mapboxMap.addLayer(layer);
        // Layer reference is now stale, get new reference
        layer = mapboxMap.getLayerAs("my-layer");
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
  public void testSymbolPlacementAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-placement");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getSymbolPlacement().getValue());

      // Set and Get
      String propertyValue = SYMBOL_PLACEMENT_POINT;
      layer.setProperties(symbolPlacement(propertyValue));
      assertEquals(layer.getSymbolPlacement().getValue(), propertyValue);
    });
  }

  @Test
  public void testSymbolSpacingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-spacing");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getSymbolSpacing().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(symbolSpacing(propertyValue));
      assertEquals(layer.getSymbolSpacing().getValue(), propertyValue);
    });
  }

  @Test
  public void testSymbolAvoidEdgesAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-avoid-edges");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getSymbolAvoidEdges().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(symbolAvoidEdges(propertyValue));
      assertEquals(layer.getSymbolAvoidEdges().getValue(), propertyValue);
    });
  }

  @Test
  public void testSymbolZOrderAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-z-order");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getSymbolZOrder().getValue());

      // Set and Get
      String propertyValue = SYMBOL_Z_ORDER_VIEWPORT_Y;
      layer.setProperties(symbolZOrder(propertyValue));
      assertEquals(layer.getSymbolZOrder().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconAllowOverlapAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-allow-overlap");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconAllowOverlap().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(iconAllowOverlap(propertyValue));
      assertEquals(layer.getIconAllowOverlap().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconIgnorePlacementAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-ignore-placement");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconIgnorePlacement().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(iconIgnorePlacement(propertyValue));
      assertEquals(layer.getIconIgnorePlacement().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconOptionalAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-optional");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconOptional().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(iconOptional(propertyValue));
      assertEquals(layer.getIconOptional().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconRotationAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-rotation-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconRotationAlignment().getValue());

      // Set and Get
      String propertyValue = ICON_ROTATION_ALIGNMENT_MAP;
      layer.setProperties(iconRotationAlignment(propertyValue));
      assertEquals(layer.getIconRotationAlignment().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconSizeAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-size");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconSize().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(iconSize(propertyValue));
      assertEquals(layer.getIconSize().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconSizeAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-size-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconSize().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(iconSize(expression));
      assertEquals(layer.getIconSize().getExpression(), expression);
    });
  }

  @Test
  public void testIconTextFitAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-text-fit");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconTextFit().getValue());

      // Set and Get
      String propertyValue = ICON_TEXT_FIT_NONE;
      layer.setProperties(iconTextFit(propertyValue));
      assertEquals(layer.getIconTextFit().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconTextFitPaddingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-text-fit-padding");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconTextFitPadding().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f, 0f, 0f};
      layer.setProperties(iconTextFitPadding(propertyValue));
      assertEquals(layer.getIconTextFitPadding().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconImageAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-image");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconImage().getValue());

      // Set and Get
      String propertyValue = "undefined";
      layer.setProperties(iconImage(propertyValue));
      assertEquals(layer.getIconImage().getValue(), propertyValue);

      layer.setProperties(iconImage("{token}"));
      assertEquals(layer.getIconImage().getExpression(), Expression.toString(Expression.get("token")));
    });
  }

  @Test
  public void testIconImageAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-image-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconImage().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(iconImage(expression));
      assertEquals(layer.getIconImage().getExpression(), expression);
    });
  }

  @Test
  public void testIconRotateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-rotate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconRotate().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(iconRotate(propertyValue));
      assertEquals(layer.getIconRotate().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconRotateAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-rotate-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconRotate().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(iconRotate(expression));
      assertEquals(layer.getIconRotate().getExpression(), expression);
    });
  }

  @Test
  public void testIconPaddingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-padding");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconPadding().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(iconPadding(propertyValue));
      assertEquals(layer.getIconPadding().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconKeepUprightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-keep-upright");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconKeepUpright().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(iconKeepUpright(propertyValue));
      assertEquals(layer.getIconKeepUpright().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconOffsetAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-offset");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconOffset().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f};
      layer.setProperties(iconOffset(propertyValue));
      assertEquals(layer.getIconOffset().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconAnchor().getValue());

      // Set and Get
      String propertyValue = ICON_ANCHOR_CENTER;
      layer.setProperties(iconAnchor(propertyValue));
      assertEquals(layer.getIconAnchor().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconAnchorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-anchor-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconAnchor().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(iconAnchor(expression));
      assertEquals(layer.getIconAnchor().getExpression(), expression);
    });
  }

  @Test
  public void testIconPitchAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-pitch-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconPitchAlignment().getValue());

      // Set and Get
      String propertyValue = ICON_PITCH_ALIGNMENT_MAP;
      layer.setProperties(iconPitchAlignment(propertyValue));
      assertEquals(layer.getIconPitchAlignment().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextPitchAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-pitch-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextPitchAlignment().getValue());

      // Set and Get
      String propertyValue = TEXT_PITCH_ALIGNMENT_MAP;
      layer.setProperties(textPitchAlignment(propertyValue));
      assertEquals(layer.getTextPitchAlignment().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextRotationAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-rotation-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextRotationAlignment().getValue());

      // Set and Get
      String propertyValue = TEXT_ROTATION_ALIGNMENT_MAP;
      layer.setProperties(textRotationAlignment(propertyValue));
      assertEquals(layer.getTextRotationAlignment().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextFieldAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-field");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextField().getValue());

      // Set and Get
      Formatted propertyValue = new Formatted(new FormattedSection("default"));

      layer.setProperties(textField("default"));
      assertEquals(layer.getTextField().getValue(), propertyValue);

      layer.setProperties(textField(propertyValue));
      assertEquals(layer.getTextField().getValue(), propertyValue);

      layer.setProperties(textField("{token}"));
      assertEquals(layer.getTextField().getExpression(), format(Expression.formatEntry(Expression.toString(Expression.get("token")))));
    });
  }

  @Test
  public void testTextFieldAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-field-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextField().getExpression());

      // Set and Get
      Expression expression = format(Expression.formatEntry(Expression.get("undefined"), FormatOption.formatFontScale(2.0), FormatOption.formatTextFont(new String[]{"Open Sans Regular", "Arial Unicode MS Regular"})));
      layer.setProperties(textField(expression));
      assertEquals(layer.getTextField().getExpression(), expression);
    });
  }

  @Test
  public void testTextFontAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-font");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextFont().getValue());

      // Set and Get
      String[] propertyValue = new String[]{"Open Sans Regular", "Arial Unicode MS Regular"};
      layer.setProperties(textFont(propertyValue));
      assertEquals(layer.getTextFont().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextSizeAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-size");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextSize().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textSize(propertyValue));
      assertEquals(layer.getTextSize().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextSizeAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-size-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextSize().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textSize(expression));
      assertEquals(layer.getTextSize().getExpression(), expression);
    });
  }

  @Test
  public void testTextMaxWidthAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-max-width");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextMaxWidth().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textMaxWidth(propertyValue));
      assertEquals(layer.getTextMaxWidth().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextMaxWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-max-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextMaxWidth().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textMaxWidth(expression));
      assertEquals(layer.getTextMaxWidth().getExpression(), expression);
    });
  }

  @Test
  public void testTextLineHeightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-line-height");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextLineHeight().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textLineHeight(propertyValue));
      assertEquals(layer.getTextLineHeight().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextLetterSpacingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-letter-spacing");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextLetterSpacing().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textLetterSpacing(propertyValue));
      assertEquals(layer.getTextLetterSpacing().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextLetterSpacingAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-letter-spacing-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextLetterSpacing().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textLetterSpacing(expression));
      assertEquals(layer.getTextLetterSpacing().getExpression(), expression);
    });
  }

  @Test
  public void testTextJustifyAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-justify");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextJustify().getValue());

      // Set and Get
      String propertyValue = TEXT_JUSTIFY_LEFT;
      layer.setProperties(textJustify(propertyValue));
      assertEquals(layer.getTextJustify().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextJustifyAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-justify-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextJustify().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(textJustify(expression));
      assertEquals(layer.getTextJustify().getExpression(), expression);
    });
  }

  @Test
  public void testTextAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextAnchor().getValue());

      // Set and Get
      String propertyValue = TEXT_ANCHOR_CENTER;
      layer.setProperties(textAnchor(propertyValue));
      assertEquals(layer.getTextAnchor().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextAnchorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-anchor-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextAnchor().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(textAnchor(expression));
      assertEquals(layer.getTextAnchor().getExpression(), expression);
    });
  }

  @Test
  public void testTextMaxAngleAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-max-angle");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextMaxAngle().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textMaxAngle(propertyValue));
      assertEquals(layer.getTextMaxAngle().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextRotateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-rotate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextRotate().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textRotate(propertyValue));
      assertEquals(layer.getTextRotate().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextRotateAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-rotate-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextRotate().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textRotate(expression));
      assertEquals(layer.getTextRotate().getExpression(), expression);
    });
  }

  @Test
  public void testTextPaddingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-padding");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextPadding().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textPadding(propertyValue));
      assertEquals(layer.getTextPadding().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextKeepUprightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-keep-upright");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextKeepUpright().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(textKeepUpright(propertyValue));
      assertEquals(layer.getTextKeepUpright().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextTransformAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-transform");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextTransform().getValue());

      // Set and Get
      String propertyValue = TEXT_TRANSFORM_NONE;
      layer.setProperties(textTransform(propertyValue));
      assertEquals(layer.getTextTransform().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextTransformAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-transform-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextTransform().getExpression());

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
      layer.setProperties(textTransform(expression));
      assertEquals(layer.getTextTransform().getExpression(), expression);
    });
  }

  @Test
  public void testTextOffsetAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-offset");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextOffset().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f};
      layer.setProperties(textOffset(propertyValue));
      assertEquals(layer.getTextOffset().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextAllowOverlapAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-allow-overlap");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextAllowOverlap().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(textAllowOverlap(propertyValue));
      assertEquals(layer.getTextAllowOverlap().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextIgnorePlacementAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-ignore-placement");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextIgnorePlacement().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(textIgnorePlacement(propertyValue));
      assertEquals(layer.getTextIgnorePlacement().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextOptionalAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-optional");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextOptional().getValue());

      // Set and Get
      Boolean propertyValue = true;
      layer.setProperties(textOptional(propertyValue));
      assertEquals(layer.getTextOptional().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setIconOpacityTransition(options);
      assertEquals(layer.getIconOpacityTransition(), options);
    });
  }

  @Test
  public void testIconOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconOpacity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(iconOpacity(propertyValue));
      assertEquals(layer.getIconOpacity().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconOpacity().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(iconOpacity(expression));
      assertEquals(layer.getIconOpacity().getExpression(), expression);
    });
  }

  @Test
  public void testIconColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setIconColorTransition(options);
      assertEquals(layer.getIconColorTransition(), options);
    });
  }

  @Test
  public void testIconColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(iconColor(propertyValue));
      assertEquals(layer.getIconColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(iconColor(expression));
      assertEquals(layer.getIconColor().getExpression(), expression);
    });
  }

  @Test
  public void testIconColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconColor(Color.RED));
      assertEquals(layer.getIconColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testIconHaloColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setIconHaloColorTransition(options);
      assertEquals(layer.getIconHaloColorTransition(), options);
    });
  }

  @Test
  public void testIconHaloColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconHaloColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(iconHaloColor(propertyValue));
      assertEquals(layer.getIconHaloColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconHaloColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconHaloColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(iconHaloColor(expression));
      assertEquals(layer.getIconHaloColor().getExpression(), expression);
    });
  }

  @Test
  public void testIconHaloColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconHaloColor(Color.RED));
      assertEquals(layer.getIconHaloColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testIconHaloWidthTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-widthTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setIconHaloWidthTransition(options);
      assertEquals(layer.getIconHaloWidthTransition(), options);
    });
  }

  @Test
  public void testIconHaloWidthAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-width");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconHaloWidth().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(iconHaloWidth(propertyValue));
      assertEquals(layer.getIconHaloWidth().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconHaloWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconHaloWidth().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(iconHaloWidth(expression));
      assertEquals(layer.getIconHaloWidth().getExpression(), expression);
    });
  }

  @Test
  public void testIconHaloBlurTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-blurTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setIconHaloBlurTransition(options);
      assertEquals(layer.getIconHaloBlurTransition(), options);
    });
  }

  @Test
  public void testIconHaloBlurAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-blur");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconHaloBlur().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(iconHaloBlur(propertyValue));
      assertEquals(layer.getIconHaloBlur().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconHaloBlurAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-blur-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconHaloBlur().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(iconHaloBlur(expression));
      assertEquals(layer.getIconHaloBlur().getExpression(), expression);
    });
  }

  @Test
  public void testIconTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-translateTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setIconTranslateTransition(options);
      assertEquals(layer.getIconTranslateTransition(), options);
    });
  }

  @Test
  public void testIconTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-translate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconTranslate().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f};
      layer.setProperties(iconTranslate(propertyValue));
      assertEquals(layer.getIconTranslate().getValue(), propertyValue);
    });
  }

  @Test
  public void testIconTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getIconTranslateAnchor().getValue());

      // Set and Get
      String propertyValue = ICON_TRANSLATE_ANCHOR_MAP;
      layer.setProperties(iconTranslateAnchor(propertyValue));
      assertEquals(layer.getIconTranslateAnchor().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextOpacityTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-opacityTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setTextOpacityTransition(options);
      assertEquals(layer.getTextOpacityTransition(), options);
    });
  }

  @Test
  public void testTextOpacityAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-opacity");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextOpacity().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textOpacity(propertyValue));
      assertEquals(layer.getTextOpacity().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextOpacity().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textOpacity(expression));
      assertEquals(layer.getTextOpacity().getExpression(), expression);
    });
  }

  @Test
  public void testTextColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setTextColorTransition(options);
      assertEquals(layer.getTextColorTransition(), options);
    });
  }

  @Test
  public void testTextColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(textColor(propertyValue));
      assertEquals(layer.getTextColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(textColor(expression));
      assertEquals(layer.getTextColor().getExpression(), expression);
    });
  }

  @Test
  public void testTextColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textColor(Color.RED));
      assertEquals(layer.getTextColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testTextHaloColorTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-colorTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setTextHaloColorTransition(options);
      assertEquals(layer.getTextHaloColorTransition(), options);
    });
  }

  @Test
  public void testTextHaloColorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextHaloColor().getValue());

      // Set and Get
      String propertyValue = "rgba(0, 0, 0, 1)";
      layer.setProperties(textHaloColor(propertyValue));
      assertEquals(layer.getTextHaloColor().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextHaloColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextHaloColor().getExpression());

      // Set and Get
      Expression expression = toColor(Expression.get("undefined"));
      layer.setProperties(textHaloColor(expression));
      assertEquals(layer.getTextHaloColor().getExpression(), expression);
    });
  }

  @Test
  public void testTextHaloColorAsIntConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-color");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textHaloColor(Color.RED));
      assertEquals(layer.getTextHaloColorAsInt(), Color.RED);
    });
  }

  @Test
  public void testTextHaloWidthTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-widthTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setTextHaloWidthTransition(options);
      assertEquals(layer.getTextHaloWidthTransition(), options);
    });
  }

  @Test
  public void testTextHaloWidthAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-width");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextHaloWidth().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textHaloWidth(propertyValue));
      assertEquals(layer.getTextHaloWidth().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextHaloWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextHaloWidth().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textHaloWidth(expression));
      assertEquals(layer.getTextHaloWidth().getExpression(), expression);
    });
  }

  @Test
  public void testTextHaloBlurTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-blurTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setTextHaloBlurTransition(options);
      assertEquals(layer.getTextHaloBlurTransition(), options);
    });
  }

  @Test
  public void testTextHaloBlurAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-blur");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextHaloBlur().getValue());

      // Set and Get
      Float propertyValue = 0.3f;
      layer.setProperties(textHaloBlur(propertyValue));
      assertEquals(layer.getTextHaloBlur().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextHaloBlurAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-blur-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextHaloBlur().getExpression());

      // Set and Get
      Expression expression = number(Expression.get("undefined"));
      layer.setProperties(textHaloBlur(expression));
      assertEquals(layer.getTextHaloBlur().getExpression(), expression);
    });
  }

  @Test
  public void testTextTranslateTransition() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-translateTransitionOptions");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      TransitionOptions options = new TransitionOptions(300, 100);
      layer.setTextTranslateTransition(options);
      assertEquals(layer.getTextTranslateTransition(), options);
    });
  }

  @Test
  public void testTextTranslateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-translate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextTranslate().getValue());

      // Set and Get
      Float[] propertyValue = new Float[] {0f, 0f};
      layer.setProperties(textTranslate(propertyValue));
      assertEquals(layer.getTextTranslate().getValue(), propertyValue);
    });
  }

  @Test
  public void testTextTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);
      assertNull(layer.getTextTranslateAnchor().getValue());

      // Set and Get
      String propertyValue = TEXT_TRANSLATE_ANCHOR_MAP;
      layer.setProperties(textTranslateAnchor(propertyValue));
      assertEquals(layer.getTextTranslateAnchor().getValue(), propertyValue);
    });
  }
}
