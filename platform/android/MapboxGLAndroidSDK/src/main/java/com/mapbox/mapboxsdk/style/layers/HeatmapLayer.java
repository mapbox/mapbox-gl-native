// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

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
  public HeatmapLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a HeatmapLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public HeatmapLayer(String layerId, String sourceId) {
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
  public HeatmapLayer withSourceLayer(String sourceLayer) {
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
  public HeatmapLayer withFilter(Filter.Statement filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
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
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapRadius() {
    return (PropertyValue<Float>) new PropertyValue("heatmap-radius", nativeGetHeatmapRadius());
  }

  /**
   * Get the HeatmapRadius property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getHeatmapRadiusTransition() {
    return nativeGetHeatmapRadiusTransition();
  }

  /**
   * Set the HeatmapRadius property transition options
   *
   * @param options transition options for Float
   */
  public void setHeatmapRadiusTransition(TransitionOptions options) {
    nativeSetHeatmapRadiusTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HeatmapWeight property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapWeight() {
    return (PropertyValue<Float>) new PropertyValue("heatmap-weight", nativeGetHeatmapWeight());
  }

  /**
   * Get the HeatmapIntensity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapIntensity() {
    return (PropertyValue<Float>) new PropertyValue("heatmap-intensity", nativeGetHeatmapIntensity());
  }

  /**
   * Get the HeatmapIntensity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getHeatmapIntensityTransition() {
    return nativeGetHeatmapIntensityTransition();
  }

  /**
   * Set the HeatmapIntensity property transition options
   *
   * @param options transition options for Float
   */
  public void setHeatmapIntensityTransition(TransitionOptions options) {
    nativeSetHeatmapIntensityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HeatmapColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getHeatmapColor() {
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
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHeatmapOpacity() {
    return (PropertyValue<Float>) new PropertyValue("heatmap-opacity", nativeGetHeatmapOpacity());
  }

  /**
   * Get the HeatmapOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getHeatmapOpacityTransition() {
    return nativeGetHeatmapOpacityTransition();
  }

  /**
   * Set the HeatmapOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setHeatmapOpacityTransition(TransitionOptions options) {
    nativeSetHeatmapOpacityTransition(options.getDuration(), options.getDelay());
  }

  private native Object nativeGetHeatmapRadius();

  private native TransitionOptions nativeGetHeatmapRadiusTransition();

  private native void nativeSetHeatmapRadiusTransition(long duration, long delay);

  private native Object nativeGetHeatmapWeight();

  private native Object nativeGetHeatmapIntensity();

  private native TransitionOptions nativeGetHeatmapIntensityTransition();

  private native void nativeSetHeatmapIntensityTransition(long duration, long delay);

  private native Object nativeGetHeatmapColor();

  private native Object nativeGetHeatmapOpacity();

  private native TransitionOptions nativeGetHeatmapOpacityTransition();

  private native void nativeSetHeatmapOpacityTransition(long duration, long delay);

  @Override
  protected native void finalize() throws Throwable;

}
