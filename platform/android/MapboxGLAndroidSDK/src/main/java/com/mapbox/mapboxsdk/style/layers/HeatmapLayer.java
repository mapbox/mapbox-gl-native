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
 * A heatmap.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-heatmap">The online documentation</a>
 */
@UiThread
public class HeatmapLayer extends Layer {

  /**
   * Creates a HeatmapLayer.
   *
   * @param nativePtr pointer used by core
   */
  @Keep
  HeatmapLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a HeatmapLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public HeatmapLayer(String layerId, String sourceId) {
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
  public HeatmapLayer withSourceLayer(String sourceLayer) {
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
  public HeatmapLayer withFilter(@NonNull Expression filter) {
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
  public HeatmapLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the HeatmapRadius property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapRadius() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("heatmap-radius", nativeGetHeatmapRadius());
  }

  /**
   * Get the HeatmapRadius property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getHeatmapRadiusTransition() {
    checkThread();
    return nativeGetHeatmapRadiusTransition();
  }

  /**
   * Set the HeatmapRadius property transition options
   *
   * @param options transition options for Float
   */
  public void setHeatmapRadiusTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetHeatmapRadiusTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HeatmapWeight property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapWeight() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("heatmap-weight", nativeGetHeatmapWeight());
  }

  /**
   * Get the HeatmapIntensity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapIntensity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("heatmap-intensity", nativeGetHeatmapIntensity());
  }

  /**
   * Get the HeatmapIntensity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getHeatmapIntensityTransition() {
    checkThread();
    return nativeGetHeatmapIntensityTransition();
  }

  /**
   * Set the HeatmapIntensity property transition options
   *
   * @param options transition options for Float
   */
  public void setHeatmapIntensityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetHeatmapIntensityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HeatmapColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getHeatmapColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("heatmap-color", nativeGetHeatmapColor());
  }

  /**
   * Defines the color of each pixel based on its density value in a heatmap.  Should be an expression that uses `["heatmap-density"]` as input.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getHeatmapColorAsInt() {
    checkThread();
    PropertyValue<String> value = getHeatmapColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("heatmap-color was set as a Function");
    }
  }

  /**
   * Get the HeatmapOpacity property
   *
   * @return property wrapper value around Float
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("heatmap-opacity", nativeGetHeatmapOpacity());
  }

  /**
   * Get the HeatmapOpacity property transition options
   *
   * @return transition options for Float
   */
  @NonNull
  public TransitionOptions getHeatmapOpacityTransition() {
    checkThread();
    return nativeGetHeatmapOpacityTransition();
  }

  /**
   * Set the HeatmapOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setHeatmapOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetHeatmapOpacityTransition(options.getDuration(), options.getDelay());
  }

  @NonNull
  @Keep
  private native Object nativeGetHeatmapRadius();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetHeatmapRadiusTransition();

  @Keep
  private native void nativeSetHeatmapRadiusTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetHeatmapWeight();

  @NonNull
  @Keep
  private native Object nativeGetHeatmapIntensity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetHeatmapIntensityTransition();

  @Keep
  private native void nativeSetHeatmapIntensityTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetHeatmapColor();

  @NonNull
  @Keep
  private native Object nativeGetHeatmapOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetHeatmapOpacityTransition();

  @Keep
  private native void nativeSetHeatmapOpacityTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
