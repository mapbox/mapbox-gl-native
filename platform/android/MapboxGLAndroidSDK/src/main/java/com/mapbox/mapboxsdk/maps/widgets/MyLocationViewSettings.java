package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.maps.FocalPointChangeListener;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.Projection;

/**
 * Settings to configure the visual appearance of the MyLocationView.
 */
public class MyLocationViewSettings {

  private Projection projection;
  private MyLocationView myLocationView;
  private FocalPointChangeListener focalPointChangeListener;

  //
  // State
  //

  private boolean enabled;

  //
  // Foreground
  //

  private Drawable foregroundDrawable;
  private Drawable foregroundBearingDrawable;

  @ColorInt
  private int foregroundTintColor;

  //
  // Background
  //

  private Drawable backgroundDrawable;
  private int[] backgroundOffset = new int[4];

  @ColorInt
  private int backgroundTintColor;

  //
  // Accuracy
  //

  private int accuracyAlpha;

  @ColorInt
  private int accuracyTintColor;

  //
  // Padding
  //

  private int[] padding = new int[4];

  /**
   * Creates an instance of MyLocationViewSettings
   * <p>
   *
   * @param myLocationView            the MyLocationView to apply the settings to
   * @param projection                the MapView projection
   * @param focalPointChangedListener the interface to be invoked when focal points changes
   * @see MyLocationView
   */
  public MyLocationViewSettings(MyLocationView myLocationView, Projection projection, FocalPointChangeListener
    focalPointChangedListener) {
    this.myLocationView = myLocationView;
    this.projection = projection;
    this.focalPointChangeListener = focalPointChangedListener;
  }

  public void initialise(@NonNull MapboxMapOptions options) {
    CameraPosition position = options.getCamera();
    if (position != null && !position.equals(CameraPosition.DEFAULT)) {
      setTilt(position.tilt);
    }
    setForegroundDrawable(options.getMyLocationForegroundDrawable(), options.getMyLocationForegroundBearingDrawable());
    setForegroundTintColor(options.getMyLocationForegroundTintColor());
    setBackgroundDrawable(options.getMyLocationBackgroundDrawable(), options.getMyLocationBackgroundPadding());
    setBackgroundTintColor(options.getMyLocationBackgroundTintColor());
    setAccuracyAlpha(options.getMyLocationAccuracyAlpha());
    setAccuracyTintColor(options.getMyLocationAccuracyTintColor());
  }

  /**
   * Returns if the MyLocationView is enabled
   *
   * @return true if MyLocationView is enabled,
   */
  public boolean isEnabled() {
    return enabled;
  }

  /**
   * Set the enabled state of MyLocationView
   *
   * @param enabled true shows the MyLocationView on the map
   */
  public void setEnabled(boolean enabled) {
    this.enabled = enabled;
    myLocationView.setEnabled(enabled);
  }

  /**
   * Set the foreground drawable of the MyLocationView
   * <p>
   * The foreground drawable is the image visible on screen
   * </p>
   *
   * @param foregroundDrawable        the drawable to show as foreground without bearing
   * @param foregroundBearingDrawable the drawable to show as foreground when bearing is enabled
   */
  public void setForegroundDrawable(Drawable foregroundDrawable, Drawable foregroundBearingDrawable) {
    this.foregroundDrawable = foregroundDrawable;
    this.foregroundBearingDrawable = foregroundBearingDrawable;
    myLocationView.setForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
  }

  /**
   * Get the foreground drawable when bearing is disabled.
   *
   * @return the drawable used as foreground
   */
  public Drawable getForegroundDrawable() {
    return foregroundDrawable;
  }

  /**
   * Get the foreground drawable when bearing is enabled.
   *
   * @return the bearing drawable used as foreground
   */
  public Drawable getForegroundBearingDrawable() {
    return foregroundBearingDrawable;
  }

