package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.ColorInt;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.maps.FocalPointChangeListener;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

/**
 * Settings to configure the visual appearance of the MyLocationView.
 *
 * @deprecated use location layer plugin from
 * https://github.com/mapbox/mapbox-plugins-android/tree/master/plugins/locationlayer instead.
 */
@Deprecated
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
  private float accuracyThreshold = 0f;

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

  /**
   * Initialise this with MapboxMapOptions.
   *
   * @param options the options to initialise this class from
   */
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
    setAccuracyThreshold(options.getMyLocationAccuracyThreshold());
  }

  public void onSaveInstanceState(Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_LOCATION_VIEW_ENABLED, isEnabled());
    outState.putByteArray(
      MapboxConstants.STATE_LOCATION_VIEW_FOREGROUND_DRAWABLE,
      BitmapUtils.getByteArrayFromDrawable(getForegroundDrawable())
    );
    outState.putByteArray(
      MapboxConstants.STATE_LOCATION_VIEW_FOREGROUND_BEARING_DRAWABLE,
      BitmapUtils.getByteArrayFromDrawable(getForegroundBearingDrawable())
    );
    outState.putInt(MapboxConstants.STATE_LOCATION_VIEW_FOREGROUND_TINT_COLOR, getForegroundTintColor());
    outState.putByteArray(
      MapboxConstants.STATE_LOCATION_VIEW_BACKGROUND_DRAWABLE,
      BitmapUtils.getByteArrayFromDrawable(getBackgroundDrawable())
    );
    outState.putIntArray(MapboxConstants.STATE_LOCATION_VIEW_BACKGROUND_OFFSET, getBackgroundOffset());
    outState.putInt(MapboxConstants.STATE_LOCATION_VIEW_BACKGROUND_TINT_COLOR, getBackgroundTintColor());
    outState.putInt(MapboxConstants.STATE_LOCATION_VIEW_ACCURACY_ALPHA, getAccuracyAlpha());
    outState.putInt(MapboxConstants.STATE_LOCATION_VIEW_ACCURACY_TINT_COLOR, getAccuracyTintColor());
    outState.putFloat(MapboxConstants.STATE_LOCATION_VIEW_ACCURACY_THRESHOLD, getAccuracyThreshold());
    outState.putIntArray(MapboxConstants.STATE_LOCATION_VIEW_PADDING, getPadding());
  }

  public void onRestoreInstanceState(@NonNull Bundle savedInstanceState) {
    setEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_LOCATION_VIEW_ENABLED));
    setForegroundDrawable(
      BitmapUtils.getDrawableFromByteArray(
        myLocationView.getContext(),
        savedInstanceState.getByteArray(MapboxConstants.STATE_LOCATION_VIEW_FOREGROUND_DRAWABLE)
      ),
      BitmapUtils.getDrawableFromByteArray(
        myLocationView.getContext(),
        savedInstanceState.getByteArray(MapboxConstants.STATE_LOCATION_VIEW_FOREGROUND_BEARING_DRAWABLE)
      )
    );
    setForegroundTintColor(savedInstanceState.getInt(MapboxConstants.STATE_LOCATION_VIEW_FOREGROUND_TINT_COLOR));
    setBackgroundDrawable(
      BitmapUtils.getDrawableFromByteArray(
        myLocationView.getContext(),
        savedInstanceState.getByteArray(MapboxConstants.STATE_LOCATION_VIEW_BACKGROUND_DRAWABLE)
      ),
      savedInstanceState.getIntArray(MapboxConstants.STATE_LOCATION_VIEW_BACKGROUND_OFFSET)
    );
    setBackgroundTintColor(savedInstanceState.getInt(MapboxConstants.STATE_LOCATION_VIEW_BACKGROUND_TINT_COLOR));
    setAccuracyAlpha(savedInstanceState.getInt(MapboxConstants.STATE_LOCATION_VIEW_ACCURACY_ALPHA));
    setAccuracyTintColor(savedInstanceState.getInt(MapboxConstants.STATE_LOCATION_VIEW_ACCURACY_TINT_COLOR));
    setAccuracyThreshold(savedInstanceState.getFloat(MapboxConstants.STATE_LOCATION_VIEW_ACCURACY_THRESHOLD));
    setPadding(savedInstanceState.getIntArray(MapboxConstants.STATE_LOCATION_VIEW_PADDING));
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
   * It's linked with the foreground tint color
   *
   * @param foregroundDrawable        the drawable to show as foreground without bearing
   * @param foregroundBearingDrawable the drawable to show as foreground when bearing is enabled
   */
  public void setForegroundDrawable(Drawable foregroundDrawable, Drawable foregroundBearingDrawable) {
    this.foregroundDrawable = foregroundDrawable;
    this.foregroundBearingDrawable = foregroundBearingDrawable;
    myLocationView.setForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
    myLocationView.setForegroundDrawableTint(foregroundTintColor);
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
   * @param foregroundTintColor the color to tint the foreground drawable or -1 (undefined color) to remove the
   *                            existing foreground tint color
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
   * It's linked with the background tint color
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
    myLocationView.setShadowDrawableTint(backgroundTintColor);
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
   * @param backgroundTintColor the color to tint the background drawable or -1 (undefined color) to remove the
   *                            existing background tint color
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
    setPadding(padding);
  }

  private void setPadding(int[] padding) {
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

  /**
   * Returns current accuracy threshold value (in meters).
   *
   * @return Value of accuracy threshold (in meters), below which circle won't be displayed
   */
  public float getAccuracyThreshold() {
    return accuracyThreshold;
  }

  /**
   * Set accuracy circle threshold. Circle won't be displayed if accuracy is below set value.
   *
   * @param accuracyThreshold Value of accuracy (in meters), below which circle won't be displayed
   */
  public void setAccuracyThreshold(float accuracyThreshold) {
    this.accuracyThreshold = accuracyThreshold;
    myLocationView.setAccuracyThreshold(accuracyThreshold);
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
