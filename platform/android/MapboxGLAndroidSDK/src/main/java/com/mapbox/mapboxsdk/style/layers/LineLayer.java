// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.layers;

import androidx.annotation.ColorInt;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;

import static com.mapbox.mapboxsdk.utils.ColorUtils.rgbaToColor;

import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.style.expressions.Expression;

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
  LineLayer(long nativePtr) {
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
  @NonNull
  public LineLayer withSourceLayer(String sourceLayer) {
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
  public LineLayer withFilter(@NonNull Expression filter) {
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
  @NonNull
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
  @NonNull
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
  @NonNull
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
  @NonNull
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
  @NonNull
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
  @NonNull
  public TransitionOptions getLineOpacityTransition() {
    checkThread();
    return nativeGetLineOpacityTransition();
  }

  /**
   * Set the LineOpacity property transition options
   *
   * @param options transition options for Float
   */
  public void setLineOpacityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineOpacityTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineColor property
   *
   * @return property wrapper value around String
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLineColorTransition() {
    checkThread();
    return nativeGetLineColorTransition();
  }

  /**
   * Set the LineColor property transition options
   *
   * @param options transition options for String
   */
  public void setLineColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineTranslate property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLineTranslateTransition() {
    checkThread();
    return nativeGetLineTranslateTransition();
  }

  /**
   * Set the LineTranslate property transition options
   *
   * @param options transition options for Float[]
   */
  public void setLineTranslateTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineTranslateTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineTranslateAnchor property
   *
   * @return property wrapper value around String
   */
  @NonNull
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
  @NonNull
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
  @NonNull
  public TransitionOptions getLineWidthTransition() {
    checkThread();
    return nativeGetLineWidthTransition();
  }

  /**
   * Set the LineWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setLineWidthTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineGapWidth property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLineGapWidthTransition() {
    checkThread();
    return nativeGetLineGapWidthTransition();
  }

  /**
   * Set the LineGapWidth property transition options
   *
   * @param options transition options for Float
   */
  public void setLineGapWidthTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineGapWidthTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineOffset property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLineOffsetTransition() {
    checkThread();
    return nativeGetLineOffsetTransition();
  }

  /**
   * Set the LineOffset property transition options
   *
   * @param options transition options for Float
   */
  public void setLineOffsetTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineOffsetTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineBlur property
   *
   * @return property wrapper value around Float
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLineBlurTransition() {
    checkThread();
    return nativeGetLineBlurTransition();
  }

  /**
   * Set the LineBlur property transition options
   *
   * @param options transition options for Float
   */
  public void setLineBlurTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineBlurTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineDasharray property
   *
   * @return property wrapper value around Float[]
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLineDasharrayTransition() {
    checkThread();
    return nativeGetLineDasharrayTransition();
  }

  /**
   * Set the LineDasharray property transition options
   *
   * @param options transition options for Float[]
   */
  public void setLineDasharrayTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLineDasharrayTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LinePattern property
   *
   * @return property wrapper value around String
   */
  @NonNull
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
  @NonNull
  public TransitionOptions getLinePatternTransition() {
    checkThread();
    return nativeGetLinePatternTransition();
  }

  /**
   * Set the LinePattern property transition options
   *
   * @param options transition options for String
   */
  public void setLinePatternTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetLinePatternTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the LineGradient property
   *
   * @return property wrapper value around String
   */
  @NonNull
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getLineGradient() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("line-gradient", nativeGetLineGradient());
  }

  /**
   * Defines a gradient with which to color a line feature. Can only be used with GeoJSON sources that specify `"lineMetrics": true`.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getLineGradientAsInt() {
    checkThread();
    PropertyValue<String> value = getLineGradient();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("line-gradient was set as a Function");
    }
  }

  @NonNull
  @Keep
  private native Object nativeGetLineCap();

  @NonNull
  @Keep
  private native Object nativeGetLineJoin();

  @NonNull
  @Keep
  private native Object nativeGetLineMiterLimit();

  @NonNull
  @Keep
  private native Object nativeGetLineRoundLimit();

  @NonNull
  @Keep
  private native Object nativeGetLineOpacity();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineOpacityTransition();

  @Keep
  private native void nativeSetLineOpacityTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineColor();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineColorTransition();

  @Keep
  private native void nativeSetLineColorTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineTranslate();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineTranslateTransition();

  @Keep
  private native void nativeSetLineTranslateTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineTranslateAnchor();

  @NonNull
  @Keep
  private native Object nativeGetLineWidth();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineWidthTransition();

  @Keep
  private native void nativeSetLineWidthTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineGapWidth();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineGapWidthTransition();

  @Keep
  private native void nativeSetLineGapWidthTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineOffset();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineOffsetTransition();

  @Keep
  private native void nativeSetLineOffsetTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineBlur();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineBlurTransition();

  @Keep
  private native void nativeSetLineBlurTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineDasharray();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLineDasharrayTransition();

  @Keep
  private native void nativeSetLineDasharrayTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLinePattern();

  @NonNull
  @Keep
  private native TransitionOptions nativeGetLinePatternTransition();

  @Keep
  private native void nativeSetLinePatternTransition(long duration, long delay);

  @NonNull
  @Keep
  private native Object nativeGetLineGradient();

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
