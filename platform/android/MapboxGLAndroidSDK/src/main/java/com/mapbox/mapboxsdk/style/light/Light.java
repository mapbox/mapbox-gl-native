// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.

package com.mapbox.mapboxsdk.style.light;

import androidx.annotation.ColorInt;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.UiThread;

import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.utils.ColorUtils;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

/**
 * The global light source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#light">The online documentation</a>
 */
@UiThread
public class Light {

  private static final String TAG = "Mbgl-Light";

  @Keep
  private long nativePtr;

  /**
   * Creates a Light.
   *
   * @param nativePtr pointer used by core
   */
  @Keep
  Light(long nativePtr) {
    checkThread();
    this.nativePtr = nativePtr;
  }

  /**
   * Set the Anchor property. Whether extruded geometries are lit relative to the map or viewport.
   *
   * @param anchor as String
   */
  public void setAnchor(@Property.ANCHOR String anchor) {
    checkThread();
    nativeSetAnchor(anchor);
  }

  /**
   * Get the Anchor property. Whether extruded geometries are lit relative to the map or viewport.
   *
   * @return anchor as String
   */
   @NonNull
  @Property.ANCHOR public String getAnchor() {
    checkThread();
    return nativeGetAnchor();
  }

  /**
   * Set the Position property. Position of the light source relative to lit (extruded) geometries, in [r radial coordinate, a azimuthal angle, p polar angle] where r indicates the distance from the center of the base of an object to its light, a indicates the position of the light relative to 0&#xB0; (0&#xB0; when `light.anchor` is set to `viewport` corresponds to the top of the viewport, or 0&#xB0; when `light.anchor` is set to `map` corresponds to due north, and degrees proceed clockwise), and p indicates the height of the light (from 0&#xB0;, directly above, to 180&#xB0;, directly below).
   *
   * @param position of the light
   */
  public void setPosition(@NonNull Position position) {
    checkThread();
    nativeSetPosition(position);
  }

  /**
   * Get the Position property. Position of the light source relative to lit (extruded) geometries, in [r radial coordinate, a azimuthal angle, p polar angle] where r indicates the distance from the center of the base of an object to its light, a indicates the position of the light relative to 0&#xB0; (0&#xB0; when `light.anchor` is set to `viewport` corresponds to the top of the viewport, or 0&#xB0; when `light.anchor` is set to `map` corresponds to due north, and degrees proceed clockwise), and p indicates the height of the light (from 0&#xB0;, directly above, to 180&#xB0;, directly below).
   *
   * @return position as Position
   */
  @NonNull
  public Position getPosition() {
    checkThread();
    return nativeGetPosition();
  }

  /**
   * Get the Position property transition options.
   *
   * @return transition options for position
   */
  @NonNull
  public TransitionOptions getPositionTransition() {
    checkThread();
    return nativeGetPositionTransition();
  }

  /**
   * Set the Position property transition options.
   *
   * @param options transition options for position
   */
  public void setPositionTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetPositionTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Set the Color property. Color tint for lighting extruded geometries.
   *
   * @param color as int
   */
  public void setColor(@ColorInt int color) {
    checkThread();
    nativeSetColor(ColorUtils.colorToRgbaString(color));
  }

  /**
   * Set the Color property. Color tint for lighting extruded geometries.
   *
   * @param color as String
   */
  public void setColor(String color) {
    checkThread();
    nativeSetColor(color);
  }

  /**
   * Get the Color property. Color tint for lighting extruded geometries.
   *
   * @return color as String
   */
   @NonNull
   public String getColor() {
    checkThread();
    return nativeGetColor();
  }

  /**
   * Get the Color property transition options.
   *
   * @return transition options for color
   */
  @NonNull
  public TransitionOptions getColorTransition() {
    checkThread();
    return nativeGetColorTransition();
  }

  /**
   * Set the Color property transition options.
   *
   * @param options transition options for color
   */
  public void setColorTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetColorTransition(options.getDuration(), options.getDelay());
  }

  /**
   * Set the Intensity property. Intensity of lighting (on a scale from 0 to 1). Higher numbers will present as more extreme contrast.
   *
   * @param intensity as Float
   */
  public void setIntensity(float intensity) {
    checkThread();
    nativeSetIntensity(intensity);
  }

  /**
   * Get the Intensity property. Intensity of lighting (on a scale from 0 to 1). Higher numbers will present as more extreme contrast.
   *
   * @return intensity as Float
   */
   @NonNull
   public float getIntensity() {
    checkThread();
    return nativeGetIntensity();
  }

  /**
   * Get the Intensity property transition options.
   *
   * @return transition options for intensity
   */
  @NonNull
  public TransitionOptions getIntensityTransition() {
    checkThread();
    return nativeGetIntensityTransition();
  }

  /**
   * Set the Intensity property transition options.
   *
   * @param options transition options for intensity
   */
  public void setIntensityTransition(@NonNull TransitionOptions options) {
    checkThread();
    nativeSetIntensityTransition(options.getDuration(), options.getDelay());
  }

  private void checkThread(){
    ThreadUtils.checkThread(TAG);
  }

  @Keep
  private native void nativeSetAnchor(String anchor);

  @NonNull
  @Keep
  private native String nativeGetAnchor();
  @Keep
  private native void nativeSetPosition(Position position);

  @NonNull
  @Keep
  private native Position nativeGetPosition();
  @NonNull
  @Keep
  private native TransitionOptions nativeGetPositionTransition();

  @Keep
  private native void nativeSetPositionTransition(long duration, long delay);
  @Keep
  private native void nativeSetColor(String color);

  @NonNull
  @Keep
  private native String nativeGetColor();
  @NonNull
  @Keep
  private native TransitionOptions nativeGetColorTransition();

  @Keep
  private native void nativeSetColorTransition(long duration, long delay);
  @Keep
  private native void nativeSetIntensity(float intensity);

  @NonNull
  @Keep
  private native float nativeGetIntensity();
  @NonNull
  @Keep
  private native TransitionOptions nativeGetIntensityTransition();

  @Keep
  private native void nativeSetIntensityTransition(long duration, long delay);
}