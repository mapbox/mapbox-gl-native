// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

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
  public BackgroundLayer withProperties(@NonNull PropertyValue<?>... properties) {
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
    return (PropertyValue<String>) new PropertyValue("background-color", nativeGetBackgroundColor());
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
   * Get the BackgroundColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getBackgroundColorTransition() {
    return nativeGetBackgroundColorTransition();
  }

  /**
   * Set the BackgroundColor property transition options
   *
   * @param options transition options for String
   */
  public void setBackgroundColorTransition(TransitionOptions options) {
    nativeSetBackgroundColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the BackgroundPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getBackgroundPattern() {
    return (PropertyValue<String>) new PropertyValue("background-pattern", nativeGetBackgroundPattern());
  }

  /**
   * Get the BackgroundPattern property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getBackgroundPatternTransition() {
    return nativeGetBackgroundPatternTransition();
  }

  /**
   * Set the BackgroundPattern property transition options
   *
   * @param options transition options for String
   */
  public void setBackgroundPatternTransition(TransitionOptions options) {
    nativeSetBackgroundPatternTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the BackgroundOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getBackgroundOpacity() {
    return (PropertyValue<Float>) new PropertyValue("background-opacity", nativeGetBackgroundOpacity());
  }

  /**
   * Get the BackgroundOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getBackgroundOpacityTransition() {
    return nativeGetBackgroundOpacityTransition();
  }

  /**
   * Set the BackgroundOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setBackgroundOpacityTransition(TransitionOptions options) {
    nativeSetBackgroundOpacityTransition(options.getDuration(), options.getDelay());
  }

  private native Object nativeGetBackgroundColor();

  private native TransitionOptions nativeGetBackgroundColorTransition();

  private native void nativeSetBackgroundColorTransition(long duration, long delay);

  private native Object nativeGetBackgroundPattern();

  private native TransitionOptions nativeGetBackgroundPatternTransition();

  private native void nativeSetBackgroundPatternTransition(long duration, long delay);

  private native Object nativeGetBackgroundOpacity();

  private native TransitionOptions nativeGetBackgroundOpacityTransition();

  private native void nativeSetBackgroundOpacityTransition(long duration, long delay);

  @Override
  protected native void finalize() throws Throwable;

}
