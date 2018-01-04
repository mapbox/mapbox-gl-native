// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.light;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;

/**
 * The global light source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#light">The online documentation</a>
 */
@UiThread
public class Light {

  private long nativePtr;

  /**
   * Creates a Light.
   *
   * @param nativePtr pointer used by core
   */
  public Light(long nativePtr) {
    this.nativePtr = nativePtr;
  }

  /**
   * Set the Anchor property. Whether extruded geometries are lit relative to the map or viewport.
   *
   * @param anchor as String
   */
  public void setAnchor(@Property.ANCHOR String anchor) {
    nativeSetAnchor(anchor);
  }

  /**
   * Get the Anchor property. Whether extruded geometries are lit relative to the map or viewport.
   *
   * @return anchor as String
   */
  @Property.ANCHOR public String getAnchor() {
    return nativeGetAnchor();
  }

  /**
   * Set the Position property. Position of the light source relative to lit (extruded) geometries, in [r radial coordinate, a azimuthal angle, p polar angle] where r indicates the distance from the center of the base of an object to its light, a indicates the position of the light relative to 0° (0° when `light.anchor` is set to `viewport` corresponds to the top of the viewport, or 0° when `light.anchor` is set to `map` corresponds to due north, and degrees proceed clockwise), and p indicates the height of the light (from 0°, directly above, to 180°, directly below).
   *
   * @param position of the light
   */
  public void setPosition(@NonNull Position position) {
    nativeSetPosition(position);
  }

  /**
   * Get the Position property. Position of the light source relative to lit (extruded) geometries, in [r radial coordinate, a azimuthal angle, p polar angle] where r indicates the distance from the center of the base of an object to its light, a indicates the position of the light relative to 0° (0° when `light.anchor` is set to `viewport` corresponds to the top of the viewport, or 0° when `light.anchor` is set to `map` corresponds to due north, and degrees proceed clockwise), and p indicates the height of the light (from 0°, directly above, to 180°, directly below).
   *
   * @return position as Position
   */
  public Position getPosition() {
    return nativeGetPosition();
  }

  /**
   * Get the Position property transition options.
   *
   * @return transition options for position
   */
  public TransitionOptions getPositionTransition() {
    return nativeGetPositionTransition();
  }

  /**
   * Set the Position property transition options.
   *
   * @param options transition options for position
   */
  public void setPositionTransition(TransitionOptions options) {
    nativeSetPositionTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Set the Color property. Color tint for lighting extruded geometries.
   *
   * @param color as int
   */
  public void setColor(@ColorInt int color) {
    nativeSetColor(PropertyFactory.colorToRgbaString(color));
  }

  /**
   * Set the Color property. Color tint for lighting extruded geometries.
   *
   * @param color as String
   */
  public void setColor(String color) {
    nativeSetColor(color);
  }

  /**
   * Get the Color property. Color tint for lighting extruded geometries.
   *
   * @return color as String
   */
   public String getColor() {
    return nativeGetColor();
  }

  /**
   * Get the Color property transition options.
   *
   * @return transition options for color
   */
  public TransitionOptions getColorTransition() {
    return nativeGetColorTransition();
  }

  /**
   * Set the Color property transition options.
   *
   * @param options transition options for color
   */
  public void setColorTransition(TransitionOptions options) {
    nativeSetColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Set the Intensity property. Intensity of lighting (on a scale from 0 to 1). Higher numbers will present as more extreme contrast.
   *
   * @param intensity as Float
   */
  public void setIntensity(float intensity) {
    nativeSetIntensity(intensity);
  }

  /**
   * Get the Intensity property. Intensity of lighting (on a scale from 0 to 1). Higher numbers will present as more extreme contrast.
   *
   * @return intensity as Float
   */
   public float getIntensity() {
    return nativeGetIntensity();
  }

  /**
   * Get the Intensity property transition options.
   *
   * @return transition options for intensity
   */
  public TransitionOptions getIntensityTransition() {
    return nativeGetIntensityTransition();
  }

  /**
   * Set the Intensity property transition options.
   *
   * @param options transition options for intensity
   */
  public void setIntensityTransition(TransitionOptions options) {
    nativeSetIntensityTransition(options.getDuration(), options.getDelay());
  }

  private native void nativeSetAnchor(String anchor);
  private native String nativeGetAnchor();
  private native void nativeSetPosition(Position position);
  private native Position nativeGetPosition();
  private native TransitionOptions nativeGetPositionTransition();
  private native void nativeSetPositionTransition(long duration, long delay);
  private native void nativeSetColor(String color);
  private native String nativeGetColor();
  private native TransitionOptions nativeGetColorTransition();
  private native void nativeSetColorTransition(long duration, long delay);
  private native void nativeSetIntensity(float intensity);
  private native float nativeGetIntensity();
  private native TransitionOptions nativeGetIntensityTransition();
  private native void nativeSetIntensityTransition(long duration, long delay);
}