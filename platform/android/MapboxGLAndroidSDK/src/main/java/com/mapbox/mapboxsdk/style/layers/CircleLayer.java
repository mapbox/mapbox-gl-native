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
  @Keep
  CircleLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a CircleLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public CircleLayer(String layerId, String sourceId) {
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
  public CircleLayer withSourceLayer(String sourceLayer) {
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
  public CircleLayer withFilter(@NonNull Expression filter) {
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
  public CircleLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the CircleRadius property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleRadius() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("circle-radius", nativeGetCircleRadius());
  }

  /**
   * Get the CircleRadius property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getCircleRadiusTransition() {
    checkThread();
    return nativeGetCircleRadiusTransition();
  }

  /**
   * Set the CircleRadius property transition options
   *
   * @param options transition options for Float
   */
  public void setCircleRadiusTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleRadiusTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCircleColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("circle-color", nativeGetCircleColor());
  }

  /**
   * The fill color of the circle.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getCircleColorAsInt() {
    checkThread();
    PropertyValue<String> value = getCircleColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("circle-color was set as a Function");
    }
  }

  /**
   * Get the CircleColor property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getCircleColorTransition() {
    checkThread();
    return nativeGetCircleColorTransition();
  }

  /**
   * Set the CircleColor property transition options
   *
   * @param options transition options for String
   */
  public void setCircleColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleBlur property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleBlur() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("circle-blur", nativeGetCircleBlur());
  }

  /**
   * Get the CircleBlur property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getCircleBlurTransition() {
    checkThread();
    return nativeGetCircleBlurTransition();
  }

  /**
   * Set the CircleBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setCircleBlurTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleOpacity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("circle-opacity", nativeGetCircleOpacity());
  }

  /**
   * Get the CircleOpacity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getCircleOpacityTransition() {
    checkThread();
    return nativeGetCircleOpacityTransition();
  }

  /**
   * Set the CircleOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setCircleOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getCircleTranslate() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("circle-translate", nativeGetCircleTranslate());
  }

  /**
   * Get the CircleTranslate property transition options
   *
   * @return transition options for Float[]
   */
  @NonNull
  public TransitionOptions getCircleTranslateTransition() {
    checkThread();
    return nativeGetCircleTranslateTransition();
  }

  /**
   * Set the CircleTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setCircleTranslateTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCircleTranslateAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("circle-translate-anchor", nativeGetCircleTranslateAnchor());
  }

  /**
   * Get the CirclePitchScale property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCirclePitchScale() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("circle-pitch-scale", nativeGetCirclePitchScale());
  }

  /**
   * Get the CirclePitchAlignment property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCirclePitchAlignment() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("circle-pitch-alignment", nativeGetCirclePitchAlignment());
  }

  /**
   * Get the CircleStrokeWidth property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleStrokeWidth() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("circle-stroke-width", nativeGetCircleStrokeWidth());
  }

  /**
   * Get the CircleStrokeWidth property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getCircleStrokeWidthTransition() {
    checkThread();
    return nativeGetCircleStrokeWidthTransition();
  }

  /**
   * Set the CircleStrokeWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setCircleStrokeWidthTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleStrokeWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleStrokeColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getCircleStrokeColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("circle-stroke-color", nativeGetCircleStrokeColor());
  }

  /**
   * The stroke color of the circle.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getCircleStrokeColorAsInt() {
    checkThread();
    PropertyValue<String> value = getCircleStrokeColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("circle-stroke-color was set as a Function");
    }
  }

  /**
   * Get the CircleStrokeColor property transition options
   *
   * @return transition options for String
   */
  @NonNull
  public TransitionOptions getCircleStrokeColorTransition() {
    checkThread();
    return nativeGetCircleStrokeColorTransition();
  }

  /**
   * Set the CircleStrokeColor property transition options
   *
   * @param options transition options for String
   */
  public void setCircleStrokeColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleStrokeColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the CircleStrokeOpacity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getCircleStrokeOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("circle-stroke-opacity", nativeGetCircleStrokeOpacity());
  }

  /**
   * Get the CircleStrokeOpacity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getCircleStrokeOpacityTransition() {
    checkThread();
    return nativeGetCircleStrokeOpacityTransition();
  }

  /**
   * Set the CircleStrokeOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setCircleStrokeOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetCircleStrokeOpacityTransition(options.getDuration(), options.getDelay());
  }

  @NonNull
  @Keep
  private native Object nativeGetCircleRadius();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleRadiusTransition();

  @Keep
  private native void nativeSetCircleRadiusTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleColorTransition();

  @Keep
  private native void nativeSetCircleColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleBlur();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleBlurTransition();

  @Keep
  private native void nativeSetCircleBlurTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleOpacityTransition();

  @Keep
  private native void nativeSetCircleOpacityTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleTranslate();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleTranslateTransition();

  @Keep
  private native void nativeSetCircleTranslateTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleTranslateAnchor();

  @NonNull
  @Keep
  private native Object nativeGetCirclePitchScale();

  @NonNull
  @Keep
  private native Object nativeGetCirclePitchAlignment();

  @NonNull
  @Keep
  private native Object nativeGetCircleStrokeWidth();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleStrokeWidthTransition();

  @Keep
  private native void nativeSetCircleStrokeWidthTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleStrokeColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleStrokeColorTransition();

  @Keep
  private native void nativeSetCircleStrokeColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetCircleStrokeOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetCircleStrokeOpacityTransition();

  @Keep
  private native void nativeSetCircleStrokeOpacityTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
