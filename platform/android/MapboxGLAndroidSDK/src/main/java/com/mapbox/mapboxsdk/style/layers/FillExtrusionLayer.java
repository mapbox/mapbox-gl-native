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
  @Keep
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
  public FillExtrusionLayer withFilter(Expression filter) {
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
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("fill-extrusion-opacity", nativeGetFillExtrusionOpacity());
  }

  /**
   * Get the FillExtrusionOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillExtrusionOpacityTransition() {
    checkThread();
    return nativeGetFillExtrusionOpacityTransition();
  }

  /**
   * Set the FillExtrusionOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setFillExtrusionOpacityTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillExtrusionOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillExtrusionColor() {
    checkThread();
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
    checkThread();
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
    checkThread();
    return nativeGetFillExtrusionColorTransition();
  }

  /**
   * Set the FillExtrusionColor property transition options
   *
   * @param options transition options for String
   */
  public void setFillExtrusionColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillExtrusionColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getFillExtrusionTranslate() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("fill-extrusion-translate", nativeGetFillExtrusionTranslate());
  }

  /**
   * Get the FillExtrusionTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getFillExtrusionTranslateTransition() {
    checkThread();
    return nativeGetFillExtrusionTranslateTransition();
  }

  /**
   * Set the FillExtrusionTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setFillExtrusionTranslateTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillExtrusionTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillExtrusionTranslateAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("fill-extrusion-translate-anchor", nativeGetFillExtrusionTranslateAnchor());
  }

  /**
   * Get the FillExtrusionPattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getFillExtrusionPattern() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("fill-extrusion-pattern", nativeGetFillExtrusionPattern());
  }

  /**
   * Get the FillExtrusionPattern property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getFillExtrusionPatternTransition() {
    checkThread();
    return nativeGetFillExtrusionPatternTransition();
  }

  /**
   * Set the FillExtrusionPattern property transition options
   *
   * @param options transition options for String
   */
  public void setFillExtrusionPatternTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillExtrusionPatternTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionHeight property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillExtrusionHeight() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("fill-extrusion-height", nativeGetFillExtrusionHeight());
  }

  /**
   * Get the FillExtrusionHeight property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillExtrusionHeightTransition() {
    checkThread();
    return nativeGetFillExtrusionHeightTransition();
  }

  /**
   * Set the FillExtrusionHeight property transition options
   *
   * @param options transition options for Float
   */
  public void setFillExtrusionHeightTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillExtrusionHeightTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the FillExtrusionBase property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getFillExtrusionBase() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("fill-extrusion-base", nativeGetFillExtrusionBase());
  }

  /**
   * Get the FillExtrusionBase property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getFillExtrusionBaseTransition() {
    checkThread();
    return nativeGetFillExtrusionBaseTransition();
  }

  /**
   * Set the FillExtrusionBase property transition options
   *
   * @param options transition options for Float
   */
  public void setFillExtrusionBaseTransition(TransitionOptions options) {
    checkThread();
    nativeSetFillExtrusionBaseTransition(options.getDuration(), options.getDelay());
  }

  @Keep
  private native Object nativeGetFillExtrusionOpacity();

  @Keep
  private native TransitionOptions nativeGetFillExtrusionOpacityTransition();

  @Keep
  private native void nativeSetFillExtrusionOpacityTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillExtrusionColor();

  @Keep
  private native TransitionOptions nativeGetFillExtrusionColorTransition();

  @Keep
  private native void nativeSetFillExtrusionColorTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillExtrusionTranslate();

  @Keep
  private native TransitionOptions nativeGetFillExtrusionTranslateTransition();

  @Keep
  private native void nativeSetFillExtrusionTranslateTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillExtrusionTranslateAnchor();

  @Keep
  private native Object nativeGetFillExtrusionPattern();

  @Keep
  private native TransitionOptions nativeGetFillExtrusionPatternTransition();

  @Keep
  private native void nativeSetFillExtrusionPatternTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillExtrusionHeight();

  @Keep
  private native TransitionOptions nativeGetFillExtrusionHeightTransition();

  @Keep
  private native void nativeSetFillExtrusionHeightTransition(long duration, long delay);

  @Keep
  private native Object nativeGetFillExtrusionBase();

  @Keep
  private native TransitionOptions nativeGetFillExtrusionBaseTransition();

  @Keep
  private native void nativeSetFillExtrusionBaseTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
