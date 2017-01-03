package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v4.graphics.drawable.DrawableCompat;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.maps.FocalPointChangeListener;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;

import static com.mapbox.mapboxsdk.constants.MyLocationConstants.ACCURACY_LAYER;
import static com.mapbox.mapboxsdk.constants.MyLocationConstants.USER_LOCATION_ARROW_ICON;
import static com.mapbox.mapboxsdk.constants.MyLocationConstants.USER_LOCATION_ICON;
import static com.mapbox.mapboxsdk.constants.MyLocationConstants.USER_LOCATION_ICON_BACKGROUND;

// TODO change file name
/**
 * Settings to configure the visual appearance of the MyLocationView.
 */
public class MyLocationViewSettings {

  private Projection projection;
  private MyLocationView myLocationView;
  private FocalPointChangeListener focalPointChangeListener;
  private MapboxMap mapboxMap;

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

  private float accuracyAlpha;

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
    setBackgroundDrawable(options.getMyLocationBackgroundDrawable());
    setBackgroundTintColor(options.getMyLocationBackgroundTintColor());
    setAccuracyAlpha(options.getMyLocationAccuracyAlpha());
    setAccuracyTintColor(options.getMyLocationAccuracyTintColor());
  }

  public void setMapboxMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
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

  public void setStaleStateTint() {

  }

  public

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
    // Add the location icon image to the map
    Bitmap icon = getBitmapFromDrawable(foregroundDrawable);
    mapboxMap.addImage(USER_LOCATION_ICON, icon);

    Bitmap bearingIcon = getBitmapFromDrawable(foregroundBearingDrawable);
    mapboxMap.addImage(USER_LOCATION_ARROW_ICON, bearingIcon);
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
    if (foregroundDrawable != null) {
      DrawableCompat.setTint(foregroundDrawable, foregroundTintColor);
      Bitmap userLocationIcon = getBitmapFromDrawable(foregroundDrawable);
      mapboxMap.addImage(USER_LOCATION_ICON, userLocationIcon);
    }
    if (foregroundBearingDrawable != null) {
      DrawableCompat.setTint(foregroundBearingDrawable, foregroundTintColor);
      Bitmap userBearingIcon = getBitmapFromDrawable(foregroundBearingDrawable);
      mapboxMap.addImage(USER_LOCATION_ARROW_ICON, userBearingIcon);
    }
  }

  private static Bitmap getBitmapFromDrawable(Drawable drawable) {
    if (drawable instanceof BitmapDrawable) {
      return ((BitmapDrawable) drawable).getBitmap();
    } else {
      Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(),
        Bitmap.Config.ARGB_8888);
      Canvas canvas = new Canvas(bitmap);
      drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
      drawable.draw(canvas);
      return bitmap;
    }
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
   */
  public void setBackgroundDrawable(Drawable backgroundDrawable) {
    this.backgroundDrawable = backgroundDrawable;
    Bitmap bitmap = getBitmapFromDrawable(backgroundDrawable);
    mapboxMap.addImage(USER_LOCATION_ICON_BACKGROUND, bitmap);
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
    DrawableCompat.setTint(backgroundDrawable, backgroundTintColor);
    Bitmap bitmap = getBitmapFromDrawable(backgroundDrawable);
    mapboxMap.addImage(USER_LOCATION_ICON_BACKGROUND, bitmap);
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
    // TODO allow setting padding
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
  public float getAccuracyAlpha() {
    return accuracyAlpha;
  }

  /**
   * Set the alpha value of the accuracy circle of MyLocation. Default is {@code 0.3f} and can range from 0 to 1.
   *
   * @param accuracyAlpha the alpha value to set
   */
  public void setAccuracyAlpha(float accuracyAlpha) {
    this.accuracyAlpha = accuracyAlpha;
    if (mapboxMap.getLayer(ACCURACY_LAYER) != null) {
      mapboxMap.getLayer(ACCURACY_LAYER).setProperties(
        PropertyFactory.fillOpacity(accuracyAlpha)
      );
    }
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
    if (mapboxMap.getLayer(ACCURACY_LAYER) != null) {
      mapboxMap.getLayer(ACCURACY_LAYER).setProperties(
        PropertyFactory.fillColor(accuracyTintColor)
      );
    }
  }

  public void setTilt(double tilt) {
    myLocationView.setTilt(tilt);
  }

  private void invalidateFocalPointForTracking(MyLocationView myLocationView) {
    if (!(myLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE)) {
      focalPointChangeListener.onFocalPointChanged(new PointF(myLocationView.getCenterX(),
        myLocationView.getCenterY()));
    } else {
      focalPointChangeListener.onFocalPointChanged(null);
    }
  }
}
