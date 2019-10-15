// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import androidx.annotation.ColorInt;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.style.expressions.Expression;
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
  @Keep
  BackgroundLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a BackgroundLayer.
   *
   * @param layerId the id of the layer
   */
  public BackgroundLayer(String layerId) {
    super();
    initialize(layerId);
  }

  @Keep
  protected native void initialize(String layerId);

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  @NonNull
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
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getBackgroundColor() {
    checkThread();
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
    checkThread();
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
  @NonNull
  public TransitionOptions getBackgroundColorTransition() {
    checkThread();
    return nativeGetBackgroundColorTransition();
  }

  /**
   * Set the BackgroundColor property transition options
   *
   * @param options transition options for String
   */
  public void setBackgroundColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetBackgroundColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the BackgroundPattern property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getBackgroundPattern() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("background-pattern", nativeGetBackgroundPattern());
  }

  /**
   * Get the BackgroundPattern property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getBackgroundPatternTransition() {
    checkThread();
    return nativeGetBackgroundPatternTransition();
  }

  /**
   * Set the BackgroundPattern property transition options
   *
   * @param options transition options for String
   */
  public void setBackgroundPatternTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetBackgroundPatternTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the BackgroundOpacity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getBackgroundOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("background-opacity", nativeGetBackgroundOpacity());
  }

  /**
   * Get the BackgroundOpacity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getBackgroundOpacityTransition() {
    checkThread();
    return nativeGetBackgroundOpacityTransition();
  }

  /**
   * Set the BackgroundOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setBackgroundOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetBackgroundOpacityTransition(options.getDuration(), options.getDelay());
  }

  @NonNull
  @Keep
  private native Object nativeGetBackgroundColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetBackgroundColorTransition();

  @Keep
  private native void nativeSetBackgroundColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetBackgroundPattern();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetBackgroundPatternTransition();

  @Keep
  private native void nativeSetBackgroundPatternTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetBackgroundOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetBackgroundOpacityTransition();

  @Keep
  private native void nativeSetBackgroundOpacityTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
