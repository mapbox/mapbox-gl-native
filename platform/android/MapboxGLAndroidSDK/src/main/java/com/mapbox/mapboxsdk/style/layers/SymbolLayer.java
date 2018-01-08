// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

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
  public SymbolLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a SymbolLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public SymbolLayer(String layerId, String sourceId) {
    initialize(layerId, sourceId);
  }

  protected native void initialize(String layerId, String sourceId);

  /**
   * Set the source layer.
   *
   * @param sourceLayer the source layer to set
   */
  public void setSourceLayer(String sourceLayer) {
    nativeSetSourceLayer(sourceLayer);
  }

  /**
   * Set the source Layer.
   *
   * @param sourceLayer the source layer to set
   * @return This
   */
  public SymbolLayer withSourceLayer(String sourceLayer) {
    setSourceLayer(sourceLayer);
    return this;
  }

  /**
   * Get the source layer.
   *
   * @return sourceLayer the source layer to get
   */
  public String getSourceLayer() {
    return nativeGetSourceLayer();
  }

  /**
   * Set a single filter.
   *
   * @param filter the filter to set
   */
  public void setFilter(Filter.Statement filter) {
    nativeSetFilter(filter.toArray());
  }

  /**
   * Set a single filter.
   *
   * @param filter the filter to set
   * @return This
   */
  public SymbolLayer withFilter(Filter.Statement filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
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
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getSymbolPlacement() {
    return (PropertyValue<String>) new PropertyValue("symbol-placement", nativeGetSymbolPlacement());
  }

  /**
   * Get the SymbolSpacing property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getSymbolSpacing() {
    return (PropertyValue<Float>) new PropertyValue("symbol-spacing", nativeGetSymbolSpacing());
  }

  /**
   * Get the SymbolAvoidEdges property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getSymbolAvoidEdges() {
    return (PropertyValue<Boolean>) new PropertyValue("symbol-avoid-edges", nativeGetSymbolAvoidEdges());
  }

  /**
   * Get the IconAllowOverlap property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconAllowOverlap() {
    return (PropertyValue<Boolean>) new PropertyValue("icon-allow-overlap", nativeGetIconAllowOverlap());
  }

  /**
   * Get the IconIgnorePlacement property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconIgnorePlacement() {
    return (PropertyValue<Boolean>) new PropertyValue("icon-ignore-placement", nativeGetIconIgnorePlacement());
  }

  /**
   * Get the IconOptional property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconOptional() {
    return (PropertyValue<Boolean>) new PropertyValue("icon-optional", nativeGetIconOptional());
  }

  /**
   * Get the IconRotationAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconRotationAlignment() {
    return (PropertyValue<String>) new PropertyValue("icon-rotation-alignment", nativeGetIconRotationAlignment());
  }

  /**
   * Get the IconSize property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconSize() {
    return (PropertyValue<Float>) new PropertyValue("icon-size", nativeGetIconSize());
  }

  /**
   * Get the IconTextFit property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconTextFit() {
    return (PropertyValue<String>) new PropertyValue("icon-text-fit", nativeGetIconTextFit());
  }

  /**
   * Get the IconTextFitPadding property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconTextFitPadding() {
    return (PropertyValue<Float[]>) new PropertyValue("icon-text-fit-padding", nativeGetIconTextFitPadding());
  }

  /**
   * Get the IconImage property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconImage() {
    return (PropertyValue<String>) new PropertyValue("icon-image", nativeGetIconImage());
  }

  /**
   * Get the IconRotate property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconRotate() {
    return (PropertyValue<Float>) new PropertyValue("icon-rotate", nativeGetIconRotate());
  }

  /**
   * Get the IconPadding property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconPadding() {
    return (PropertyValue<Float>) new PropertyValue("icon-padding", nativeGetIconPadding());
  }

  /**
   * Get the IconKeepUpright property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconKeepUpright() {
    return (PropertyValue<Boolean>) new PropertyValue("icon-keep-upright", nativeGetIconKeepUpright());
  }

  /**
   * Get the IconOffset property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconOffset() {
    return (PropertyValue<Float[]>) new PropertyValue("icon-offset", nativeGetIconOffset());
  }

  /**
   * Get the IconAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconAnchor() {
    return (PropertyValue<String>) new PropertyValue("icon-anchor", nativeGetIconAnchor());
  }

  /**
   * Get the IconPitchAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconPitchAlignment() {
    return (PropertyValue<String>) new PropertyValue("icon-pitch-alignment", nativeGetIconPitchAlignment());
  }

  /**
   * Get the TextPitchAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextPitchAlignment() {
    return (PropertyValue<String>) new PropertyValue("text-pitch-alignment", nativeGetTextPitchAlignment());
  }

  /**
   * Get the TextRotationAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextRotationAlignment() {
    return (PropertyValue<String>) new PropertyValue("text-rotation-alignment", nativeGetTextRotationAlignment());
  }

  /**
   * Get the TextField property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextField() {
    return (PropertyValue<String>) new PropertyValue("text-field", nativeGetTextField());
  }

  /**
   * Get the TextFont property
   *
   * @return property wrapper value around String[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String[]> getTextFont() {
    return (PropertyValue<String[]>) new PropertyValue("text-font", nativeGetTextFont());
  }

  /**
   * Get the TextSize property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextSize() {
    return (PropertyValue<Float>) new PropertyValue("text-size", nativeGetTextSize());
  }

  /**
   * Get the TextMaxWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextMaxWidth() {
    return (PropertyValue<Float>) new PropertyValue("text-max-width", nativeGetTextMaxWidth());
  }

  /**
   * Get the TextLineHeight property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextLineHeight() {
    return (PropertyValue<Float>) new PropertyValue("text-line-height", nativeGetTextLineHeight());
  }

  /**
   * Get the TextLetterSpacing property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextLetterSpacing() {
    return (PropertyValue<Float>) new PropertyValue("text-letter-spacing", nativeGetTextLetterSpacing());
  }

  /**
   * Get the TextJustify property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextJustify() {
    return (PropertyValue<String>) new PropertyValue("text-justify", nativeGetTextJustify());
  }

  /**
   * Get the TextAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextAnchor() {
    return (PropertyValue<String>) new PropertyValue("text-anchor", nativeGetTextAnchor());
  }

  /**
   * Get the TextMaxAngle property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextMaxAngle() {
    return (PropertyValue<Float>) new PropertyValue("text-max-angle", nativeGetTextMaxAngle());
  }

  /**
   * Get the TextRotate property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextRotate() {
    return (PropertyValue<Float>) new PropertyValue("text-rotate", nativeGetTextRotate());
  }

  /**
   * Get the TextPadding property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextPadding() {
    return (PropertyValue<Float>) new PropertyValue("text-padding", nativeGetTextPadding());
  }

  /**
   * Get the TextKeepUpright property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextKeepUpright() {
    return (PropertyValue<Boolean>) new PropertyValue("text-keep-upright", nativeGetTextKeepUpright());
  }

  /**
   * Get the TextTransform property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextTransform() {
    return (PropertyValue<String>) new PropertyValue("text-transform", nativeGetTextTransform());
  }

  /**
   * Get the TextOffset property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getTextOffset() {
    return (PropertyValue<Float[]>) new PropertyValue("text-offset", nativeGetTextOffset());
  }

  /**
   * Get the TextAllowOverlap property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextAllowOverlap() {
    return (PropertyValue<Boolean>) new PropertyValue("text-allow-overlap", nativeGetTextAllowOverlap());
  }

  /**
   * Get the TextIgnorePlacement property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextIgnorePlacement() {
    return (PropertyValue<Boolean>) new PropertyValue("text-ignore-placement", nativeGetTextIgnorePlacement());
  }

  /**
   * Get the TextOptional property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextOptional() {
    return (PropertyValue<Boolean>) new PropertyValue("text-optional", nativeGetTextOptional());
  }

  /**
   * Get the IconOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconOpacity() {
    return (PropertyValue<Float>) new PropertyValue("icon-opacity", nativeGetIconOpacity());
  }

  /**
   * Get the IconOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getIconOpacityTransition() {
    return nativeGetIconOpacityTransition();
  }

  /**
   * Set the IconOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setIconOpacityTransition(TransitionOptions options) {
    nativeSetIconOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconColor() {
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
  public TransitionOptions getIconColorTransition() {
    return nativeGetIconColorTransition();
  }

  /**
   * Set the IconColor property transition options
   *
   * @param options transition options for String
   */
  public void setIconColorTransition(TransitionOptions options) {
    nativeSetIconColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconHaloColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconHaloColor() {
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
  public TransitionOptions getIconHaloColorTransition() {
    return nativeGetIconHaloColorTransition();
  }

  /**
   * Set the IconHaloColor property transition options
   *
   * @param options transition options for String
   */
  public void setIconHaloColorTransition(TransitionOptions options) {
    nativeSetIconHaloColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconHaloWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconHaloWidth() {
    return (PropertyValue<Float>) new PropertyValue("icon-halo-width", nativeGetIconHaloWidth());
  }

  /**
   * Get the IconHaloWidth property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getIconHaloWidthTransition() {
    return nativeGetIconHaloWidthTransition();
  }

  /**
   * Set the IconHaloWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setIconHaloWidthTransition(TransitionOptions options) {
    nativeSetIconHaloWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconHaloBlur property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconHaloBlur() {
    return (PropertyValue<Float>) new PropertyValue("icon-halo-blur", nativeGetIconHaloBlur());
  }

  /**
   * Get the IconHaloBlur property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getIconHaloBlurTransition() {
    return nativeGetIconHaloBlurTransition();
  }

  /**
   * Set the IconHaloBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setIconHaloBlurTransition(TransitionOptions options) {
    nativeSetIconHaloBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue("icon-translate", nativeGetIconTranslate());
  }

  /**
   * Get the IconTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getIconTranslateTransition() {
    return nativeGetIconTranslateTransition();
  }

  /**
   * Set the IconTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setIconTranslateTransition(TransitionOptions options) {
    nativeSetIconTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the IconTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue("icon-translate-anchor", nativeGetIconTranslateAnchor());
  }

  /**
   * Get the TextOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextOpacity() {
    return (PropertyValue<Float>) new PropertyValue("text-opacity", nativeGetTextOpacity());
  }

  /**
   * Get the TextOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getTextOpacityTransition() {
    return nativeGetTextOpacityTransition();
  }

  /**
   * Set the TextOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setTextOpacityTransition(TransitionOptions options) {
    nativeSetTextOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextColor() {
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
  public TransitionOptions getTextColorTransition() {
    return nativeGetTextColorTransition();
  }

  /**
   * Set the TextColor property transition options
   *
   * @param options transition options for String
   */
  public void setTextColorTransition(TransitionOptions options) {
    nativeSetTextColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextHaloColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextHaloColor() {
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
  public TransitionOptions getTextHaloColorTransition() {
    return nativeGetTextHaloColorTransition();
  }

  /**
   * Set the TextHaloColor property transition options
   *
   * @param options transition options for String
   */
  public void setTextHaloColorTransition(TransitionOptions options) {
    nativeSetTextHaloColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextHaloWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextHaloWidth() {
    return (PropertyValue<Float>) new PropertyValue("text-halo-width", nativeGetTextHaloWidth());
  }

  /**
   * Get the TextHaloWidth property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getTextHaloWidthTransition() {
    return nativeGetTextHaloWidthTransition();
  }

  /**
   * Set the TextHaloWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setTextHaloWidthTransition(TransitionOptions options) {
    nativeSetTextHaloWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextHaloBlur property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextHaloBlur() {
    return (PropertyValue<Float>) new PropertyValue("text-halo-blur", nativeGetTextHaloBlur());
  }

  /**
   * Get the TextHaloBlur property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getTextHaloBlurTransition() {
    return nativeGetTextHaloBlurTransition();
  }

  /**
   * Set the TextHaloBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setTextHaloBlurTransition(TransitionOptions options) {
    nativeSetTextHaloBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getTextTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue("text-translate", nativeGetTextTranslate());
  }

  /**
   * Get the TextTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getTextTranslateTransition() {
    return nativeGetTextTranslateTransition();
  }

  /**
   * Set the TextTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setTextTranslateTransition(TransitionOptions options) {
    nativeSetTextTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the TextTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue("text-translate-anchor", nativeGetTextTranslateAnchor());
  }

  private native Object nativeGetSymbolPlacement();

  private native Object nativeGetSymbolSpacing();

  private native Object nativeGetSymbolAvoidEdges();

  private native Object nativeGetIconAllowOverlap();

  private native Object nativeGetIconIgnorePlacement();

  private native Object nativeGetIconOptional();

  private native Object nativeGetIconRotationAlignment();

  private native Object nativeGetIconSize();

  private native Object nativeGetIconTextFit();

  private native Object nativeGetIconTextFitPadding();

  private native Object nativeGetIconImage();

  private native Object nativeGetIconRotate();

  private native Object nativeGetIconPadding();

  private native Object nativeGetIconKeepUpright();

  private native Object nativeGetIconOffset();

  private native Object nativeGetIconAnchor();

  private native Object nativeGetIconPitchAlignment();

  private native Object nativeGetTextPitchAlignment();

  private native Object nativeGetTextRotationAlignment();

  private native Object nativeGetTextField();

  private native Object nativeGetTextFont();

  private native Object nativeGetTextSize();

  private native Object nativeGetTextMaxWidth();

  private native Object nativeGetTextLineHeight();

  private native Object nativeGetTextLetterSpacing();

  private native Object nativeGetTextJustify();

  private native Object nativeGetTextAnchor();

  private native Object nativeGetTextMaxAngle();

  private native Object nativeGetTextRotate();

  private native Object nativeGetTextPadding();

  private native Object nativeGetTextKeepUpright();

  private native Object nativeGetTextTransform();

  private native Object nativeGetTextOffset();

  private native Object nativeGetTextAllowOverlap();

  private native Object nativeGetTextIgnorePlacement();

  private native Object nativeGetTextOptional();

  private native Object nativeGetIconOpacity();

  private native TransitionOptions nativeGetIconOpacityTransition();

  private native void nativeSetIconOpacityTransition(long duration, long delay);

  private native Object nativeGetIconColor();

  private native TransitionOptions nativeGetIconColorTransition();

  private native void nativeSetIconColorTransition(long duration, long delay);

  private native Object nativeGetIconHaloColor();

  private native TransitionOptions nativeGetIconHaloColorTransition();

  private native void nativeSetIconHaloColorTransition(long duration, long delay);

  private native Object nativeGetIconHaloWidth();

  private native TransitionOptions nativeGetIconHaloWidthTransition();

  private native void nativeSetIconHaloWidthTransition(long duration, long delay);

  private native Object nativeGetIconHaloBlur();

  private native TransitionOptions nativeGetIconHaloBlurTransition();

  private native void nativeSetIconHaloBlurTransition(long duration, long delay);

  private native Object nativeGetIconTranslate();

  private native TransitionOptions nativeGetIconTranslateTransition();

  private native void nativeSetIconTranslateTransition(long duration, long delay);

  private native Object nativeGetIconTranslateAnchor();

  private native Object nativeGetTextOpacity();

  private native TransitionOptions nativeGetTextOpacityTransition();

  private native void nativeSetTextOpacityTransition(long duration, long delay);

  private native Object nativeGetTextColor();

  private native TransitionOptions nativeGetTextColorTransition();

  private native void nativeSetTextColorTransition(long duration, long delay);

  private native Object nativeGetTextHaloColor();

  private native TransitionOptions nativeGetTextHaloColorTransition();

  private native void nativeSetTextHaloColorTransition(long duration, long delay);

  private native Object nativeGetTextHaloWidth();

  private native TransitionOptions nativeGetTextHaloWidthTransition();

  private native void nativeSetTextHaloWidthTransition(long duration, long delay);

  private native Object nativeGetTextHaloBlur();

  private native TransitionOptions nativeGetTextHaloBlurTransition();

  private native void nativeSetTextHaloBlurTransition(long duration, long delay);

  private native Object nativeGetTextTranslate();

  private native TransitionOptions nativeGetTextTranslateTransition();

  private native void nativeSetTextTranslateTransition(long duration, long delay);

  private native Object nativeGetTextTranslateAnchor();

  @Override
  protected native void finalize() throws Throwable;

}
