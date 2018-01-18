package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.PointF;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.utils.BitmapUtils;
import com.mapbox.mapboxsdk.utils.ColorUtils;

/**
 * Settings for the user interface of a MapboxMap. To obtain this interface, call getUiSettings().
 */
public final class UiSettings {

  private final FocalPointChangeListener focalPointChangeListener;
  private final Projection projection;
  private final CompassView compassView;
  private final int[] compassMargins = new int[4];

  private final ImageView attributionsView;
  private final int[] attributionsMargins = new int[4];
  private AttributionDialogManager attributionDialogManager;

  private final View logoView;
  private final int[] logoMargins = new int[4];

  private float pixelRatio;

  private boolean rotateGesturesEnabled = true;
  private boolean rotateGestureChangeAllowed = true;

  private boolean tiltGesturesEnabled = true;
  private boolean tiltGestureChangeAllowed = true;

  private boolean zoomGesturesEnabled = true;
  private boolean zoomGestureChangeAllowed = true;

  private boolean scrollGesturesEnabled = true;
  private boolean scrollGestureChangeAllowed = true;

  private boolean zoomControlsEnabled;

  private boolean doubleTapGesturesEnabled = true;
  private boolean doubleTapGestureChangeAllowed = true;

  private boolean deselectMarkersOnTap = true;

  private PointF userProvidedFocalPoint;

  UiSettings(@NonNull Projection projection, @NonNull FocalPointChangeListener listener,
             @NonNull CompassView compassView, @NonNull ImageView attributionsView, @NonNull View logoView) {
    this.projection = projection;
    this.focalPointChangeListener = listener;
    this.compassView = compassView;
    this.attributionsView = attributionsView;
    this.logoView = logoView;
    if (logoView.getResources() != null) {
      this.pixelRatio = logoView.getResources().getDisplayMetrics().density;
    }
  }

  void initialise(@NonNull Context context, @NonNull MapboxMapOptions options) {
    Resources resources = context.getResources();
    initialiseGestures(options);
    initialiseCompass(options, resources);
    initialiseLogo(options, resources);
    initialiseAttribution(context, options);
    initialiseZoomControl(context);
  }

  void onSaveInstanceState(Bundle outState) {
    saveGestures(outState);
    saveCompass(outState);
    saveLogo(outState);
    saveAttribution(outState);
    saveZoomControl(outState);
    saveDeselectMarkersOnTap(outState);
    saveFocalPoint(outState);
  }

  void onRestoreInstanceState(@NonNull Bundle savedInstanceState) {
    restoreGestures(savedInstanceState);
    restoreCompass(savedInstanceState);
    restoreLogo(savedInstanceState);
    restoreAttribution(savedInstanceState);
    restoreZoomControl(savedInstanceState);
    restoreDeselectMarkersOnTap(savedInstanceState);
    restoreFocalPoint(savedInstanceState);
  }

  private void initialiseGestures(MapboxMapOptions options) {
    setZoomGesturesEnabled(options.getZoomGesturesEnabled());
    setZoomGestureChangeAllowed(options.getZoomGesturesEnabled());
    setScrollGesturesEnabled(options.getScrollGesturesEnabled());
    setScrollGestureChangeAllowed(options.getScrollGesturesEnabled());
    setRotateGesturesEnabled(options.getRotateGesturesEnabled());
    setRotateGestureChangeAllowed(options.getRotateGesturesEnabled());
    setTiltGesturesEnabled(options.getTiltGesturesEnabled());
    setTiltGestureChangeAllowed(options.getTiltGesturesEnabled());
    setZoomControlsEnabled(options.getZoomControlsEnabled());
    setDoubleTapGesturesEnabled(options.getDoubleTapGesturesEnabled());
    setDoubleTapGestureChangeAllowed(options.getDoubleTapGesturesEnabled());
  }

