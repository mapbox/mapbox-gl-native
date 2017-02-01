// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

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
    return (PropertyValue<Float>) new PropertyValue("raster-opacity", nativeGetRasterOpacity());
  }

  /**
   * Get the RasterHueRotate property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterHueRotate() {
    return (PropertyValue<Float>) new PropertyValue("raster-hue-rotate", nativeGetRasterHueRotate());
  }

  /**
   * Get the RasterBrightnessMin property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterBrightnessMin() {
    return (PropertyValue<Float>) new PropertyValue("raster-brightness-min", nativeGetRasterBrightnessMin());
  }

  /**
   * Get the RasterBrightnessMax property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterBrightnessMax() {
    return (PropertyValue<Float>) new PropertyValue("raster-brightness-max", nativeGetRasterBrightnessMax());
  }

  /**
   * Get the RasterSaturation property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterSaturation() {
    return (PropertyValue<Float>) new PropertyValue("raster-saturation", nativeGetRasterSaturation());
  }

  /**
   * Get the RasterContrast property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterContrast() {
    return (PropertyValue<Float>) new PropertyValue("raster-contrast", nativeGetRasterContrast());
  }

  /**
   * Get the RasterFadeDuration property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getRasterFadeDuration() {
    return (PropertyValue<Float>) new PropertyValue("raster-fade-duration", nativeGetRasterFadeDuration());
  }

  private native Object nativeGetRasterOpacity();

  private native Object nativeGetRasterHueRotate();

  private native Object nativeGetRasterBrightnessMin();

  private native Object nativeGetRasterBrightnessMax();

  private native Object nativeGetRasterSaturation();

  private native Object nativeGetRasterContrast();

  private native Object nativeGetRasterFadeDuration();


  @Override
  protected native void finalize() throws Throwable;

}
