package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.ArgbEvaluator;
import android.animation.ValueAnimator;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.PorterDuff;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.v4.graphics.drawable.DrawableCompat;

import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.maps.FocalPointChangeListener;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;

import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.ACCURACY_LAYER;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.LOCATION_LAYER;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.LOCATION_LAYER_BACKGROUND;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.USER_LOCATION_ACCURACY_SOURCE;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.USER_LOCATION_BEARING_ICON;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.USER_LOCATION_ICON;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.USER_LOCATION_ICON_BACKGROUND;
import static com.mapbox.mapboxsdk.constants.MyLocationWidgetConstants.USER_LOCATION_SOURCE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconRotationAlignment;

/**
 * Settings to configure the visual appearance of the MyLocationView.
 */
public class MyLocationWidgetSettings {

  private Projection projection;
  private MyLocationWidget myLocationWidget;
  private FocalPointChangeListener focalPointChangeListener;
  private MapboxMap mapboxMap;

  private static final int STALE_USER_LOCATION_MS = 10000;
  private static final int STALE_STATE_TRANSITION_DURATION_MS = 1000;

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

  @MyBearingTracking.Mode
  private int myBearingTrackingMode;

  // Stale state variables
  private Handler staleHandler;
  private Runnable staleRunnable;
  private boolean isStale;
  private int staleStateTintColor;

  //
  // Padding
  //

  private int[] padding = new int[4];

  /**
   * Creates an instance of MyLocationWidgetSettings
   * <p>
   *
   * @param myLocationWidget          the MyLocationWidget to apply the settings to
   * @param projection                the MapView projection
   * @param focalPointChangedListener the interface to be invoked when focal points changes
   * @see MyLocationWidget
   */
  public MyLocationWidgetSettings(MyLocationWidget myLocationWidget, Projection projection, FocalPointChangeListener
    focalPointChangedListener) {
    this.myLocationWidget = myLocationWidget;
    this.projection = projection;
    this.focalPointChangeListener = focalPointChangedListener;
  }

  public void initialise(@NonNull MapboxMapOptions options) {
    setAccuracyTintColor(options.getMyLocationAccuracyTintColor());
    setAccuracyAlpha(options.getMyLocationAccuracyAlpha());
    setBackgroundDrawable(options.getMyLocationBackgroundDrawable());
    setBackgroundTintColor(options.getMyLocationBackgroundTintColor());
    setForegroundDrawable(options.getMyLocationForegroundDrawable(), options.getMyLocationForegroundBearingDrawable());
    setForegroundTintColor(options.getMyLocationForegroundTintColor());
    setStaleStateTint(options.getMyLocationStaleTintColor());
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
    myLocationWidget.toggleGps(enabled);
  }

