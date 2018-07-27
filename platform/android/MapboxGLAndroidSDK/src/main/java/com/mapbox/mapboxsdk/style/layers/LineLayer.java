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
 * A stroked line.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-line">The online documentation</a>
 */
@UiThread
public class LineLayer extends Layer {

  /**
   * Creates a LineLayer.
   *
   * @param nativePtr pointer used by core
   */
  @Keep
  public LineLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a LineLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public LineLayer(String layerId, String sourceId) {
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
  public LineLayer withSourceLayer(String sourceLayer) {
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
  public LineLayer withFilter(Expression filter) {
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
  public LineLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the LineCap property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getLineCap() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("line-cap", nativeGetLineCap());
  }

  /**
   * Get the LineJoin property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getLineJoin() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("line-join", nativeGetLineJoin());
  }

  /**
   * Get the LineMiterLimit property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineMiterLimit() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-miter-limit", nativeGetLineMiterLimit());
  }

  /**
   * Get the LineRoundLimit property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineRoundLimit() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-round-limit", nativeGetLineRoundLimit());
  }

  /**
   * Get the LineOpacity property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineOpacity() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-opacity", nativeGetLineOpacity());
  }

  /**
   * Get the LineOpacity property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getLineOpacityTransition() {
    checkThread();
    return nativeGetLineOpacityTransition();
  }

  /**
   * Set the LineOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setLineOpacityTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getLineColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("line-color", nativeGetLineColor());
  }

  /**
   * The color with which the line will be drawn.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getLineColorAsInt() {
    checkThread();
    PropertyValue<String> value = getLineColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("line-color was set as a Function");
    }
  }

  /**
   * Get the LineColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getLineColorTransition() {
    checkThread();
    return nativeGetLineColorTransition();
  }

  /**
   * Set the LineColor property transition options
   *
   * @param options transition options for String
   */
  public void setLineColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getLineTranslate() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("line-translate", nativeGetLineTranslate());
  }

  /**
   * Get the LineTranslate property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getLineTranslateTransition() {
    checkThread();
    return nativeGetLineTranslateTransition();
  }

  /**
   * Set the LineTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setLineTranslateTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getLineTranslateAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("line-translate-anchor", nativeGetLineTranslateAnchor());
  }

  /**
   * Get the LineWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineWidth() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-width", nativeGetLineWidth());
  }

  /**
   * Get the LineWidth property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getLineWidthTransition() {
    checkThread();
    return nativeGetLineWidthTransition();
  }

  /**
   * Set the LineWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setLineWidthTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineGapWidth property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineGapWidth() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-gap-width", nativeGetLineGapWidth());
  }

  /**
   * Get the LineGapWidth property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getLineGapWidthTransition() {
    checkThread();
    return nativeGetLineGapWidthTransition();
  }

  /**
   * Set the LineGapWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setLineGapWidthTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineGapWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineOffset property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineOffset() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-offset", nativeGetLineOffset());
  }

  /**
   * Get the LineOffset property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getLineOffsetTransition() {
    checkThread();
    return nativeGetLineOffsetTransition();
  }

  /**
   * Set the LineOffset property transition options
   *
   * @param options transition options for Float
   */
  public void setLineOffsetTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineOffsetTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineBlur property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getLineBlur() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("line-blur", nativeGetLineBlur());
  }

  /**
   * Get the LineBlur property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getLineBlurTransition() {
    checkThread();
    return nativeGetLineBlurTransition();
  }

  /**
   * Set the LineBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setLineBlurTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineDasharray property
   *
   * @return property wrapper value around Float[]
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float[]> getLineDasharray() {
    checkThread();
    return (PropertyValue<Float[]>) new PropertyValue("line-dasharray", nativeGetLineDasharray());
  }

  /**
   * Get the LineDasharray property transition options
   *
   * @return transition options for Float[]
   */
  public TransitionOptions getLineDasharrayTransition() {
    checkThread();
    return nativeGetLineDasharrayTransition();
  }

  /**
   * Set the LineDasharray property transition options
   *
   * @param options transition options for Float[]
   */
  public void setLineDasharrayTransition(TransitionOptions options) {
    checkThread();
    nativeSetLineDasharrayTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LinePattern property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getLinePattern() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("line-pattern", nativeGetLinePattern());
  }

  /**
   * Get the LinePattern property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getLinePatternTransition() {
    checkThread();
    return nativeGetLinePatternTransition();
  }

  /**
   * Set the LinePattern property transition options
   *
   * @param options transition options for String
   */
  public void setLinePatternTransition(TransitionOptions options) {
    checkThread();
    nativeSetLinePatternTransition(options.getDuration(), options.getDelay());
  }

  @Keep
  private native Object nativeGetLineCap();

  @Keep
  private native Object nativeGetLineJoin();

  @Keep
  private native Object nativeGetLineMiterLimit();

  @Keep
  private native Object nativeGetLineRoundLimit();

  @Keep
  private native Object nativeGetLineOpacity();

  @Keep
  private native TransitionOptions nativeGetLineOpacityTransition();

  @Keep
  private native void nativeSetLineOpacityTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineColor();

  @Keep
  private native TransitionOptions nativeGetLineColorTransition();

  @Keep
  private native void nativeSetLineColorTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineTranslate();

  @Keep
  private native TransitionOptions nativeGetLineTranslateTransition();

  @Keep
  private native void nativeSetLineTranslateTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineTranslateAnchor();

  @Keep
  private native Object nativeGetLineWidth();

  @Keep
  private native TransitionOptions nativeGetLineWidthTransition();

  @Keep
  private native void nativeSetLineWidthTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineGapWidth();

  @Keep
  private native TransitionOptions nativeGetLineGapWidthTransition();

  @Keep
  private native void nativeSetLineGapWidthTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineOffset();

  @Keep
  private native TransitionOptions nativeGetLineOffsetTransition();

  @Keep
  private native void nativeSetLineOffsetTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineBlur();

  @Keep
  private native TransitionOptions nativeGetLineBlurTransition();

  @Keep
  private native void nativeSetLineBlurTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLineDasharray();

  @Keep
  private native TransitionOptions nativeGetLineDasharrayTransition();

  @Keep
  private native void nativeSetLineDasharrayTransition(long duration, long delay);

  @Keep
  private native Object nativeGetLinePattern();

  @Keep
  private native TransitionOptions nativeGetLinePatternTransition();

  @Keep
  private native void nativeSetLinePatternTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
