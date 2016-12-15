package com.mapbox.mapboxsdk.style.layers;
// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

/**
 * A filled polygon with an optional stroked border.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-fill">The online documentation</a>
 */
@UiThread
public class FillLayer extends Layer {

  /**
   * Creates a FillLayer.
   *
   * @param nativePtr pointer used by core
   */
  public FillLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a FillLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public FillLayer(String layerId, String sourceId) {
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
  public FillLayer withSourceLayer(String sourceLayer) {
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
  public FillLayer withFilter(Object[] filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Set a single filter.
   *
   * @param filter the filter to set
   * @return This
   */
  public FillLayer withFilter(Filter.Statement filter) {
    setFilter(filter);
    return this;
  }


  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public FillLayer withProperties(@NonNull Property<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the FillAntialias property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getFillAntialias() {
    return (PropertyValue<Boolean>) new PropertyValue(nativeGetFillAntialias());
  }

  /**
   * Get the FillOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillOpacity() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetFillOpacity());
  }

  /**
   * Get the FillColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetFillColor());
  }

  /**
   * The color of the filled part of this layer. This color can be specified as `rgba` with an alpha component and the
   * color's opacity will not affect the opacity of the 1px stroke, if it is used.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getFillColorAsInt() {
    PropertyValue<String> value = getFillColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("fill-color was set as a Function");
    }
  }


  /**
   * Get the FillOutlineColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillOutlineColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetFillOutlineColor());
  }

  /**
   * The outline color of the fill. Matches the value of `fill-color` if unspecified.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getFillOutlineColorAsInt() {
    PropertyValue<String> value = getFillOutlineColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("fill-outline-color was set as a Function");
    }
  }


  /**
   * Get the FillTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getFillTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetFillTranslate());
  }

  /**
   * Get the FillTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetFillTranslateAnchor());
  }

  /**
   * Get the FillPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillPattern() {
    return (PropertyValue<String>) new PropertyValue(nativeGetFillPattern());
  }

  private native Object nativeGetFillAntialias();

  private native Object nativeGetFillOpacity();

  private native Object nativeGetFillColor();

  private native Object nativeGetFillOutlineColor();

  private native Object nativeGetFillTranslate();

  private native Object nativeGetFillTranslateAnchor();

  private native Object nativeGetFillPattern();


  @Override
  protected native void finalize() throws Throwable;

}
