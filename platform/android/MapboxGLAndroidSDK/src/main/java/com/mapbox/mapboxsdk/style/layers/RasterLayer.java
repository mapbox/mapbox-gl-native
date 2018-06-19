// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.google.gson.JsonArray;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * Raster map textures such as satellite imagery.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-raster">The online documentation</a>
 */
@UiThread
public class RasterLayer extends Layer {

  /**
   * Creates a RasterLayer.
   *
   * @param nativePtr pointer used by core
   */
  public RasterLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a RasterLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public RasterLayer(String layerId, String sourceId) {
    super();
    initialize(layerId, sourceId);
  }

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
  public RasterLayer withSourceLayer(String sourceLayer) {
    setSourceLayer(sourceLayer);
    return this;
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public RasterLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the RasterOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-opacity", nativeGetRasterOpacity());
  }

  /**
   * Get the RasterOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getRasterOpacityTransition() {
    checkThread();
    return nativeGetRasterOpacityTransition();
  }

  /**
   * Set the RasterOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterOpacityTransition(TransitionOptions options) {
    checkThread();
    nativeSetRasterOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterHueRotate property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterHueRotate() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-hue-rotate", nativeGetRasterHueRotate());
  }

  /**
   * Get the RasterHueRotate property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getRasterHueRotateTransition() {
    checkThread();
    return nativeGetRasterHueRotateTransition();
  }

  /**
   * Set the RasterHueRotate property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterHueRotateTransition(TransitionOptions options) {
    checkThread();
    nativeSetRasterHueRotateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterBrightnessMin property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterBrightnessMin() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-brightness-min", nativeGetRasterBrightnessMin());
  }

  /**
   * Get the RasterBrightnessMin property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getRasterBrightnessMinTransition() {
    checkThread();
    return nativeGetRasterBrightnessMinTransition();
  }

  /**
   * Set the RasterBrightnessMin property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterBrightnessMinTransition(TransitionOptions options) {
    checkThread();
    nativeSetRasterBrightnessMinTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterBrightnessMax property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterBrightnessMax() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-brightness-max", nativeGetRasterBrightnessMax());
  }

  /**
   * Get the RasterBrightnessMax property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getRasterBrightnessMaxTransition() {
    checkThread();
    return nativeGetRasterBrightnessMaxTransition();
  }

  /**
   * Set the RasterBrightnessMax property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterBrightnessMaxTransition(TransitionOptions options) {
    checkThread();
    nativeSetRasterBrightnessMaxTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterSaturation property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterSaturation() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-saturation", nativeGetRasterSaturation());
  }

  /**
   * Get the RasterSaturation property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getRasterSaturationTransition() {
    checkThread();
    return nativeGetRasterSaturationTransition();
  }

  /**
   * Set the RasterSaturation property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterSaturationTransition(TransitionOptions options) {
    checkThread();
    nativeSetRasterSaturationTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterContrast property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterContrast() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-contrast", nativeGetRasterContrast());
  }

  /**
   * Get the RasterContrast property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getRasterContrastTransition() {
    checkThread();
    return nativeGetRasterContrastTransition();
  }

  /**
   * Set the RasterContrast property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterContrastTransition(TransitionOptions options) {
    checkThread();
    nativeSetRasterContrastTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterResampling property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getRasterResampling() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("raster-resampling", nativeGetRasterResampling());
  }

  /**
   * Get the RasterFadeDuration property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterFadeDuration() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-fade-duration", nativeGetRasterFadeDuration());
  }

  private native Object nativeGetRasterOpacity();

  private native TransitionOptions nativeGetRasterOpacityTransition();

  private native void nativeSetRasterOpacityTransition(long duration, long delay);

  private native Object nativeGetRasterHueRotate();

  private native TransitionOptions nativeGetRasterHueRotateTransition();

  private native void nativeSetRasterHueRotateTransition(long duration, long delay);

  private native Object nativeGetRasterBrightnessMin();

  private native TransitionOptions nativeGetRasterBrightnessMinTransition();

  private native void nativeSetRasterBrightnessMinTransition(long duration, long delay);

  private native Object nativeGetRasterBrightnessMax();

  private native TransitionOptions nativeGetRasterBrightnessMaxTransition();

  private native void nativeSetRasterBrightnessMaxTransition(long duration, long delay);

  private native Object nativeGetRasterSaturation();

  private native TransitionOptions nativeGetRasterSaturationTransition();

  private native void nativeSetRasterSaturationTransition(long duration, long delay);

  private native Object nativeGetRasterContrast();

  private native TransitionOptions nativeGetRasterContrastTransition();

  private native void nativeSetRasterContrastTransition(long duration, long delay);

  private native Object nativeGetRasterResampling();

  private native Object nativeGetRasterFadeDuration();

  @Override
  protected native void finalize() throws Throwable;

}