  private void saveGestures(Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_ZOOM_ENABLED, isZoomGesturesEnabled());
    outState.putBoolean(MapboxConstants.STATE_ZOOM_ENABLED_CHANGE, isZoomGestureChangeAllowed());
    outState.putBoolean(MapboxConstants.STATE_SCROLL_ENABLED, isScrollGesturesEnabled());
    outState.putBoolean(MapboxConstants.STATE_SCROLL_ENABLED_CHANGE, isScrollGestureChangeAllowed());
    outState.putBoolean(MapboxConstants.STATE_ROTATE_ENABLED, isRotateGesturesEnabled());
    outState.putBoolean(MapboxConstants.STATE_ROTATE_ENABLED_CHANGE, isRotateGestureChangeAllowed());
    outState.putBoolean(MapboxConstants.STATE_TILT_ENABLED, isTiltGesturesEnabled());
    outState.putBoolean(MapboxConstants.STATE_TILT_ENABLED_CHANGE, isTiltGestureChangeAllowed());
    outState.putBoolean(MapboxConstants.STATE_DOUBLE_TAP_ENABLED, isDoubleTapGesturesEnabled());
    outState.putBoolean(MapboxConstants.STATE_DOUBLE_TAP_ENABLED_CHANGE, isDoubleTapGestureChangeAllowed());
  }

  private void restoreGestures(Bundle savedInstanceState) {
    setZoomGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_ENABLED));
    setZoomGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_ENABLED_CHANGE));
    setScrollGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_SCROLL_ENABLED));
    setScrollGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_SCROLL_ENABLED_CHANGE));
    setRotateGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ROTATE_ENABLED));
    setRotateGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_ROTATE_ENABLED_CHANGE));
    setTiltGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_TILT_ENABLED));
    setTiltGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_TILT_ENABLED_CHANGE));
    setDoubleTapGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_DOUBLE_TAP_ENABLED));
    setDoubleTapGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_DOUBLE_TAP_ENABLED_CHANGE));
  }

  private void initialiseCompass(MapboxMapOptions options, Resources resources) {
    setCompassEnabled(options.getCompassEnabled());
    setCompassGravity(options.getCompassGravity());
    int[] compassMargins = options.getCompassMargins();
    if (compassMargins != null) {
      setCompassMargins(compassMargins[0], compassMargins[1], compassMargins[2], compassMargins[3]);
    } else {
      int tenDp = (int) resources.getDimension(R.dimen.mapbox_four_dp);
      setCompassMargins(tenDp, tenDp, tenDp, tenDp);
    }
    setCompassFadeFacingNorth(options.getCompassFadeFacingNorth());
    if (options.getCompassImage() == null) {
      options.compassImage(ResourcesCompat.getDrawable(resources, R.drawable.mapbox_compass_icon, null));
    }
    setCompassImage(options.getCompassImage());
  }

  private void saveCompass(Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_COMPASS_ENABLED, isCompassEnabled());
    outState.putInt(MapboxConstants.STATE_COMPASS_GRAVITY, getCompassGravity());
    outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT, getCompassMarginLeft());
    outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP, getCompassMarginTop());
    outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM, getCompassMarginBottom());
    outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT, getCompassMarginRight());
    outState.putBoolean(MapboxConstants.STATE_COMPASS_FADE_WHEN_FACING_NORTH, isCompassFadeWhenFacingNorth());
    outState.putByteArray(MapboxConstants.STATE_COMPASS_IMAGE_BITMAP,
      BitmapUtils.getByteArrayFromDrawable(getCompassImage()));
  }

  private void restoreCompass(Bundle savedInstanceState) {
    setCompassEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_COMPASS_ENABLED));
    setCompassGravity(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_GRAVITY));
    setCompassMargins(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT),
      savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP),
      savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT),
      savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM));
    setCompassFadeFacingNorth(savedInstanceState.getBoolean(MapboxConstants.STATE_COMPASS_FADE_WHEN_FACING_NORTH));
    setCompassImage(BitmapUtils.getDrawableFromByteArray(
      compassView.getContext(), savedInstanceState.getByteArray(MapboxConstants.STATE_COMPASS_IMAGE_BITMAP)));
  }

  private void initialiseLogo(MapboxMapOptions options, Resources resources) {
    setLogoEnabled(options.getLogoEnabled());
    setLogoGravity(options.getLogoGravity());
    setLogoMargins(resources, options.getLogoMargins());
  }

  private void setLogoMargins(Resources resources, int[] logoMargins) {
    if (logoMargins != null) {
      setLogoMargins(logoMargins[0], logoMargins[1], logoMargins[2], logoMargins[3]);
    } else {
      // user did not specify margins when programmatically creating a map
      int fourDp = (int) resources.getDimension(R.dimen.mapbox_four_dp);
      setLogoMargins(fourDp, fourDp, fourDp, fourDp);
    }
  }

  private void saveLogo(Bundle outState) {
    outState.putInt(MapboxConstants.STATE_LOGO_GRAVITY, getLogoGravity());
    outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_LEFT, getLogoMarginLeft());
    outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_TOP, getLogoMarginTop());
    outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_RIGHT, getLogoMarginRight());
    outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_BOTTOM, getLogoMarginBottom());
    outState.putBoolean(MapboxConstants.STATE_LOGO_ENABLED, isLogoEnabled());
  }

  private void restoreLogo(Bundle savedInstanceState) {
    setLogoEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_LOGO_ENABLED));
    setLogoGravity(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_GRAVITY));
    setLogoMargins(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_LEFT),
      savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_TOP),
      savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_RIGHT),
      savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_BOTTOM));
  }

  private void initialiseAttribution(Context context, MapboxMapOptions options) {
    setAttributionEnabled(options.getAttributionEnabled());
    setAttributionGravity(options.getAttributionGravity());
    setAttributionMargins(context, options.getAttributionMargins());
    int attributionTintColor = options.getAttributionTintColor();
    setAttributionTintColor(attributionTintColor != -1
      ? attributionTintColor : ColorUtils.getPrimaryColor(context));
  }

  private void setAttributionMargins(Context context, int[] attributionMargins) {
    if (attributionMargins != null) {
      setAttributionMargins(attributionMargins[0], attributionMargins[1],
        attributionMargins[2], attributionMargins[3]);
    } else {
      // user did not specify margins when programmatically creating a map
      Resources resources = context.getResources();
      int margin = (int) resources.getDimension(R.dimen.mapbox_four_dp);
      int leftMargin = (int) resources.getDimension(R.dimen.mapbox_ninety_two_dp);
      setAttributionMargins(leftMargin, margin, margin, margin);
    }
  }

  private void saveAttribution(Bundle outState) {
    outState.putInt(MapboxConstants.STATE_ATTRIBUTION_GRAVITY, getAttributionGravity());
    outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_LEFT, getAttributionMarginLeft());
    outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_TOP, getAttributionMarginTop());
    outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_RIGHT, getAttributionMarginRight());
    outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_BOTTOM, getAttributionMarginBottom());
    outState.putBoolean(MapboxConstants.STATE_ATTRIBUTION_ENABLED, isAttributionEnabled());
  }

  private void restoreAttribution(Bundle savedInstanceState) {
    setAttributionEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ATTRIBUTION_ENABLED));
    setAttributionGravity(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_GRAVITY));
    setAttributionMargins(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_LEFT),
      savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_TOP),
      savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_RIGHT),
      savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_BOTTOM));
  }

  private void initialiseZoomControl(Context context) {
    if (!context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
      setZoomControlsEnabled(true);
    }
  }

  private void saveZoomControl(Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_ZOOM_CONTROLS_ENABLED, isZoomControlsEnabled());
  }

  private void restoreZoomControl(Bundle savedInstanceState) {
    setZoomControlsEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_CONTROLS_ENABLED));
  }

  /**
   * <p>
   * Enables or disables the compass. The compass is an icon on the map that indicates the
   * direction of north on the map. When a user clicks
   * the compass, the camera orients itself to its default orientation and fades away shortly
   * after. If disabled, the compass will never be displayed.
   * </p>
   * By default, the compass is enabled.
   *
   * @param compassEnabled True to enable the compass; false to disable the compass.
   */
  public void setCompassEnabled(boolean compassEnabled) {
    compassView.setEnabled(compassEnabled);
  }

  /**
   * Returns whether the compass is enabled.
   *
   * @return True if the compass is enabled; false if the compass is disabled.
   */
  public boolean isCompassEnabled() {
    return compassView.isEnabled();
  }

  /**
   * <p>
   * Sets the gravity of the compass view. Use this to change the corner of the map view that the
   * compass is displayed in.
   * </p>
   * By default, the compass is in the top right corner.
   *
   * @param gravity Android SDK Gravity.
   */
  @UiThread
  public void setCompassGravity(int gravity) {
    setWidgetGravity(compassView, gravity);
  }

  /**
   * Enables or disables fading of the compass when facing north.
   * <p>
   * By default this feature is enabled
   * </p>
   *
   * @param compassFadeFacingNorth True to enable the fading animation; false to disable it
   */
  public void setCompassFadeFacingNorth(boolean compassFadeFacingNorth) {
    compassView.fadeCompassViewFacingNorth(compassFadeFacingNorth);
  }

  /**
   * Specifies the CompassView image.
   * <p>
   * By default this value is R.drawable.mapbox_compass_icon.
   * </p>
   *
   * @param compass the drawable to show as image compass
   */
  public void setCompassImage(Drawable compass) {
    compassView.setCompassImage(compass);
  }

  /**
   * Returns whether the compass performs a fading animation out when facing north.
   *
   * @return True if the compass will fade, false if it remains visible
   */
  public boolean isCompassFadeWhenFacingNorth() {
    return compassView.isFadeCompassViewFacingNorth();
  }

  /**
   * Returns the gravity value of the CompassView
   *
   * @return The gravity
   */
  public int getCompassGravity() {
    return ((FrameLayout.LayoutParams) compassView.getLayoutParams()).gravity;
  }

  /**
   * Sets the margins of the compass view. Use this to change the distance of the compass from the
   * map view edge.
   *
   * @param left   The left margin in pixels.
   * @param top    The top margin in pixels.
   * @param right  The right margin in pixels.
   * @param bottom The bottom margin in pixels.
   */
  @UiThread
  public void setCompassMargins(int left, int top, int right, int bottom) {
    setWidgetMargins(compassView, compassMargins, left, top, right, bottom);
  }

  /**
   * Returns the left side margin of CompassView
   *
   * @return The left margin in pixels
   */
  public int getCompassMarginLeft() {
    return compassMargins[0];
  }

  /**
   * Returns the top side margin of CompassView
   *
   * @return The top margin in pixels
   */
  public int getCompassMarginTop() {
    return compassMargins[1];
  }

  /**
   * Returns the right side margin of CompassView
   *
   * @return The right margin in pixels
   */
  public int getCompassMarginRight() {
    return compassMargins[2];
  }

  /**
   * Returns the bottom side margin of CompassView
   *
   * @return The bottom margin in pixels
   */
  public int getCompassMarginBottom() {
    return compassMargins[3];
  }

  /**
   * Get the current configured CompassView image.
   *
   * @return the drawable used as compass image
   */
  public Drawable getCompassImage() {
    return compassView.getCompassImage();
  }

  void update(@NonNull CameraPosition cameraPosition) {
    if (!isCompassEnabled()) {
      return;
    }

    double clockwiseBearing = -cameraPosition.bearing;
    compassView.update(clockwiseBearing);
  }

  /**
   * <p>
   * Enables or disables the Mapbox logo.
   * </p>
   * By default, the logo is enabled.
   *
   * @param enabled True to enable the logo; false to disable the logo.
   */
  public void setLogoEnabled(boolean enabled) {
    logoView.setVisibility(enabled ? View.VISIBLE : View.GONE);
  }

  /**
   * Returns whether the logo is enabled.
   *
   * @return True if the logo is enabled; false if the logo is disabled.
   */
  public boolean isLogoEnabled() {
    return logoView.getVisibility() == View.VISIBLE;
  }

  /**
   * <p>
   * Sets the gravity of the logo view. Use this to change the corner of the map view that the
   * Mapbox logo is displayed in.
   * </p>
   * By default, the logo is in the bottom left corner.
   *
   * @param gravity Android SDK Gravity.
   */
  public void setLogoGravity(int gravity) {
    setWidgetGravity(logoView, gravity);
  }

  /**
   * Returns the gravity value of the logo
   *
   * @return The gravity
   */
  public int getLogoGravity() {
    return ((FrameLayout.LayoutParams) logoView.getLayoutParams()).gravity;
  }

  /**
   * Sets the margins of the logo view. Use this to change the distance of the Mapbox logo from the
   * map view edge.
   *
   * @param left   The left margin in pixels.
   * @param top    The top margin in pixels.
   * @param right  The right margin in pixels.
   * @param bottom The bottom margin in pixels.
   */
  public void setLogoMargins(int left, int top, int right, int bottom) {
    setWidgetMargins(logoView, logoMargins, left, top, right, bottom);
  }

  /**
   * Returns the left side margin of the logo
   *
   * @return The left margin in pixels
   */
  public int getLogoMarginLeft() {
    return logoMargins[0];
  }

  /**
   * Returns the top side margin of the logo
   *
   * @return The top margin in pixels
   */
  public int getLogoMarginTop() {
    return logoMargins[1];
  }

  /**
   * Returns the right side margin of the logo
   *
   * @return The right margin in pixels
   */
  public int getLogoMarginRight() {
    return logoMargins[2];
  }

  /**
   * Returns the bottom side margin of the logo
   *
   * @return The bottom margin in pixels
   */
  public int getLogoMarginBottom() {
    return logoMargins[3];
  }

  /**
   * <p>
   * Enables or disables the attribution.
   * </p>
   * By default, the attribution is enabled.
   *
   * @param enabled True to enable the attribution; false to disable the attribution.
   */
  public void setAttributionEnabled(boolean enabled) {
    attributionsView.setVisibility(enabled ? View.VISIBLE : View.GONE);
  }

  /**
   * Returns whether the attribution is enabled.
   *
   * @return True if the attribution is enabled; false if the attribution is disabled.
   */
  public boolean isAttributionEnabled() {
    return attributionsView.getVisibility() == View.VISIBLE;
  }


  /**
   * Set a custom attribution dialog manager.
   * <p>
   * Set to null to reset to default behaviour.
   * </p>
   *
   * @param attributionDialogManager the manager class used for showing attribution
   */
  public void setAttributionDialogManager(AttributionDialogManager attributionDialogManager) {
    this.attributionDialogManager = attributionDialogManager;
  }

  /**
   * Get the custom attribution dialog manager.
   *
   * @return the active manager class used for showing attribution
   */
  public AttributionDialogManager getAttributionDialogManager() {
    return attributionDialogManager;
  }

  /**
   * <p>
   * Sets the gravity of the attribution.
   * </p>
   * By default, the attribution is in the bottom left corner next to the Mapbox logo.
   *
   * @param gravity Android SDK Gravity.
   */
  public void setAttributionGravity(int gravity) {
    setWidgetGravity(attributionsView, gravity);
  }

  /**
   * Returns the gravity value of the logo
   *
   * @return The gravity
   */
  public int getAttributionGravity() {
    return ((FrameLayout.LayoutParams) attributionsView.getLayoutParams()).gravity;
  }

  /**
   * Sets the margins of the attribution view.
   *
   * @param left   The left margin in pixels.
   * @param top    The top margin in pixels.
   * @param right  The right margin in pixels.
   * @param bottom The bottom margin in pixels.
   */
  public void setAttributionMargins(int left, int top, int right, int bottom) {
    setWidgetMargins(attributionsView, attributionsMargins, left, top, right, bottom);
  }

  /**
   * <p>
   * Sets the tint of the attribution view. Use this to change the color of the attribution.
   * </p>
   * By default, the logo is tinted with the primary color of your theme.
   *
   * @param tintColor Color to tint the attribution.
   */
  public void setAttributionTintColor(@ColorInt int tintColor) {
    // Check that the tint color being passed in isn't transparent.
    if (Color.alpha(tintColor) == 0) {
      ColorUtils.setTintList(attributionsView,
        ContextCompat.getColor(attributionsView.getContext(), R.color.mapbox_blue));
    } else {
      ColorUtils.setTintList(attributionsView, tintColor);
    }
  }

  /**
   * Returns the left side margin of the attribution view.
   *
   * @return The left margin in pixels
   */
  public int getAttributionMarginLeft() {
    return attributionsMargins[0];
  }

  /**
   * Returns the top side margin of the attribution view.
   *
   * @return The top margin in pixels
   */
  public int getAttributionMarginTop() {
    return attributionsMargins[1];
  }

  /**
   * Returns the right side margin of the attribution view.
   *
   * @return The right margin in pixels
   */
  public int getAttributionMarginRight() {
    return attributionsMargins[2];
  }

  /**
   * Returns the bottom side margin of the logo
   *
   * @return The bottom margin in pixels
   */
  public int getAttributionMarginBottom() {
    return attributionsMargins[3];
  }

  /**
   * <p>
   * Changes whether the user may rotate the map.
   * </p>
   * <p>
   * This setting controls only user interactions with the map. If you set the value to false,
   * you may still change the map location programmatically.
   * </p>
   * The default value is true.
   *
   * @param rotateGesturesEnabled If true, rotating is enabled.
   */
  public void setRotateGesturesEnabled(boolean rotateGesturesEnabled) {
    if (rotateGestureChangeAllowed) {
      this.rotateGesturesEnabled = rotateGesturesEnabled;
    }
  }

  /**
   * Returns whether the user may rotate the map.
   *
   * @return If true, rotating is enabled.
   */
  public boolean isRotateGesturesEnabled() {
    return rotateGesturesEnabled;
  }

  void setRotateGestureChangeAllowed(boolean rotateGestureChangeAllowed) {
    this.rotateGestureChangeAllowed = rotateGestureChangeAllowed;
  }

  boolean isRotateGestureChangeAllowed() {
    return rotateGestureChangeAllowed;
  }

  /**
   * <p>
   * Changes whether the user may tilt the map.
   * </p>
   * <p>
   * This setting controls only user interactions with the map. If you set the value to false,
   * you may still change the map location programmatically.
   * </p>
   * The default value is true.
   *
   * @param tiltGesturesEnabled If true, tilting is enabled.
   */
  public void setTiltGesturesEnabled(boolean tiltGesturesEnabled) {
    if (tiltGestureChangeAllowed) {
      this.tiltGesturesEnabled = tiltGesturesEnabled;
    }
  }

  /**
   * Returns whether the user may tilt the map.
   *
   * @return If true, tilting is enabled.
   */
  public boolean isTiltGesturesEnabled() {
    return tiltGesturesEnabled;
  }

  void setTiltGestureChangeAllowed(boolean tiltGestureChangeAllowed) {
    this.tiltGestureChangeAllowed = tiltGestureChangeAllowed;
  }

  boolean isTiltGestureChangeAllowed() {
    return tiltGestureChangeAllowed;
  }

  /**
   * <p>
   * Changes whether the user may zoom the map.
   * </p>
   * <p>
   * This setting controls only user interactions with the map. If you set the value to false,
   * you may still change the map location programmatically.
   * </p>
   * The default value is true.
   *
   * @param zoomGesturesEnabled If true, zooming is enabled.
   */
  public void setZoomGesturesEnabled(boolean zoomGesturesEnabled) {
    if (zoomGestureChangeAllowed) {
      this.zoomGesturesEnabled = zoomGesturesEnabled;
    }
  }

  /**
   * Returns whether the user may zoom the map.
   *
   * @return If true, zooming is enabled.
   */
  public boolean isZoomGesturesEnabled() {
    return zoomGesturesEnabled;
  }

  void setZoomGestureChangeAllowed(boolean zoomGestureChangeAllowed) {
    this.zoomGestureChangeAllowed = zoomGestureChangeAllowed;
  }

  boolean isZoomGestureChangeAllowed() {
    return zoomGestureChangeAllowed;
  }

  /**
   * <p>
   * Sets whether the zoom controls are enabled.
   * If enabled, the zoom controls are a pair of buttons
   * (one for zooming in, one for zooming out) that appear on the screen.
   * When pressed, they cause the camera to zoom in (or out) by one zoom level.
   * If disabled, the zoom controls are not shown.
   * </p>
   * By default the zoom controls are enabled if the device is only single touch capable;
   *
   * @param zoomControlsEnabled If true, the zoom controls are enabled.
   */
  public void setZoomControlsEnabled(boolean zoomControlsEnabled) {
    this.zoomControlsEnabled = zoomControlsEnabled;
  }

  /**
   * Gets whether the zoom controls are enabled.
   *
   * @return If true, the zoom controls are enabled.
   */
  public boolean isZoomControlsEnabled() {
    return zoomControlsEnabled;
  }

  /**
   * <p>
   * Changes whether the user may zoom the map with a double tap.
   * </p>
   * <p>
   * This setting controls only user interactions with the map. If you set the value to false,
   * you may still change the map location programmatically.
   * </p>
   * The default value is true.
   *
   * @param doubleTapGesturesEnabled If true, zooming with a double tap is enabled.
   */
  public void setDoubleTapGesturesEnabled(boolean doubleTapGesturesEnabled) {
    if (doubleTapGestureChangeAllowed) {
      this.doubleTapGesturesEnabled = doubleTapGesturesEnabled;
    }
  }

  /**
   * Returns whether the user may zoom the map with a double tap.
   *
   * @return If true, zooming with a double tap is enabled.
   */
  public boolean isDoubleTapGesturesEnabled() {
    return doubleTapGesturesEnabled;
  }

  void setDoubleTapGestureChangeAllowed(boolean doubleTapGestureChangeAllowed) {
    this.doubleTapGestureChangeAllowed = doubleTapGestureChangeAllowed;
  }

  boolean isDoubleTapGestureChangeAllowed() {
    return doubleTapGestureChangeAllowed;
  }

  private void restoreDeselectMarkersOnTap(Bundle savedInstanceState) {
    setDeselectMarkersOnTap(savedInstanceState.getBoolean(MapboxConstants.STATE_DESELECT_MARKER_ON_TAP));
  }

  private void saveDeselectMarkersOnTap(Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_DESELECT_MARKER_ON_TAP, isDeselectMarkersOnTap());
  }

  /**
   * Gets whether the markers are automatically deselected (and therefore, their infowindows
   * closed) when a map tap is detected.
   *
   * @return If true, markers are deselected on a map tap.
   */
  public boolean isDeselectMarkersOnTap() {
    return deselectMarkersOnTap;
  }

  /**
   * Sets whether the markers are automatically deselected (and therefore, their infowindows
   * closed) when a map tap is detected.
   *
   * @param deselectMarkersOnTap determines if markers should be deslected on tap
   */
  public void setDeselectMarkersOnTap(boolean deselectMarkersOnTap) {
    this.deselectMarkersOnTap = deselectMarkersOnTap;
  }

  /**
   * <p>
   * Changes whether the user may scroll around the map.
   * </p>
   * <p>
   * This setting controls only user interactions with the map. If you set the value to false,
   * you may still change the map location programmatically.
   * </p>
   * The default value is true.
   *
   * @param scrollGesturesEnabled If true, scrolling is enabled.
   */
  public void setScrollGesturesEnabled(boolean scrollGesturesEnabled) {
    if (scrollGestureChangeAllowed) {
      this.scrollGesturesEnabled = scrollGesturesEnabled;
    }
  }

  /**
   * Returns whether the user may scroll around the map.
   *
   * @return If true, scrolling is enabled.
   */
  public boolean isScrollGesturesEnabled() {
    return scrollGesturesEnabled;
  }

  void setScrollGestureChangeAllowed(boolean scrollGestureChangeAllowed) {
    this.scrollGestureChangeAllowed = scrollGestureChangeAllowed;
  }

  boolean isScrollGestureChangeAllowed() {
    return scrollGestureChangeAllowed;
  }

  /**
   * <p>
   * Sets the preference for whether all gestures should be enabled or disabled.
   * </p>
   * <p>
   * This setting controls only user interactions with the map. If you set the value to false,
   * you may still change the map location programmatically.
   * </p>
   * The default value is true.
   *
   * @param enabled If true, all gestures are available; otherwise, all gestures are disabled.
   * @see #setZoomGesturesEnabled(boolean) )
   * @see #setScrollGesturesEnabled(boolean)
   * @see #setRotateGesturesEnabled(boolean)
   * @see #setTiltGesturesEnabled(boolean)
   */
  public void setAllGesturesEnabled(boolean enabled) {
    setScrollGesturesEnabled(enabled);
    setRotateGesturesEnabled(enabled);
    setTiltGesturesEnabled(enabled);
    setZoomGesturesEnabled(enabled);
    setDoubleTapGesturesEnabled(enabled);
  }

  private void saveFocalPoint(Bundle outState) {
    outState.putParcelable(MapboxConstants.STATE_USER_FOCAL_POINT, getFocalPoint());
  }

  private void restoreFocalPoint(Bundle savedInstanceState) {
    PointF pointF = savedInstanceState.getParcelable(MapboxConstants.STATE_USER_FOCAL_POINT);
    if (pointF != null) {
      setFocalPoint(pointF);
    }
  }

  /**
   * Sets the focal point used as center for a gesture
   *
   * @param focalPoint the focal point to be used.
   */
  public void setFocalPoint(@Nullable PointF focalPoint) {
    this.userProvidedFocalPoint = focalPoint;
    focalPointChangeListener.onFocalPointChanged(focalPoint);
  }

  /**
   * Returns the gesture focal point
   *
   * @return The focal point
   */
  public PointF getFocalPoint() {
    return userProvidedFocalPoint;
  }

  /**
   * Returns the measured height of the MapView
   *
   * @return height in pixels
   */
  public float getHeight() {
    return projection.getHeight();
  }

  /**
   * Returns the measured width of the MapView
   *
   * @return widht in pixels
   */
  public float getWidth() {
    return projection.getWidth();
  }

  float getPixelRatio() {
    return pixelRatio;
  }

  /**
   * Invalidates the ViewSettings instances shown on top of the MapView
   */
  public void invalidate() {
    setLogoMargins(getLogoMarginLeft(), getLogoMarginTop(), getLogoMarginRight(), getLogoMarginBottom());
    setCompassMargins(getCompassMarginLeft(), getCompassMarginTop(), getCompassMarginRight(), getCompassMarginBottom());
    setAttributionMargins(getAttributionMarginLeft(), getAttributionMarginTop(), getAttributionMarginRight(),
      getAttributionMarginBottom());
  }

  private void setWidgetGravity(@NonNull final View view, int gravity) {
    FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) view.getLayoutParams();
    layoutParams.gravity = gravity;
    view.setLayoutParams(layoutParams);
  }

  private void setWidgetMargins(@NonNull final View view, int[] initMargins, int left, int top, int right, int bottom) {
    // keep state of initially set margins
    initMargins[0] = left;
    initMargins[1] = top;
    initMargins[2] = right;
    initMargins[3] = bottom;

    // convert initial margins with padding
    int[] contentPadding = projection.getContentPadding();
    FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) view.getLayoutParams();
    left += contentPadding[0];
    top += contentPadding[1];
    right += contentPadding[2];
    bottom += contentPadding[3];
    layoutParams.setMargins(left, top, right, bottom);

    // support RTL
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
      layoutParams.setMarginStart(left);
      layoutParams.setMarginEnd(right);
    }

    view.setLayoutParams(layoutParams);
  }
}