  /**
   * Set the foreground tint color.
   * <p>
   * The color will tint both the foreground and the bearing foreground drawable.
   * </p>
   *
   * @param foregroundTintColor the color to tint the foreground drawable
   */
  public void setForegroundTintColor(@ColorInt int foregroundTintColor) {
    this.foregroundTintColor = foregroundTintColor;
    myLocationView.setForegroundDrawableTint(foregroundTintColor);
  }

  /**
   * Get the foreground tint color.
   *
   * @return the foreground tint color
   */
  public int getForegroundTintColor() {
    return foregroundTintColor;
  }

  /**
   * Set the background drawable of MyLocationView
   * <p>
   * Padding can be added to provide an offset to the background
   * </p>
   *
   * @param backgroundDrawable the drawable to show as background
   * @param padding            the padding added to the background
   */
  public void setBackgroundDrawable(Drawable backgroundDrawable, int[] padding) {
    this.backgroundDrawable = backgroundDrawable;
    this.backgroundOffset = padding;
    if (padding != null && padding.length == 4) {
      myLocationView.setShadowDrawable(backgroundDrawable, padding[0], padding[1], padding[2], padding[3]);
    } else {
      myLocationView.setShadowDrawable(backgroundDrawable);
    }
  }

  /**
   * Get the background drawable of MyLocationView.
   *
   * @return the drawable used as background
   */
  public Drawable getBackgroundDrawable() {
    return backgroundDrawable;
  }

  /**
   * Set the background tint color.
   *
   * @param backgroundTintColor the color to tint the background
   */
  public void setBackgroundTintColor(@ColorInt int backgroundTintColor) {
    this.backgroundTintColor = backgroundTintColor;
    myLocationView.setShadowDrawableTint(backgroundTintColor);
  }

  /**
   * Get the background tint color.
   *
   * @return the background tint color
   */
  public int getBackgroundTintColor() {
    return backgroundTintColor;
  }

  /**
   * Get the background offset.
   *
   * @return the background offset
   */
  public int[] getBackgroundOffset() {
    return backgroundOffset;
  }

  /**
   * Set the MyLocationView padding.
   *
   * @param left   the padding left of MyLocationView
   * @param top    the padding top of MyLocationView
   * @param right  the padding right of MyLocationView
   * @param bottom the padding bottom of MyLocaionView
   */
  public void setPadding(int left, int top, int right, int bottom) {
    padding = new int[] {left, top, right, bottom};
    myLocationView.setContentPadding(padding);
    projection.invalidateContentPadding(padding);
    invalidateFocalPointForTracking(myLocationView);
  }

  /**
   * Get the MyLocationView padding.
   *
   * @return an array describing the padding in a LTRB manner
   */
  public int[] getPadding() {
    return padding;
  }

  /**
   * Get the alpha value of the accuracy circle of MyLocationView
   *
   * @return the alpha value
   */
  public int getAccuracyAlpha() {
    return accuracyAlpha;
  }

  /**
   * Set the alpha value of the accuracy circle of MyLocationView
   *
   * @param accuracyAlpha the alpha value to set
   */
  public void setAccuracyAlpha(@IntRange(from = 0, to = 255) int accuracyAlpha) {
    this.accuracyAlpha = accuracyAlpha;
    myLocationView.setAccuracyAlpha(accuracyAlpha);
  }

  /**
   * Get the accuracy tint color of MyLocationView.
   *
   * @return the tint color used for accuracy
   */
  public int getAccuracyTintColor() {
    return accuracyTintColor;
  }

  /**
   * Set the accuracy tint color of MyLocationView.
   *
   * @param accuracyTintColor the accuracy tint color
   */
  public void setAccuracyTintColor(@ColorInt int accuracyTintColor) {
    this.accuracyTintColor = accuracyTintColor;
    myLocationView.setAccuracyTint(accuracyTintColor);
  }

  public void setTilt(double tilt) {
    myLocationView.setTilt(tilt);
  }

  private void invalidateFocalPointForTracking(MyLocationView myLocationView) {
    if (!(myLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE)) {
      focalPointChangeListener.onFocalPointChanged(myLocationView.getCenter());
    } else {
      focalPointChangeListener.onFocalPointChanged(null);
    }
  }
}
