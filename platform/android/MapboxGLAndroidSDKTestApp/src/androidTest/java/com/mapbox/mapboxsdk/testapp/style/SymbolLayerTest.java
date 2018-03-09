// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.testapp.style;

import android.graphics.Color;
import android.support.test.runner.AndroidJUnit4;

import timber.log.Timber;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
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
  public void testSymbolPlacementAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-placement");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(symbolPlacement(SYMBOL_PLACEMENT_POINT));
      assertEquals((String) layer.getSymbolPlacement().getValue(), (String) SYMBOL_PLACEMENT_POINT);
    });
  }

  @Test
  public void testSymbolSpacingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-spacing");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(symbolSpacing(0.3f));
      assertEquals((Float) layer.getSymbolSpacing().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testSymbolAvoidEdgesAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("symbol-avoid-edges");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(symbolAvoidEdges(true));
      assertEquals((Boolean) layer.getSymbolAvoidEdges().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testIconAllowOverlapAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-allow-overlap");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconAllowOverlap(true));
      assertEquals((Boolean) layer.getIconAllowOverlap().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testIconIgnorePlacementAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-ignore-placement");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconIgnorePlacement(true));
      assertEquals((Boolean) layer.getIconIgnorePlacement().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testIconOptionalAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-optional");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconOptional(true));
      assertEquals((Boolean) layer.getIconOptional().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testIconRotationAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-rotation-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconRotationAlignment(ICON_ROTATION_ALIGNMENT_MAP));
      assertEquals((String) layer.getIconRotationAlignment().getValue(), (String) ICON_ROTATION_ALIGNMENT_MAP);
    });
  }

  @Test
  public void testIconSizeAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-size");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconSize(0.3f));
      assertEquals((Float) layer.getIconSize().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testIconSizeAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-size-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconTextFit(ICON_TEXT_FIT_NONE));
      assertEquals((String) layer.getIconTextFit().getValue(), (String) ICON_TEXT_FIT_NONE);
    });
  }

  @Test
  public void testIconTextFitPaddingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-text-fit-padding");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconTextFitPadding(new Float[] {0f, 0f, 0f, 0f}));
      assertEquals((Float[]) layer.getIconTextFitPadding().getValue(), (Float[]) new Float[] {0f, 0f, 0f, 0f});
    });
  }

  @Test
  public void testIconImageAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-image");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconImage("undefined"));
      assertEquals((String) layer.getIconImage().getValue(), (String) "undefined");
    });
  }

  @Test
  public void testIconImageAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-image-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconRotate(0.3f));
      assertEquals((Float) layer.getIconRotate().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testIconRotateAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-rotate-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconPadding(0.3f));
      assertEquals((Float) layer.getIconPadding().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testIconKeepUprightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-keep-upright");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconKeepUpright(true));
      assertEquals((Boolean) layer.getIconKeepUpright().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testIconOffsetAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-offset");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconOffset(new Float[] {0f, 0f}));
      assertEquals((Float[]) layer.getIconOffset().getValue(), (Float[]) new Float[] {0f, 0f});
    });
  }

  @Test
  public void testIconAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconAnchor(ICON_ANCHOR_CENTER));
      assertEquals((String) layer.getIconAnchor().getValue(), (String) ICON_ANCHOR_CENTER);
    });
  }

  @Test
  public void testIconAnchorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-anchor-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconPitchAlignment(ICON_PITCH_ALIGNMENT_MAP));
      assertEquals((String) layer.getIconPitchAlignment().getValue(), (String) ICON_PITCH_ALIGNMENT_MAP);
    });
  }

  @Test
  public void testTextPitchAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-pitch-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textPitchAlignment(TEXT_PITCH_ALIGNMENT_MAP));
      assertEquals((String) layer.getTextPitchAlignment().getValue(), (String) TEXT_PITCH_ALIGNMENT_MAP);
    });
  }

  @Test
  public void testTextRotationAlignmentAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-rotation-alignment");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textRotationAlignment(TEXT_ROTATION_ALIGNMENT_MAP));
      assertEquals((String) layer.getTextRotationAlignment().getValue(), (String) TEXT_ROTATION_ALIGNMENT_MAP);
    });
  }

  @Test
  public void testTextFieldAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-field");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textField(""));
      assertEquals((String) layer.getTextField().getValue(), (String) "");
    });
  }

  @Test
  public void testTextFieldAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-field-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      Expression expression = string(Expression.get("undefined"));
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

      // Set and Get
      layer.setProperties(textFont(new String[]{"Open Sans Regular", "Arial Unicode MS Regular"}));
      assertEquals((String[]) layer.getTextFont().getValue(), (String[]) new String[]{"Open Sans Regular", "Arial Unicode MS Regular"});
    });
  }

  @Test
  public void testTextSizeAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-size");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textSize(0.3f));
      assertEquals((Float) layer.getTextSize().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextSizeAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-size-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textMaxWidth(0.3f));
      assertEquals((Float) layer.getTextMaxWidth().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextMaxWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-max-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textLineHeight(0.3f));
      assertEquals((Float) layer.getTextLineHeight().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextLetterSpacingAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-letter-spacing");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textLetterSpacing(0.3f));
      assertEquals((Float) layer.getTextLetterSpacing().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextLetterSpacingAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-letter-spacing-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textJustify(TEXT_JUSTIFY_LEFT));
      assertEquals((String) layer.getTextJustify().getValue(), (String) TEXT_JUSTIFY_LEFT);
    });
  }

  @Test
  public void testTextJustifyAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-justify-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textAnchor(TEXT_ANCHOR_CENTER));
      assertEquals((String) layer.getTextAnchor().getValue(), (String) TEXT_ANCHOR_CENTER);
    });
  }

  @Test
  public void testTextAnchorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-anchor-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textMaxAngle(0.3f));
      assertEquals((Float) layer.getTextMaxAngle().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextRotateAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-rotate");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textRotate(0.3f));
      assertEquals((Float) layer.getTextRotate().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextRotateAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-rotate-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textPadding(0.3f));
      assertEquals((Float) layer.getTextPadding().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextKeepUprightAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-keep-upright");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textKeepUpright(true));
      assertEquals((Boolean) layer.getTextKeepUpright().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testTextTransformAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-transform");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textTransform(TEXT_TRANSFORM_NONE));
      assertEquals((String) layer.getTextTransform().getValue(), (String) TEXT_TRANSFORM_NONE);
    });
  }

  @Test
  public void testTextTransformAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-transform-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textOffset(new Float[] {0f, 0f}));
      assertEquals((Float[]) layer.getTextOffset().getValue(), (Float[]) new Float[] {0f, 0f});
    });
  }

  @Test
  public void testTextAllowOverlapAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-allow-overlap");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textAllowOverlap(true));
      assertEquals((Boolean) layer.getTextAllowOverlap().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testTextIgnorePlacementAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-ignore-placement");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textIgnorePlacement(true));
      assertEquals((Boolean) layer.getTextIgnorePlacement().getValue(), (Boolean) true);
    });
  }

  @Test
  public void testTextOptionalAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-optional");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textOptional(true));
      assertEquals((Boolean) layer.getTextOptional().getValue(), (Boolean) true);
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

      // Set and Get
      layer.setProperties(iconOpacity(0.3f));
      assertEquals((Float) layer.getIconOpacity().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testIconOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getIconColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testIconColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconHaloColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getIconHaloColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testIconHaloColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconHaloWidth(0.3f));
      assertEquals((Float) layer.getIconHaloWidth().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testIconHaloWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconHaloBlur(0.3f));
      assertEquals((Float) layer.getIconHaloBlur().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testIconHaloBlurAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-halo-blur-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(iconTranslate(new Float[] {0f, 0f}));
      assertEquals((Float[]) layer.getIconTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
    });
  }

  @Test
  public void testIconTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("icon-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(iconTranslateAnchor(ICON_TRANSLATE_ANCHOR_MAP));
      assertEquals((String) layer.getIconTranslateAnchor().getValue(), (String) ICON_TRANSLATE_ANCHOR_MAP);
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

      // Set and Get
      layer.setProperties(textOpacity(0.3f));
      assertEquals((Float) layer.getTextOpacity().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextOpacityAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-opacity-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getTextColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testTextColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textHaloColor("rgba(0, 0, 0, 1)"));
      assertEquals((String) layer.getTextHaloColor().getValue(), (String) "rgba(0, 0, 0, 1)");
    });
  }

  @Test
  public void testTextHaloColorAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-color-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textHaloWidth(0.3f));
      assertEquals((Float) layer.getTextHaloWidth().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextHaloWidthAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-width-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textHaloBlur(0.3f));
      assertEquals((Float) layer.getTextHaloBlur().getValue(), (Float) 0.3f);
    });
  }

  @Test
  public void testTextHaloBlurAsExpression() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-halo-blur-expression");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

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

      // Set and Get
      layer.setProperties(textTranslate(new Float[] {0f, 0f}));
      assertEquals((Float[]) layer.getTextTranslate().getValue(), (Float[]) new Float[] {0f, 0f});
    });
  }

  @Test
  public void testTextTranslateAnchorAsConstant() {
    validateTestSetup();
    setupLayer();
    Timber.i("text-translate-anchor");
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertNotNull(layer);

      // Set and Get
      layer.setProperties(textTranslateAnchor(TEXT_TRANSLATE_ANCHOR_MAP));
      assertEquals((String) layer.getTextTranslateAnchor().getValue(), (String) TEXT_TRANSLATE_ANCHOR_MAP);
    });
  }
}