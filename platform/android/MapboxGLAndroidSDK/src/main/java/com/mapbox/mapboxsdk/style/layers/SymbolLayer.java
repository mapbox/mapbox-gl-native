// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.types.Formatted;
import com.mapbox.mapboxsdk.style.types.FormattedSection;

/**
 * An icon or a text label.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-symbol">The online documentation</a>
 */
@UiThread
public class SymbolLayer extends Layer {

  /**
   * Creates a SymbolLayer.
   *
   * @param nativePtr pointer used by core
   */
  @Keep
  SymbolLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a SymbolLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public SymbolLayer(String layerId, String sourceId) {
    super();
    initialize(layerId, sourceId);
  }

  @Keep
  protected native void initialize(String layerId, String sourceId);

  /**
   * Set the source layer.
   *
   * @param sourceLayer the source layer to set
   */
  public void setSourceLayer(String sourceLayer) {
    checkThread();
    nativeSetSourceLayer(sourceLayer);
  }

  /**
   * Set the source Layer.
   *
   * @param sourceLayer the source layer to set
   * @return This
   */
  @NonNull
  public SymbolLayer withSourceLayer(String sourceLayer) {
    setSourceLayer(sourceLayer);
    return this;
  }

  /**
   * Get the source id.
   *
   * @return id of the source
   */
  @NonNull
  public String getSourceId() {
    checkThread();
    return nativeGetSourceId();
  }

  /**
   * Get the source layer.
   *
   * @return sourceLayer the source layer to get
   */
  @NonNull
  public String getSourceLayer() {
    checkThread();
    return nativeGetSourceLayer();
  }

  /**
   * Set a single expression filter.
   *
   * @param filter the expression filter to set
   */
  public void setFilter(@NonNull Expression filter) {
    checkThread();
    nativeSetFilter(filter.toArray());
  }

