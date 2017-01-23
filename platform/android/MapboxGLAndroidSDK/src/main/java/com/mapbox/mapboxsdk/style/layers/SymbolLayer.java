package com.mapbox.mapboxsdk.style.layers;
// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

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
   * Set a single filter.
   *
   * @param filter the filter to set
   */
  public void setFilter(Filter.Statement filter) {
    this.setFilter(filter.toArray());
  }

  /**
   * Set an array of filters.
   *
   * @param filter the filter array to set
   */
  public void setFilter(Object[] filter) {
    nativeSetFilter(filter);
  }

  /**
   * Set an array of filters.
   *
   * @param filter tthe filter array to set
   * @return This
   */
  public SymbolLayer withFilter(Object[] filter) {
    setFilter(filter);
    return this;
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
  public SymbolLayer withProperties(@NonNull Property<?>... properties) {
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
    return (PropertyValue<String>) new PropertyValue(nativeGetSymbolPlacement());
  }

  /**
   * Get the SymbolSpacing property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getSymbolSpacing() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetSymbolSpacing());
  }

  /**
   * Get the SymbolAvoidEdges property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getSymbolAvoidEdges() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetSymbolAvoidEdges());
  }

  /**
   * Get the IconAllowOverlap property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconAllowOverlap() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconAllowOverlap());
  }

  /**
   * Get the IconIgnorePlacement property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconIgnorePlacement() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconIgnorePlacement());
  }

  /**
   * Get the IconOptional property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconOptional() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconOptional());
  }

  /**
   * Get the IconRotationAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconRotationAlignment() {
    return (PropertyValue<String>) new PropertyValue(nativeGetIconRotationAlignment());
  }

  /**
   * Get the IconSize property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconSize() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetIconSize());
  }

  /**
   * Get the IconTextFit property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconTextFit() {
    return (PropertyValue<String>) new PropertyValue(nativeGetIconTextFit());
  }

  /**
   * Get the IconTextFitPadding property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconTextFitPadding() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconTextFitPadding());
  }

  /**
   * Get the IconImage property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconImage() {
    return (PropertyValue<String>) new PropertyValue(nativeGetIconImage());
  }

  /**
   * Get the IconRotate property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconRotate() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetIconRotate());
  }

  /**
   * Get the IconPadding property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconPadding() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetIconPadding());
  }

  /**
   * Get the IconKeepUpright property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getIconKeepUpright() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconKeepUpright());
  }

  /**
   * Get the IconOffset property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconOffset() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconOffset());
  }

  /**
   * Get the TextPitchAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextPitchAlignment() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextPitchAlignment());
  }

  /**
   * Get the TextRotationAlignment property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextRotationAlignment() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextRotationAlignment());
  }

  /**
   * Get the TextField property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextField() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextField());
  }

  /**
   * Get the TextFont property
   *
   * @return property wrapper value around String[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String[]> getTextFont() {
    return (PropertyValue<String[]>) new PropertyValue(nativeGetTextFont());
  }

  /**
   * Get the TextSize property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextSize() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextSize());
  }

  /**
   * Get the TextMaxWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextMaxWidth() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextMaxWidth());
  }

  /**
   * Get the TextLineHeight property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextLineHeight() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextLineHeight());
  }

  /**
   * Get the TextLetterSpacing property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextLetterSpacing() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextLetterSpacing());
  }

  /**
   * Get the TextJustify property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextJustify() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextJustify());
  }

  /**
   * Get the TextAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextAnchor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextAnchor());
  }

  /**
   * Get the TextMaxAngle property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextMaxAngle() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextMaxAngle());
  }

  /**
   * Get the TextRotate property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextRotate() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextRotate());
  }

  /**
   * Get the TextPadding property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextPadding() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextPadding());
  }

  /**
   * Get the TextKeepUpright property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextKeepUpright() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextKeepUpright());
  }

  /**
   * Get the TextTransform property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextTransform() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextTransform());
  }

  /**
   * Get the TextOffset property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getTextOffset() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetTextOffset());
  }

  /**
   * Get the TextAllowOverlap property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextAllowOverlap() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextAllowOverlap());
  }

  /**
   * Get the TextIgnorePlacement property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextIgnorePlacement() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextIgnorePlacement());
  }

  /**
   * Get the TextOptional property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getTextOptional() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextOptional());
  }

  /**
   * Get the IconOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconOpacity() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetIconOpacity());
  }

  /**
   * Get the IconColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetIconColor());
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
   * Get the IconHaloColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconHaloColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetIconHaloColor());
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
   * Get the IconHaloWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconHaloWidth() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetIconHaloWidth());
  }

  /**
   * Get the IconHaloBlur property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getIconHaloBlur() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetIconHaloBlur());
  }

  /**
   * Get the IconTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getIconTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconTranslate());
  }

  /**
   * Get the IconTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getIconTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetIconTranslateAnchor());
  }

  /**
   * Get the TextOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextOpacity() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextOpacity());
  }

  /**
   * Get the TextColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextColor());
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
   * Get the TextHaloColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextHaloColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextHaloColor());
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
   * Get the TextHaloWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextHaloWidth() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextHaloWidth());
  }

  /**
   * Get the TextHaloBlur property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getTextHaloBlur() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetTextHaloBlur());
  }

  /**
   * Get the TextTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getTextTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetTextTranslate());
  }

  /**
   * Get the TextTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getTextTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetTextTranslateAnchor());
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

  private native Object nativeGetIconColor();

  private native Object nativeGetIconHaloColor();

  private native Object nativeGetIconHaloWidth();

  private native Object nativeGetIconHaloBlur();

  private native Object nativeGetIconTranslate();

  private native Object nativeGetIconTranslateAnchor();

  private native Object nativeGetTextOpacity();

  private native Object nativeGetTextColor();

  private native Object nativeGetTextHaloColor();

  private native Object nativeGetTextHaloWidth();

  private native Object nativeGetTextHaloBlur();

  private native Object nativeGetTextTranslate();

  private native Object nativeGetTextTranslateAnchor();


  @Override
  protected native void finalize() throws Throwable;

}
