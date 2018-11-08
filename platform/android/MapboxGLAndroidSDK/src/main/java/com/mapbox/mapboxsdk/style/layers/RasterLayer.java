// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import androidx.annotation.ColorInt;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;

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
  @Keep
  RasterLayer(long nativePtr) {
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
  public RasterLayer withSourceLayer(String sourceLayer) {
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
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  @NonNull
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
  @NonNull
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
  @NonNull
  public TransitionOptions getRasterOpacityTransition() {
    checkThread();
    return nativeGetRasterOpacityTransition();
  }

  /**
   * Set the RasterOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetRasterOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterHueRotate property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getRasterHueRotateTransition() {
    checkThread();
    return nativeGetRasterHueRotateTransition();
  }

  /**
   * Set the RasterHueRotate property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterHueRotateTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetRasterHueRotateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterBrightnessMin property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getRasterBrightnessMinTransition() {
    checkThread();
    return nativeGetRasterBrightnessMinTransition();
  }

  /**
   * Set the RasterBrightnessMin property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterBrightnessMinTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetRasterBrightnessMinTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterBrightnessMax property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getRasterBrightnessMaxTransition() {
    checkThread();
    return nativeGetRasterBrightnessMaxTransition();
  }

  /**
   * Set the RasterBrightnessMax property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterBrightnessMaxTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetRasterBrightnessMaxTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterSaturation property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getRasterSaturationTransition() {
    checkThread();
    return nativeGetRasterSaturationTransition();
  }

  /**
   * Set the RasterSaturation property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterSaturationTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetRasterSaturationTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterContrast property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getRasterContrastTransition() {
    checkThread();
    return nativeGetRasterContrastTransition();
  }

  /**
   * Set the RasterContrast property transition options
   *
   * @param options transition options for Float
   */
  public void setRasterContrastTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetRasterContrastTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the RasterResampling property
   *
   * @return property wrapper value around String
   */
  @NonNull
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
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterFadeDuration() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("raster-fade-duration", nativeGetRasterFadeDuration());
  }

  @NonNull
  @Keep
  private native Object nativeGetRasterOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetRasterOpacityTransition();

  @Keep
  private native void nativeSetRasterOpacityTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetRasterHueRotate();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetRasterHueRotateTransition();

  @Keep
  private native void nativeSetRasterHueRotateTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetRasterBrightnessMin();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetRasterBrightnessMinTransition();

  @Keep
  private native void nativeSetRasterBrightnessMinTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetRasterBrightnessMax();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetRasterBrightnessMaxTransition();

  @Keep
  private native void nativeSetRasterBrightnessMaxTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetRasterSaturation();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetRasterSaturationTransition();

  @Keep
  private native void nativeSetRasterSaturationTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetRasterContrast();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetRasterContrastTransition();

  @Keep
  private native void nativeSetRasterContrastTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetRasterResampling();

  @NonNull
  @Keep
  private native Object nativeGetRasterFadeDuration();

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
