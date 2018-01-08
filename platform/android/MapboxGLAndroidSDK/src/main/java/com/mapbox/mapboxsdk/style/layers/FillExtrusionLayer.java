// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * An extruded (3D) polygon.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-fill-extrusion">The online documentation</a>
 */
@UiThread
public class FillExtrusionLayer extends Layer {

  /**
   * Creates a FillExtrusionLayer.
   *
   * @param nativePtr pointer used by core
   */
  public FillExtrusionLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a FillExtrusionLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public FillExtrusionLayer(String layerId, String sourceId) {
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
  public FillExtrusionLayer withSourceLayer(String sourceLayer) {
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
  public FillExtrusionLayer withFilter(Filter.Statement filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public FillExtrusionLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the FillExtrusionOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillExtrusionOpacity() {
    return (PropertyValue<Float>) new PropertyValue("fill-extrusion-opacity", nativeGetFillExtrusionOpacity());
  }

  /**
   * Get the FillExtrusionOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillExtrusionOpacityTransition() {
    return nativeGetFillExtrusionOpacityTransition();
  }

  /**
   * Set the FillExtrusionOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setFillExtrusionOpacityTransition(TransitionOptions options) {
    nativeSetFillExtrusionOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillExtrusionColor() {
    return (PropertyValue<String>) new PropertyValue("fill-extrusion-color", nativeGetFillExtrusionColor());
  }

  /**
   * The base color of the extruded fill. The extrusion's surfaces will be shaded differently based on this color in combination with the root `light` settings. If this color is specified as `rgba` with an alpha component, the alpha component will be ignored; use `fill-extrusion-opacity` to set layer opacity.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getFillExtrusionColorAsInt() {
    PropertyValue<String> value = getFillExtrusionColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("fill-extrusion-color was set as a Function");
    }
  }

  /**
   * Get the FillExtrusionColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillExtrusionColorTransition() {
    return nativeGetFillExtrusionColorTransition();
  }

  /**
   * Set the FillExtrusionColor property transition options
   *
   * @param options transition options for String
   */
  public void setFillExtrusionColorTransition(TransitionOptions options) {
    nativeSetFillExtrusionColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getFillExtrusionTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue("fill-extrusion-translate", nativeGetFillExtrusionTranslate());
  }

  /**
   * Get the FillExtrusionTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getFillExtrusionTranslateTransition() {
    return nativeGetFillExtrusionTranslateTransition();
  }

  /**
   * Set the FillExtrusionTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setFillExtrusionTranslateTransition(TransitionOptions options) {
    nativeSetFillExtrusionTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillExtrusionTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue("fill-extrusion-translate-anchor", nativeGetFillExtrusionTranslateAnchor());
  }

  /**
   * Get the FillExtrusionPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillExtrusionPattern() {
    return (PropertyValue<String>) new PropertyValue("fill-extrusion-pattern", nativeGetFillExtrusionPattern());
  }

  /**
   * Get the FillExtrusionPattern property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillExtrusionPatternTransition() {
    return nativeGetFillExtrusionPatternTransition();
  }

  /**
   * Set the FillExtrusionPattern property transition options
   *
   * @param options transition options for String
   */
  public void setFillExtrusionPatternTransition(TransitionOptions options) {
    nativeSetFillExtrusionPatternTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionHeight property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillExtrusionHeight() {
    return (PropertyValue<Float>) new PropertyValue("fill-extrusion-height", nativeGetFillExtrusionHeight());
  }

  /**
   * Get the FillExtrusionHeight property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillExtrusionHeightTransition() {
    return nativeGetFillExtrusionHeightTransition();
  }

  /**
   * Set the FillExtrusionHeight property transition options
   *
   * @param options transition options for Float
   */
  public void setFillExtrusionHeightTransition(TransitionOptions options) {
    nativeSetFillExtrusionHeightTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionBase property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillExtrusionBase() {
    return (PropertyValue<Float>) new PropertyValue("fill-extrusion-base", nativeGetFillExtrusionBase());
  }

  /**
   * Get the FillExtrusionBase property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillExtrusionBaseTransition() {
    return nativeGetFillExtrusionBaseTransition();
  }

  /**
   * Set the FillExtrusionBase property transition options
   *
   * @param options transition options for Float
   */
  public void setFillExtrusionBaseTransition(TransitionOptions options) {
    nativeSetFillExtrusionBaseTransition(options.getDuration(), options.getDelay());
  }

  private native Object nativeGetFillExtrusionOpacity();

  private native TransitionOptions nativeGetFillExtrusionOpacityTransition();

  private native void nativeSetFillExtrusionOpacityTransition(long duration, long delay);

  private native Object nativeGetFillExtrusionColor();

  private native TransitionOptions nativeGetFillExtrusionColorTransition();

  private native void nativeSetFillExtrusionColorTransition(long duration, long delay);

  private native Object nativeGetFillExtrusionTranslate();

  private native TransitionOptions nativeGetFillExtrusionTranslateTransition();

  private native void nativeSetFillExtrusionTranslateTransition(long duration, long delay);

  private native Object nativeGetFillExtrusionTranslateAnchor();

  private native Object nativeGetFillExtrusionPattern();

  private native TransitionOptions nativeGetFillExtrusionPatternTransition();

  private native void nativeSetFillExtrusionPatternTransition(long duration, long delay);

  private native Object nativeGetFillExtrusionHeight();

  private native TransitionOptions nativeGetFillExtrusionHeightTransition();

  private native void nativeSetFillExtrusionHeightTransition(long duration, long delay);

  private native Object nativeGetFillExtrusionBase();

  private native TransitionOptions nativeGetFillExtrusionBaseTransition();

  private native void nativeSetFillExtrusionBaseTransition(long duration, long delay);

  @Override
  protected native void finalize() throws Throwable;

}
