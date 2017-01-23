package com.mapbox.mapboxsdk.style.layers;
// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

/**
 * A filled circle.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-circle">The online documentation</a>
 */
@UiThread
public class CircleLayer extends Layer {

  /**
   * Creates a CircleLayer.
   *
   * @param nativePtr pointer used by core
   */
  public CircleLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a CircleLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public CircleLayer(String layerId, String sourceId) {
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
  public CircleLayer withSourceLayer(String sourceLayer) {
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
  public CircleLayer withFilter(Object[] filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Set a single filter.
   *
   * @param filter the filter to set
   * @return This
   */
  public CircleLayer withFilter(Filter.Statement filter) {
    setFilter(filter);
    return this;
  }


  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public CircleLayer withProperties(@NonNull Property<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the CircleRadius property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleRadius() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetCircleRadius());
  }

  /**
   * Get the CircleColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCircleColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetCircleColor());
  }

  /**
   * The fill color of the circle.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getCircleColorAsInt() {
    PropertyValue<String> value = getCircleColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("circle-color was set as a Function");
    }
  }


  /**
   * Get the CircleBlur property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleBlur() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetCircleBlur());
  }

  /**
   * Get the CircleOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleOpacity() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetCircleOpacity());
  }

  /**
   * Get the CircleTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getCircleTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue(nativeGetCircleTranslate());
  }

  /**
   * Get the CircleTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCircleTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetCircleTranslateAnchor());
  }

  /**
   * Get the CirclePitchScale property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCirclePitchScale() {
    return (PropertyValue<String>) new PropertyValue(nativeGetCirclePitchScale());
  }

  /**
   * Get the CircleStrokeWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleStrokeWidth() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetCircleStrokeWidth());
  }

  /**
   * Get the CircleStrokeColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCircleStrokeColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetCircleStrokeColor());
  }

  /**
   * The stroke color of the circle.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getCircleStrokeColorAsInt() {
    PropertyValue<String> value = getCircleStrokeColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("circle-stroke-color was set as a Function");
    }
  }


  /**
   * Get the CircleStrokeOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleStrokeOpacity() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetCircleStrokeOpacity());
  }

  private native Object nativeGetCircleRadius();

  private native Object nativeGetCircleColor();

  private native Object nativeGetCircleBlur();

  private native Object nativeGetCircleOpacity();

  private native Object nativeGetCircleTranslate();

  private native Object nativeGetCircleTranslateAnchor();

  private native Object nativeGetCirclePitchScale();

  private native Object nativeGetCircleStrokeWidth();

  private native Object nativeGetCircleStrokeColor();

  private native Object nativeGetCircleStrokeOpacity();


  @Override
  protected native void finalize() throws Throwable;

}