  /**
   * Set a single expression filter.
   *
   * @param filter the expression filter to set
   * @return This
   */
  @NonNull
  public SymbolLayer withFilter(@NonNull Expression filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Get a single expression filter.
   *
   * @return the expression filter to get
   */
  @Nullable
  public Expression getFilter() {
    checkThread();
    JsonElement jsonElement = nativeGetFilter();
    if (jsonElement != null) {
      return Expression.Converter.convert(jsonElement);
    } else {
      return null;
    }
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  @NonNull
  public SymbolLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the SymbolPlacement property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getSymbolPlacement() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("symbol-placement", nativeGetSymbolPlacement());
  }

  /**
   * Get the SymbolSpacing property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getSymbolSpacing() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("symbol-spacing", nativeGetSymbolSpacing());
  }

  /**
   * Get the SymbolAvoidEdges property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getSymbolAvoidEdges() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("symbol-avoid-edges", nativeGetSymbolAvoidEdges());
  }

  /**
   * Get the SymbolSortKey property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getSymbolSortKey() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("symbol-sort-key", nativeGetSymbolSortKey());
  }

  /**
   * Get the SymbolZOrder property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getSymbolZOrder() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("symbol-z-order", nativeGetSymbolZOrder());
  }

  /**
   * Get the IconAllowOverlap property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconAllowOverlap() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("icon-allow-overlap", nativeGetIconAllowOverlap());
  }

  /**
   * Get the IconIgnorePlacement property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconIgnorePlacement() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("icon-ignore-placement", nativeGetIconIgnorePlacement());
  }

  /**
   * Get the IconOptional property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconOptional() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("icon-optional", nativeGetIconOptional());
  }

  /**
   * Get the IconRotationAlignment property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconRotationAlignment() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-rotation-alignment", nativeGetIconRotationAlignment());
  }

  /**
   * Get the IconSize property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconSize() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("icon-size", nativeGetIconSize());
  }

  /**
   * Get the IconTextFit property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconTextFit() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-text-fit", nativeGetIconTextFit());
  }

  /**
   * Get the IconTextFitPadding property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconTextFitPadding() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("icon-text-fit-padding", nativeGetIconTextFitPadding());
  }

  /**
   * Get the IconImage property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconImage() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-image", nativeGetIconImage());
  }

  /**
   * Get the IconRotate property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconRotate() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("icon-rotate", nativeGetIconRotate());
  }

  /**
   * Get the IconPadding property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconPadding() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("icon-padding", nativeGetIconPadding());
  }

  /**
   * Get the IconKeepUpright property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconKeepUpright() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("icon-keep-upright", nativeGetIconKeepUpright());
  }

  /**
   * Get the IconOffset property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconOffset() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("icon-offset", nativeGetIconOffset());
  }

  /**
   * Get the IconAnchor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-anchor", nativeGetIconAnchor());
  }

  /**
   * Get the IconPitchAlignment property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconPitchAlignment() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-pitch-alignment", nativeGetIconPitchAlignment());
  }

  /**
   * Get the TextPitchAlignment property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextPitchAlignment() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-pitch-alignment", nativeGetTextPitchAlignment());
  }

  /**
   * Get the TextRotationAlignment property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextRotationAlignment() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-rotation-alignment", nativeGetTextRotationAlignment());
  }

  /**
   * Get the TextField property
   *
   * @return property wrapper value around Formatted
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Formatted> getTextField() {
    checkThread();
    return (PropertyValue<Formatted>) new PropertyValue("text-field", nativeGetTextField());
  }

  /**
   * Get the TextFont property
   *
   * @return property wrapper value around String[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String[]> getTextFont() {
    checkThread();
    return (PropertyValue<String[]>) new PropertyValue("text-font", nativeGetTextFont());
  }

  /**
   * Get the TextSize property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextSize() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-size", nativeGetTextSize());
  }

  /**
   * Get the TextMaxWidth property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextMaxWidth() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-max-width", nativeGetTextMaxWidth());
  }

  /**
   * Get the TextLineHeight property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextLineHeight() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-line-height", nativeGetTextLineHeight());
  }

  /**
   * Get the TextLetterSpacing property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextLetterSpacing() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-letter-spacing", nativeGetTextLetterSpacing());
  }

  /**
   * Get the TextJustify property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextJustify() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-justify", nativeGetTextJustify());
  }

  /**
   * Get the TextRadialOffset property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextRadialOffset() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-radial-offset", nativeGetTextRadialOffset());
  }

  /**
   * Get the TextVariableAnchor property
   *
   * @return property wrapper value around String[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String[]> getTextVariableAnchor() {
    checkThread();
    return (PropertyValue<String[]>) new PropertyValue("text-variable-anchor", nativeGetTextVariableAnchor());
  }

  /**
   * Get the TextAnchor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-anchor", nativeGetTextAnchor());
  }

  /**
   * Get the TextMaxAngle property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextMaxAngle() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-max-angle", nativeGetTextMaxAngle());
  }

  /**
   * Get the TextWritingMode property
   *
   * @return property wrapper value around String[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String[]> getTextWritingMode() {
    checkThread();
    return (PropertyValue<String[]>) new PropertyValue("text-writing-mode", nativeGetTextWritingMode());
  }

  /**
   * Get the TextRotate property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextRotate() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-rotate", nativeGetTextRotate());
  }

  /**
   * Get the TextPadding property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextPadding() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-padding", nativeGetTextPadding());
  }

  /**
   * Get the TextKeepUpright property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextKeepUpright() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("text-keep-upright", nativeGetTextKeepUpright());
  }

  /**
   * Get the TextTransform property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextTransform() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-transform", nativeGetTextTransform());
  }

  /**
   * Get the TextOffset property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getTextOffset() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("text-offset", nativeGetTextOffset());
  }

  /**
   * Get the TextAllowOverlap property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextAllowOverlap() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("text-allow-overlap", nativeGetTextAllowOverlap());
  }

  /**
   * Get the TextIgnorePlacement property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextIgnorePlacement() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("text-ignore-placement", nativeGetTextIgnorePlacement());
  }

  /**
   * Get the TextOptional property
   *
   * @return property wrapper value around Boolean
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextOptional() {
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("text-optional", nativeGetTextOptional());
  }

  /**
   * Get the IconOpacity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("icon-opacity", nativeGetIconOpacity());
  }

  /**
   * Get the IconOpacity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getIconOpacityTransition() {
    checkThread();
    return nativeGetIconOpacityTransition();
  }

  /**
   * Set the IconOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setIconOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIconOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-color", nativeGetIconColor());
  }

  /**
   * The color of the icon. This can only be used with sdf icons.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getIconColorAsInt() {
    checkThread();
    PropertyValue<String> value = getIconColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("icon-color was set as a Function");
    }
  }

  /**
   * Get the IconColor property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getIconColorTransition() {
    checkThread();
    return nativeGetIconColorTransition();
  }

  /**
   * Set the IconColor property transition options
   *
   * @param options transition options for String
   */
  public void setIconColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIconColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconHaloColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconHaloColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-halo-color", nativeGetIconHaloColor());
  }