  public void setStaleStateTint(@ColorInt int staleStateTintColor) {
    this.staleStateTintColor = staleStateTintColor;
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
    // Add the location icon image to the map
    Bitmap icon = getBitmapFromDrawable(foregroundDrawable);
    mapboxMap.addImage(USER_LOCATION_ICON, icon);

    Bitmap bearingIcon = getBitmapFromDrawable(foregroundBearingDrawable);
    mapboxMap.addImage(USER_LOCATION_BEARING_ICON, bearingIcon);

    if (mapboxMap.getLayer(LOCATION_LAYER) == null) {

      SymbolLayer locationLayer = new SymbolLayer(LOCATION_LAYER, USER_LOCATION_SOURCE).withProperties(
        iconImage(myBearingTrackingMode == MyBearingTracking.NONE ? USER_LOCATION_ICON : USER_LOCATION_BEARING_ICON),
        iconAllowOverlap(true),
        iconIgnorePlacement(true),
        iconRotationAlignment(Property.ICON_ROTATION_ALIGNMENT_MAP)
      );
      mapboxMap.addLayer(locationLayer);
    }
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
      mapboxMap.addImage(USER_LOCATION_BEARING_ICON, userBearingIcon);
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


    if (mapboxMap.getLayer(LOCATION_LAYER_BACKGROUND) == null) {
      SymbolLayer locationLayerBackground = new SymbolLayer(LOCATION_LAYER_BACKGROUND, USER_LOCATION_SOURCE).withProperties(
        iconImage(myBearingTrackingMode == MyBearingTracking.NONE ? USER_LOCATION_ICON_BACKGROUND : USER_LOCATION_BEARING_ICON),
        iconAllowOverlap(true),
        iconIgnorePlacement(true),
        iconRotationAlignment(Property.ICON_ROTATION_ALIGNMENT_MAP)
      );
      mapboxMap.addLayer(locationLayerBackground);
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
    //myLocationView.setContentPadding(padding);
    //projection.invalidateContentPadding(padding);
    //invalidateFocalPointForTracking(myLocationView);
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
   * Set the alpha value of the accuracy circle of MyLocationWidget. Default is {@code 0.3f} and can range from 0 to 1.
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
    if (mapboxMap.getLayer(ACCURACY_LAYER) == null) {
      FillLayer accuracyLayer = new FillLayer(ACCURACY_LAYER, USER_LOCATION_ACCURACY_SOURCE).withProperties(
        fillColor(accuracyTintColor),
        fillOpacity(0.3f)
      );
      mapboxMap.addLayer(accuracyLayer);
    } else {
      mapboxMap.getLayer(ACCURACY_LAYER).setProperties(
        PropertyFactory.fillColor(accuracyTintColor)
      );
    }
  }

  public void onStop() {
    if (staleHandler != null && staleRunnable != null) {
      staleHandler.removeCallbacks(staleRunnable);
    }
  }

  public void staleTransition() {
    if (isStale) {
      final Drawable drawable = mapboxMap.getMyLocationWidgetSettings().getForegroundDrawable();
      DrawableCompat.setTintMode(drawable, PorterDuff.Mode.SRC_IN);

      int tintColor = mapboxMap.getMyLocationWidgetSettings().getForegroundTintColor();

      ValueAnimator colorAnimator = ValueAnimator.ofObject(new ArgbEvaluator(), staleStateTintColor, tintColor);
      colorAnimator.setDuration(STALE_STATE_TRANSITION_DURATION_MS);
      colorAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
        @Override
        public void onAnimationUpdate(ValueAnimator valueAnimator) {
          DrawableCompat.setTint(drawable, (int) valueAnimator.getAnimatedValue());
          Bitmap icon = getBitmapFromDrawable(drawable);
          mapboxMap.addImage(USER_LOCATION_ICON, icon);
        }
      });
      colorAnimator.start();
      isStale = false;
    }

    // TODO handle stale state when user leaves and returns to app
    if (staleHandler != null && staleRunnable != null) {
      staleHandler.removeCallbacks(staleRunnable);
    }
    staleHandler = new Handler();
    staleRunnable = new Runnable() {
      @Override
      public void run() {
        isStale = true;

        final Drawable drawable = mapboxMap.getMyLocationWidgetSettings().getForegroundDrawable();
        DrawableCompat.setTintMode(drawable, PorterDuff.Mode.SRC_IN);

        int tintColor = mapboxMap.getMyLocationWidgetSettings().getForegroundTintColor();

        ValueAnimator colorAnimator = ValueAnimator.ofObject(new ArgbEvaluator(), tintColor, staleStateTintColor);
        colorAnimator.setDuration(STALE_STATE_TRANSITION_DURATION_MS);
        colorAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
          @Override
          public void onAnimationUpdate(ValueAnimator valueAnimator) {
            DrawableCompat.setTint(drawable, (int) valueAnimator.getAnimatedValue());
            Bitmap icon = getBitmapFromDrawable(drawable);
            mapboxMap.addImage(USER_LOCATION_ICON, icon);
          }
        });
        colorAnimator.start();
      }

    };
    staleHandler.postDelayed(staleRunnable, STALE_USER_LOCATION_MS);
  }

//  private void invalidateFocalPointForTracking(MyLocationView myLocationView) {
//    if (!(myLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE)) {
//      focalPointChangeListener.onFocalPointChanged(new PointF(myLocationView.getCenterX(),
//        myLocationView.getCenterY()));
//    } else {
//      focalPointChangeListener.onFocalPointChanged(null);
//    }
//  }
}
