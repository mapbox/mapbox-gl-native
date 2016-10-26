package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.view.Gravity;
import android.view.View;

/**
 * Settings for the user interface of a MapboxMap. To obtain this interface, call getUiSettings().
 */
public class UiSettings {

    private MapView mapView;

    private CompassViewSettings compassSettings;
    private ViewSettings logoSettings;
    private ViewSettings attributionSettings;

    private boolean rotateGesturesEnabled = true;
    private boolean rotateGestureChangeAllowed = true;

    private boolean tiltGesturesEnabled = true;
    private boolean tiltGestureChangeAllowed = true;

    private boolean zoomGesturesEnabled = true;
    private boolean zoomGestureChangeAllowed = true;

    private boolean scrollGesturesEnabled = true;
    private boolean scrollGestureChangeAllowed = true;

    private boolean zoomControlsEnabled;

    private boolean deselectMarkersOnTap = true;

    private PointF focalPoint;

    UiSettings(@NonNull MapView mapView) {
        this.mapView = mapView;
        this.compassSettings = new CompassViewSettings();
        this.logoSettings = new ViewSettings();
        this.attributionSettings = new ViewSettings();
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
        compassSettings.setEnabled(compassEnabled);
        mapView.setCompassEnabled(compassEnabled);
    }

    /**
     * Returns whether the compass is enabled.
     *
     * @return True if the compass is enabled; false if the compass is disabled.
     */
    public boolean isCompassEnabled() {
        return compassSettings.isEnabled();
    }

    /**
     * <p>
     * Sets the gravity of the compass view. Use this to change the corner of the map view that the
     * compass is displayed in.
     * </p>
     * By default, the compass is in the top right corner.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
    @UiThread
    public void setCompassGravity(int gravity) {
        compassSettings.setGravity(gravity);
        mapView.setCompassGravity(gravity);
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
        compassSettings.setFadeFacingNorth(compassFadeFacingNorth);
        mapView.setCompassFadeFacingNorth(compassFadeFacingNorth);
    }

    /**
     * Returns whether the compass performs a fading animation out when facing north.
     *
     * @return True if the compass will fade, false if it remains visible
     */
    public boolean isCompassFadeWhenFacingNorth(){
        return compassSettings.isFadeFacingNorth();
    }

    /**
     * Returns the gravity value of the CompassView
     *
     * @return The gravity
     */
    public int getCompassGravity() {
        return compassSettings.getGravity();
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
        compassSettings.setMargins(new int[]{left, top, right, bottom});
        mapView.setCompassMargins(left, top, right, bottom);
    }

    /**
     * Returns the left side margin of CompassView
     *
     * @return The left margin in pixels
     */
    public int getCompassMarginLeft() {
        return compassSettings.getMargins()[0];
    }

    /**
     * Returns the top side margin of CompassView
     *
     * @return The top margin in pixels
     */
    public int getCompassMarginTop() {
        return compassSettings.getMargins()[1];
    }

    /**
     * Returns the right side margin of CompassView
     *
     * @return The right margin in pixels
     */
    public int getCompassMarginRight() {
        return compassSettings.getMargins()[2];
    }

    /**
     * Returns the bottom side margin of CompassView
     *
     * @return The bottom margin in pixels
     */
    public int getCompassMarginBottom() {
        return compassSettings.getMargins()[3];
    }

    /**
     * <p>
     * Enables or disables the Mapbox logo.
     * </p>
     * By default, the compass is enabled.
     *
     * @param enabled True to enable the logo; false to disable the logo.
     */
    public void setLogoEnabled(boolean enabled) {
        logoSettings.setEnabled(enabled);
        mapView.setLogoEnabled(enabled);
    }

    /**
     * Returns whether the logo is enabled.
     *
     * @return True if the logo is enabled; false if the logo is disabled.
     */
    public boolean isLogoEnabled() {
        return logoSettings.isEnabled();
    }

    /**
     * <p>
     * Sets the gravity of the logo view. Use this to change the corner of the map view that the
     * Mapbox logo is displayed in.
     * </p>
     * By default, the logo is in the bottom left corner.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
    public void setLogoGravity(int gravity) {
        logoSettings.setGravity(gravity);
        mapView.setLogoGravity(gravity);
    }

    /**
     * Returns the gravity value of the logo
     *
     * @return The gravity
     */
    public int getLogoGravity() {
        return logoSettings.getGravity();
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
        logoSettings.setMargins(new int[]{left, top, right, bottom});
        mapView.setLogoMargins(left, top, right, bottom);
    }

