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
 * Client-side hillshading visualization based on DEM data. Currently, the implementation only supports Mapbox Terrain RGB and Mapzen Terrarium tiles.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-hillshade">The online documentation</a>
 */
@UiThread
public class HillshadeLayer extends Layer {

  /**
   * Creates a HillshadeLayer.
   *
   * @param nativePtr pointer used by core
   */
  @Keep
  public HillshadeLayer(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Creates a HillshadeLayer.
   *
   * @param layerId  the id of the layer
   * @param sourceId the id of the source
   */
  public HillshadeLayer(String layerId, String sourceId) {
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
  public HillshadeLayer withSourceLayer(String sourceLayer) {
    setSourceLayer(sourceLayer);
    return this;
  }

  /**
   * Set a property or properties.
   *
   * @param properties the var-args properties
   * @return This
   */
  public HillshadeLayer withProperties(@NonNull PropertyValue<?>... properties) {
    setProperties(properties);
    return this;
  }

  // Property getters

  /**
   * Get the HillshadeIlluminationDirection property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHillshadeIlluminationDirection() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("hillshade-illumination-direction", nativeGetHillshadeIlluminationDirection());
  }

  /**
   * Get the HillshadeIlluminationAnchor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getHillshadeIlluminationAnchor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("hillshade-illumination-anchor", nativeGetHillshadeIlluminationAnchor());
  }

  /**
   * Get the HillshadeExaggeration property
   *
   * @return property wrapper value around Float
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<Float> getHillshadeExaggeration() {
    checkThread();
    return (PropertyValue<Float>) new PropertyValue("hillshade-exaggeration", nativeGetHillshadeExaggeration());
  }

  /**
   * Get the HillshadeExaggeration property transition options
   *
   * @return transition options for Float
   */
  public TransitionOptions getHillshadeExaggerationTransition() {
    checkThread();
    return nativeGetHillshadeExaggerationTransition();
  }

  /**
   * Set the HillshadeExaggeration property transition options
   *
   * @param options transition options for Float
   */
  public void setHillshadeExaggerationTransition(TransitionOptions options) {
    checkThread();
    nativeSetHillshadeExaggerationTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HillshadeShadowColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getHillshadeShadowColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("hillshade-shadow-color", nativeGetHillshadeShadowColor());
  }

  /**
   * The shading color of areas that face away from the light source.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getHillshadeShadowColorAsInt() {
    checkThread();
    PropertyValue<String> value = getHillshadeShadowColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("hillshade-shadow-color was set as a Function");
    }
  }

  /**
   * Get the HillshadeShadowColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getHillshadeShadowColorTransition() {
    checkThread();
    return nativeGetHillshadeShadowColorTransition();
  }

  /**
   * Set the HillshadeShadowColor property transition options
   *
   * @param options transition options for String
   */
  public void setHillshadeShadowColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetHillshadeShadowColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HillshadeHighlightColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getHillshadeHighlightColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("hillshade-highlight-color", nativeGetHillshadeHighlightColor());
  }

  /**
   * The shading color of areas that faces towards the light source.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getHillshadeHighlightColorAsInt() {
    checkThread();
    PropertyValue<String> value = getHillshadeHighlightColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("hillshade-highlight-color was set as a Function");
    }
  }

  /**
   * Get the HillshadeHighlightColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getHillshadeHighlightColorTransition() {
    checkThread();
    return nativeGetHillshadeHighlightColorTransition();
  }

  /**
   * Set the HillshadeHighlightColor property transition options
   *
   * @param options transition options for String
   */
  public void setHillshadeHighlightColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetHillshadeHighlightColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Get the HillshadeAccentColor property
   *
   * @return property wrapper value around String
   */
  @SuppressWarnings("unchecked")
  public PropertyValue<String> getHillshadeAccentColor() {
    checkThread();
    return (PropertyValue<String>) new PropertyValue("hillshade-accent-color", nativeGetHillshadeAccentColor());
  }

  /**
   * The shading color used to accentuate rugged terrain like sharp cliffs and gorges.
   *
   * @return int representation of a rgba string color
   * @throws RuntimeException thrown if property isn't a value
   */
  @ColorInt
  public int getHillshadeAccentColorAsInt() {
    checkThread();
    PropertyValue<String> value = getHillshadeAccentColor();
    if (value.isValue()) {
      return rgbaToColor(value.getValue());
    } else {
      throw new RuntimeException("hillshade-accent-color was set as a Function");
    }
  }

  /**
   * Get the HillshadeAccentColor property transition options
   *
   * @return transition options for String
   */
  public TransitionOptions getHillshadeAccentColorTransition() {
    checkThread();
    return nativeGetHillshadeAccentColorTransition();
  }

  /**
   * Set the HillshadeAccentColor property transition options
   *
   * @param options transition options for String
   */
  public void setHillshadeAccentColorTransition(TransitionOptions options) {
    checkThread();
    nativeSetHillshadeAccentColorTransition(options.getDuration(), options.getDelay());
  }

  @Keep
  private native Object nativeGetHillshadeIlluminationDirection();

  @Keep
  private native Object nativeGetHillshadeIlluminationAnchor();

  @Keep
  private native Object nativeGetHillshadeExaggeration();

  @Keep
  private native TransitionOptions nativeGetHillshadeExaggerationTransition();

  @Keep
  private native void nativeSetHillshadeExaggerationTransition(long duration, long delay);

  @Keep
  private native Object nativeGetHillshadeShadowColor();

  @Keep
  private native TransitionOptions nativeGetHillshadeShadowColorTransition();

  @Keep
  private native void nativeSetHillshadeShadowColorTransition(long duration, long delay);

  @Keep
  private native Object nativeGetHillshadeHighlightColor();

  @Keep
  private native TransitionOptions nativeGetHillshadeHighlightColorTransition();

  @Keep
  private native void nativeSetHillshadeHighlightColorTransition(long duration, long delay);

  @Keep
  private native Object nativeGetHillshadeAccentColor();

  @Keep
  private native TransitionOptions nativeGetHillshadeAccentColorTransition();

  @Keep
  private native void nativeSetHillshadeAccentColorTransition(long duration, long delay);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
