package com.mapbox.mapboxsdk.style.layers;
// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

/**
 * The background color or pattern of the map.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-background">The online documentation</a>
 */
@UiThread
public class BackgroundLayer extends Layer {

  /**
   * Creates a BackgroundLayer.
   *
   * @param nativePtr pointer used by core
   */
  public BackgroundLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a BackgroundLayer.
   *
   * @param layerId the id of the layer
   */
  public BackgroundLayer(String layerId) {
    initialize(layerId);
  }

  protected native void initialize(String layerId);

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public BackgroundLayer withProperties(@NonNull Property<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the BackgroundColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getBackgroundColor() {
    return (PropertyValue<String>) new PropertyValue(nativeGetBackgroundColor());
  }

  /**
   * The color with which the background will be drawn.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getBackgroundColorAsInt() {
    PropertyValue<String> value = getBackgroundColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("background-color was set as a Function");
    }
  }


  /**
   * Get the BackgroundPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getBackgroundPattern() {
    return (PropertyValue<String>) new PropertyValue(nativeGetBackgroundPattern());
  }

  /**
   * Get the BackgroundOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getBackgroundOpacity() {
    return (PropertyValue<Float>) new PropertyValue(nativeGetBackgroundOpacity());
  }

  private native Object nativeGetBackgroundColor();

  private native Object nativeGetBackgroundPattern();

  private native Object nativeGetBackgroundOpacity();


  @Override
  protected native void finalize() throws Throwable;

}
