// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * A filled polygon with an optional stroked border.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-fill">The online documentation</a>
 */
@UiThread
public class FillLayer extends Layer {

  /**
   * Creates a FillLayer.
   *
   * @param nativePtr pointer used by core
   */
  public FillLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a FillLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public FillLayer(String layerId, String sourceId) {
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
  public FillLayer withSourceLayer(String sourceLayer) {
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
  public FillLayer withFilter(Filter.Statement filter) {
    setFilter(filter);
    return this;
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public FillLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the FillAntialias property
   *
   * @return property wrapper value around Boolean
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Boolean> getFillAntialias() {
    return (PropertyValue<Boolean>) new PropertyValue("fill-antialias", nativeGetFillAntialias());
  }

  /**
   * Get the FillOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillOpacity() {
    return (PropertyValue<Float>) new PropertyValue("fill-opacity", nativeGetFillOpacity());
  }

  /**
   * Get the FillOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillOpacityTransition() {
    return nativeGetFillOpacityTransition();
  }

  /**
   * Set the FillOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setFillOpacityTransition(TransitionOptions options) {
    nativeSetFillOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillColor() {
    return (PropertyValue<String>) new PropertyValue("fill-color", nativeGetFillColor());
  }

  /**
   * The color of the filled part of this layer. This color can be specified as `rgba` with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getFillColorAsInt() {
    PropertyValue<String> value = getFillColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("fill-color was set as a Function");
    }
  }

  /**
   * Get the FillColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillColorTransition() {
    return nativeGetFillColorTransition();
  }

  /**
   * Set the FillColor property transition options
   *
   * @param options transition options for String
   */
  public void setFillColorTransition(TransitionOptions options) {
    nativeSetFillColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillOutlineColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillOutlineColor() {
    return (PropertyValue<String>) new PropertyValue("fill-outline-color", nativeGetFillOutlineColor());
  }

  /**
   * The outline color of the fill. Matches the value of `fill-color` if unspecified.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getFillOutlineColorAsInt() {
    PropertyValue<String> value = getFillOutlineColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("fill-outline-color was set as a Function");
    }
  }

  /**
   * Get the FillOutlineColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillOutlineColorTransition() {
    return nativeGetFillOutlineColorTransition();
  }

  /**
   * Set the FillOutlineColor property transition options
   *
   * @param options transition options for String
   */
  public void setFillOutlineColorTransition(TransitionOptions options) {
    nativeSetFillOutlineColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getFillTranslate() {
    return (PropertyValue<Float[]>) new PropertyValue("fill-translate", nativeGetFillTranslate());
  }

  /**
   * Get the FillTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getFillTranslateTransition() {
    return nativeGetFillTranslateTransition();
  }

  /**
   * Set the FillTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setFillTranslateTransition(TransitionOptions options) {
    nativeSetFillTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillTranslateAnchor() {
    return (PropertyValue<String>) new PropertyValue("fill-translate-anchor", nativeGetFillTranslateAnchor());
  }

  /**
   * Get the FillPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillPattern() {
    return (PropertyValue<String>) new PropertyValue("fill-pattern", nativeGetFillPattern());
  }

  /**
   * Get the FillPattern property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillPatternTransition() {
    return nativeGetFillPatternTransition();
  }

  /**
   * Set the FillPattern property transition options
   *
   * @param options transition options for String
   */
  public void setFillPatternTransition(TransitionOptions options) {
    nativeSetFillPatternTransition(options.getDuration(), options.getDelay());
  }

  private native Object nativeGetFillAntialias();

  private native Object nativeGetFillOpacity();

  private native TransitionOptions nativeGetFillOpacityTransition();

  private native void nativeSetFillOpacityTransition(long duration, long delay);

  private native Object nativeGetFillColor();

  private native TransitionOptions nativeGetFillColorTransition();

  private native void nativeSetFillColorTransition(long duration, long delay);

  private native Object nativeGetFillOutlineColor();

  private native TransitionOptions nativeGetFillOutlineColorTransition();

  private native void nativeSetFillOutlineColorTransition(long duration, long delay);

  private native Object nativeGetFillTranslate();

  private native TransitionOptions nativeGetFillTranslateTransition();

  private native void nativeSetFillTranslateTransition(long duration, long delay);

  private native Object nativeGetFillTranslateAnchor();

  private native Object nativeGetFillPattern();

  private native TransitionOptions nativeGetFillPatternTransition();

  private native void nativeSetFillPatternTransition(long duration, long delay);

  @Override
  protected native void finalize() throws Throwable;

}