    /**
     * Returns the left side margin of the logo
     *
     * @return The left margin in pixels
     */
    public int getLogoMarginLeft() {
        return logoSettings.getMargins()[0];
    }

    /**
     * Returns the top side margin of the logo
     *
     * @return The top margin in pixels
     */
    public int getLogoMarginTop() {
        return logoSettings.getMargins()[1];
    }

    /**
     * Returns the right side margin of the logo
     *
     * @return The right margin in pixels
     */
    public int getLogoMarginRight() {
        return logoSettings.getMargins()[2];
    }

    /**
     * Returns the bottom side margin of the logo
     *
     * @return The bottom margin in pixels
     */
    public int getLogoMarginBottom() {
        return logoSettings.getMargins()[3];
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
        attributionSettings.setEnabled(enabled);
        mapView.setAttributionEnabled(enabled ? View.VISIBLE : View.GONE);
    }

    /**
     * Returns whether the attribution is enabled.
     *
     * @return True if the attribution is enabled; false if the attribution is disabled.
     */
    public boolean isAttributionEnabled() {
        return attributionSettings.isEnabled();
    }

    /**
     * <p>
     * Sets the gravity of the attribution.
     * </p>
     * By default, the attribution is in the bottom left corner next to the Mapbox logo.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
    public void setAttributionGravity(int gravity) {
        attributionSettings.setGravity(gravity);
        mapView.setAttributionGravity(gravity);
    }

    /**
     * Returns the gravity value of the logo
     *
     * @return The gravity
     */
    public int getAttributionGravity() {
        return attributionSettings.getGravity();
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
        attributionSettings.setMargins(new int[]{left, top, right, bottom});
        mapView.setAttributionMargins(left, top, right, bottom);
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
        attributionSettings.setTintColor(tintColor);
        mapView.setAtttibutionTintColor(tintColor);
    }

    /**
     * Returns the tint color value of the attribution view.
     *
     * @return The tint color
     */
    public int getAttributionTintColor() {
        return attributionSettings.getTintColor();
    }

    /**
     * Returns the left side margin of the attribution view.
     *
     * @return The left margin in pixels
     */
    public int getAttributionMarginLeft() {
        return attributionSettings.getMargins()[0];
    }

    /**
     * Returns the top side margin of the attribution view.
     *
     * @return The top margin in pixels
     */
    public int getAttributionMarginTop() {
        return attributionSettings.getMargins()[1];
    }

    /**
     * Returns the right side margin of the attribution view.
     *
     * @return The right margin in pixels
     */
    public int getAttributionMarginRight() {
        return attributionSettings.getMargins()[2];
    }

    /**
     * Returns the bottom side margin of the logo
     *
     * @return The bottom margin in pixels
     */
    public int getAttributionMarginBottom() {
        return attributionSettings.getMargins()[3];
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
    }

    /**
     * Sets the focal point used as center for a gesture
     *
     * @param focalPoint the focal point to be used.
     */
    public void setFocalPoint(@Nullable PointF focalPoint) {
        this.focalPoint = focalPoint;
        mapView.setFocalPoint(focalPoint);
    }

    /**
     * Returns the gesture focal point
     *
     * @return The focal point
     */
    public PointF getFocalPoint() {
        return focalPoint;
    }

    /**
     * Returns the measured height of the MapView
     *
     * @return height in pixels
     */
    public float getHeight() {
        return mapView.getMeasuredHeight();
    }

    /**
     * Returns the measured width of the MapView
     *
     * @return widht in pixels
     */
    public float getWidth() {
        return mapView.getMeasuredWidth();
    }

    /**
     * Invalidates the ViewSettings instances shown on top of the MapView
     */
    public void invalidate() {
        mapView.setLogoMargins(getLogoMarginLeft(), getLogoMarginTop(), getLogoMarginRight(), getLogoMarginBottom());
        mapView.setCompassMargins(getCompassMarginLeft(), getCompassMarginTop(), getCompassMarginRight(), getCompassMarginBottom());
        mapView.setAttributionMargins(getAttributionMarginLeft(), getAttributionMarginTop(), getAttributionMarginRight(), getAttributionMarginBottom());
    }
}
