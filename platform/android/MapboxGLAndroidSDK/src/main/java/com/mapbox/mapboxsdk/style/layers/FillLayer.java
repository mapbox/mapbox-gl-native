// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.google.gson.JsonArray;
import com.mapbox.mapboxsdk.style.expressions.Expression;
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
  @Keep
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
    checkThread();
    return nativeGetSourceLayer();
  }

  /**
   * Set a single expression filter.
   *
   * @param filter the expression filter to set
   */
  public void setFilter(Expression filter) {
    checkThread();
    nativeSetFilter(filter.toArray());
  }

  /**
   * Set a single expression filter.
   *
   * @param filter the expression filter to set
   * @return This
   */
  public FillLayer withFilter(Expression filter) {
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
    Expression expression = null;
    JsonArray array = (JsonArray) nativeGetFilter();
    if (array != null) {
      expression = Expression.Converter.convert(array);
    }
    return expression;
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
    checkThread();
    return (PropertyValue<Boolean>) new PropertyValue("fill-antialias", nativeGetFillAntialias());
  }

  /**
   * Get the FillOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("fill-opacity", nativeGetFillOpacity());
  }

  /**
   * Get the FillOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillOpacityTransition() {
    checkThread();
    return nativeGetFillOpacityTransition();
  }

  /**
   * Set the FillOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setFillOpacityTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillColor() {
    checkThread();
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
    checkThread();
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
    checkThread();
    return nativeGetFillColorTransition();
  }

  /**
   * Set the FillColor property transition options
   *
   * @param options transition options for String
   */
  public void setFillColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillOutlineColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillOutlineColor() {
    checkThread();
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
    checkThread();
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
    checkThread();
    return nativeGetFillOutlineColorTransition();
  }

  /**
   * Set the FillOutlineColor property transition options
   *
   * @param options transition options for String
   */
  public void setFillOutlineColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillOutlineColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getFillTranslate() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("fill-translate", nativeGetFillTranslate());
  }

  /**
   * Get the FillTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getFillTranslateTransition() {
    checkThread();
    return nativeGetFillTranslateTransition();
  }

  /**
   * Set the FillTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setFillTranslateTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillTranslateAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("fill-translate-anchor", nativeGetFillTranslateAnchor());
  }

  /**
   * Get the FillPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillPattern() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("fill-pattern", nativeGetFillPattern());
  }

  /**
   * Get the FillPattern property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillPatternTransition() {
    checkThread();
    return nativeGetFillPatternTransition();
  }

  /**
   * Set the FillPattern property transition options
   *
   * @param options transition options for String
   */
  public void setFillPatternTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillPatternTransition(options.getDuration(), options.getDelay());
  }

  @Keep
  private native Object nativeGetFillAntialias();

  @Keep
  private native Object nativeGetFillOpacity();

  @Keep
  private native TransitionOptions nativeGetFillOpacityTransition();

  @Keep
  private native void nativeSetFillOpacityTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillColor();

  @Keep
  private native TransitionOptions nativeGetFillColorTransition();

  @Keep
  private native void nativeSetFillColorTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillOutlineColor();

  @Keep
  private native TransitionOptions nativeGetFillOutlineColorTransition();

  @Keep
  private native void nativeSetFillOutlineColorTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillTranslate();

  @Keep
  private native TransitionOptions nativeGetFillTranslateTransition();

  @Keep
  private native void nativeSetFillTranslateTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillTranslateAnchor();

  @Keep
  private native Object nativeGetFillPattern();

  @Keep
  private native TransitionOptions nativeGetFillPatternTransition();

  @Keep
  private native void nativeSetFillPatternTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