  /**
   * The color of the icon's halo. Icon halos can only be used with SDF icons.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getIconHaloColorAsInt() {
    checkThread();
    PropertyValue<String> value = getIconHaloColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("icon-halo-color was set as a Function");
    }
  }

  /**
   * Get the IconHaloColor property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getIconHaloColorTransition() {
    checkThread();
    return nativeGetIconHaloColorTransition();
  }

  /**
   * Set the IconHaloColor property transition options
   *
   * @param options transition options for String
   */
  public void setIconHaloColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIconHaloColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconHaloWidth property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconHaloWidth() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("icon-halo-width", nativeGetIconHaloWidth());
  }

  /**
   * Get the IconHaloWidth property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getIconHaloWidthTransition() {
    checkThread();
    return nativeGetIconHaloWidthTransition();
  }

  /**
   * Set the IconHaloWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setIconHaloWidthTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIconHaloWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconHaloBlur property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconHaloBlur() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("icon-halo-blur", nativeGetIconHaloBlur());
  }

  /**
   * Get the IconHaloBlur property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getIconHaloBlurTransition() {
    checkThread();
    return nativeGetIconHaloBlurTransition();
  }

  /**
   * Set the IconHaloBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setIconHaloBlurTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIconHaloBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconTranslate() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("icon-translate", nativeGetIconTranslate());
  }

  /**
   * Get the IconTranslate property transition options
   *
   * @return transition options for Float[]
   */
  @NonNull
  public TransitionOptions getIconTranslateTransition() {
    checkThread();
    return nativeGetIconTranslateTransition();
  }

  /**
   * Set the IconTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setIconTranslateTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIconTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconTranslateAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("icon-translate-anchor", nativeGetIconTranslateAnchor());
  }

  /**
   * Get the TextOpacity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-opacity", nativeGetTextOpacity());
  }

  /**
   * Get the TextOpacity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getTextOpacityTransition() {
    checkThread();
    return nativeGetTextOpacityTransition();
  }

  /**
   * Set the TextOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setTextOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetTextOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-color", nativeGetTextColor());
  }

  /**
   * The color with which the text will be drawn.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getTextColorAsInt() {
    checkThread();
    PropertyValue<String> value = getTextColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("text-color was set as a Function");
    }
  }

  /**
   * Get the TextColor property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getTextColorTransition() {
    checkThread();
    return nativeGetTextColorTransition();
  }

  /**
   * Set the TextColor property transition options
   *
   * @param options transition options for String
   */
  public void setTextColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetTextColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextHaloColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextHaloColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-halo-color", nativeGetTextHaloColor());
  }

  /**
   * The color of the text's halo, which helps it stand out from backgrounds.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getTextHaloColorAsInt() {
    checkThread();
    PropertyValue<String> value = getTextHaloColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("text-halo-color was set as a Function");
    }
  }

  /**
   * Get the TextHaloColor property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getTextHaloColorTransition() {
    checkThread();
    return nativeGetTextHaloColorTransition();
  }

  /**
   * Set the TextHaloColor property transition options
   *
   * @param options transition options for String
   */
  public void setTextHaloColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetTextHaloColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextHaloWidth property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextHaloWidth() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-halo-width", nativeGetTextHaloWidth());
  }

  /**
   * Get the TextHaloWidth property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getTextHaloWidthTransition() {
    checkThread();
    return nativeGetTextHaloWidthTransition();
  }

  /**
   * Set the TextHaloWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setTextHaloWidthTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetTextHaloWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextHaloBlur property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextHaloBlur() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("text-halo-blur", nativeGetTextHaloBlur());
  }

  /**
   * Get the TextHaloBlur property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getTextHaloBlurTransition() {
    checkThread();
    return nativeGetTextHaloBlurTransition();
  }

  /**
   * Set the TextHaloBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setTextHaloBlurTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetTextHaloBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getTextTranslate() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("text-translate", nativeGetTextTranslate());
  }

  /**
   * Get the TextTranslate property transition options
   *
   * @return transition options for Float[]
   */
  @NonNull
  public TransitionOptions getTextTranslateTransition() {
    checkThread();
    return nativeGetTextTranslateTransition();
  }

  /**
   * Set the TextTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setTextTranslateTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetTextTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextTranslateAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("text-translate-anchor", nativeGetTextTranslateAnchor());
  }

  @NonNull
  @Keep
  private native Object nativeGetSymbolPlacement();

  @NonNull
  @Keep
  private native Object nativeGetSymbolSpacing();

  @NonNull
  @Keep
  private native Object nativeGetSymbolAvoidEdges();

  @NonNull
  @Keep
  private native Object nativeGetSymbolSortKey();

  @NonNull
  @Keep
  private native Object nativeGetSymbolZOrder();

  @NonNull
  @Keep
  private native Object nativeGetIconAllowOverlap();

  @NonNull
  @Keep
  private native Object nativeGetIconIgnorePlacement();

  @NonNull
  @Keep
  private native Object nativeGetIconOptional();

  @NonNull
  @Keep
  private native Object nativeGetIconRotationAlignment();

  @NonNull
  @Keep
  private native Object nativeGetIconSize();

  @NonNull
  @Keep
  private native Object nativeGetIconTextFit();

  @NonNull
  @Keep
  private native Object nativeGetIconTextFitPadding();

  @NonNull
  @Keep
  private native Object nativeGetIconImage();

  @NonNull
  @Keep
  private native Object nativeGetIconRotate();

  @NonNull
  @Keep
  private native Object nativeGetIconPadding();

  @NonNull
  @Keep
  private native Object nativeGetIconKeepUpright();

  @NonNull
  @Keep
  private native Object nativeGetIconOffset();

  @NonNull
  @Keep
  private native Object nativeGetIconAnchor();

  @NonNull
  @Keep
  private native Object nativeGetIconPitchAlignment();

  @NonNull
  @Keep
  private native Object nativeGetTextPitchAlignment();

  @NonNull
  @Keep
  private native Object nativeGetTextRotationAlignment();

  @NonNull
  @Keep
  private native Object nativeGetTextField();

  @NonNull
  @Keep
  private native Object nativeGetTextFont();

  @NonNull
  @Keep
  private native Object nativeGetTextSize();

  @NonNull
  @Keep
  private native Object nativeGetTextMaxWidth();

  @NonNull
  @Keep
  private native Object nativeGetTextLineHeight();

  @NonNull
  @Keep
  private native Object nativeGetTextLetterSpacing();

  @NonNull
  @Keep
  private native Object nativeGetTextJustify();

  @NonNull
  @Keep
  private native Object nativeGetTextRadialOffset();

  @NonNull
  @Keep
  private native Object nativeGetTextVariableAnchor();

  @NonNull
  @Keep
  private native Object nativeGetTextAnchor();

  @NonNull
  @Keep
  private native Object nativeGetTextMaxAngle();

  @NonNull
  @Keep
  private native Object nativeGetTextWritingMode();

  @NonNull
  @Keep
  private native Object nativeGetTextRotate();

  @NonNull
  @Keep
  private native Object nativeGetTextPadding();

  @NonNull
  @Keep
  private native Object nativeGetTextKeepUpright();

  @NonNull
  @Keep
  private native Object nativeGetTextTransform();

  @NonNull
  @Keep
  private native Object nativeGetTextOffset();

  @NonNull
  @Keep
  private native Object nativeGetTextAllowOverlap();

  @NonNull
  @Keep
  private native Object nativeGetTextIgnorePlacement();

  @NonNull
  @Keep
  private native Object nativeGetTextOptional();

  @NonNull
  @Keep
  private native Object nativeGetIconOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetIconOpacityTransition();

  @Keep
  private native void nativeSetIconOpacityTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetIconColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetIconColorTransition();

  @Keep
  private native void nativeSetIconColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetIconHaloColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetIconHaloColorTransition();

  @Keep
  private native void nativeSetIconHaloColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetIconHaloWidth();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetIconHaloWidthTransition();

  @Keep
  private native void nativeSetIconHaloWidthTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetIconHaloBlur();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetIconHaloBlurTransition();

  @Keep
  private native void nativeSetIconHaloBlurTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetIconTranslate();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetIconTranslateTransition();

  @Keep
  private native void nativeSetIconTranslateTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetIconTranslateAnchor();

  @NonNull
  @Keep
  private native Object nativeGetTextOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetTextOpacityTransition();

  @Keep
  private native void nativeSetTextOpacityTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetTextColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetTextColorTransition();

  @Keep
  private native void nativeSetTextColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetTextHaloColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetTextHaloColorTransition();

  @Keep
  private native void nativeSetTextHaloColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetTextHaloWidth();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetTextHaloWidthTransition();

  @Keep
  private native void nativeSetTextHaloWidthTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetTextHaloBlur();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetTextHaloBlurTransition();

  @Keep
  private native void nativeSetTextHaloBlurTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetTextTranslate();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetTextTranslateTransition();

  @Keep
  private native void nativeSetTextTranslateTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetTextTranslateAnchor();

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
