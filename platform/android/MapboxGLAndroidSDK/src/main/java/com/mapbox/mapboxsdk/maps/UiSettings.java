package com.mapbox.mapboxsdk.maps;

import android.support.annotation.UiThread;
import android.view.Gravity;
import android.view.View;

/**
 * Settings for the user interface of a MapboxMap. To obtain this interface, call getUiSettings().
 */
public class UiSettings {

    private MapboxMap mapboxMap;

    private boolean compassEnabled;
    private int compassGravity;
    private int[] compassMargins;

    private boolean logoEnabled;
    private int logoGravity;
    private int[] logoMargins;

    private boolean attributionEnabled;
    private int attributionGravity;
    private int[] attributionMargins;

    private boolean rotateGesturesEnabled;
    private boolean tiltGesturesEnabled;
    private boolean zoomGesturesEnabled;
    private boolean zoomControlsEnabled;
    private boolean scrollGesturesEnabled;

    UiSettings(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
        this.compassMargins = new int[4];
        this.attributionMargins = new int[4];
        this.logoMargins = new int[4];
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
        this.compassEnabled = compassEnabled;
        this.mapboxMap.setCompassEnabled(compassEnabled);
    }

    /**
     * Returns whether the compass is enabled.
     *
     * @return True if the compass is enabled; false if the compass is disabled.
     */
    public boolean isCompassEnabled() {
        return compassEnabled;
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
        this.compassGravity = gravity;
        this.mapboxMap.setCompassGravity(gravity);
    }

    /**
     * Returns the gravity value of the CompassView
     *
     * @return The gravity
     */
    public int getCompassGravity() {
        return compassGravity;
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
        this.compassMargins = new int[]{left, top, right, bottom};
        this.mapboxMap.setCompassMargins(left, top, right, bottom);
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
     * <p>
     * Enables or disables the Mapbox logo.
     * </p>
     * By default, the compass is enabled.
     *
     * @param enabled True to enable the logo; false to disable the logo.
     */
    public void setLogoEnabled(boolean enabled) {
        this.logoEnabled = enabled;
        this.mapboxMap.setLogoVisibility(enabled ? View.VISIBLE : View.GONE);
    }

    /**
     * Returns whether the logo is enabled.
     *
     * @return True if the logo is enabled; false if the logo is disabled.
     */
    public boolean isLogoEnabled() {
        return logoEnabled;
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
        this.logoGravity = gravity;
        this.mapboxMap.setLogoGravity(gravity);
    }

    /**
     * Returns the gravity value of the logo
     *
     * @return The gravity
     */
    public int getLogoGravity() {
        return logoGravity;
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
        this.logoMargins = new int[]{left, top, right, bottom};
        this.mapboxMap.setLogoMargins(left, top, right, bottom);
    }

    /**
     * Returns the left side margin of the logo
     *
     * @return The left margin in pixels
     */
    public int getLogoMarginLeft(){
        return logoMargins[0];
    }

    /**
     * Returns the top side margin of the logo
     *
     * @return The top margin in pixels
     */
    public int getLogoMarginTop(){
        return logoMargins[1];
    }

    /**
     * Returns the right side margin of the logo
     *
     * @return The right margin in pixels
     */
    public int getLogoMarginRight(){
        return logoMargins[2];
    }

    /**
     * Returns the bottom side margin of the logo
     *
     * @return The bottom margin in pixels
     */
    public int getLogoMarginBottom(){
        return logoMargins[3];
    }

    /**
     * <p>
     * Enables or disables the Mapbox logo.
     * </p>
     * By default, the compass is enabled.
     *
     * @param enabled True to enable the logo; false to disable the logo.
     */
    public void setAttributionEnabled(boolean enabled) {
        this.attributionEnabled = enabled;
        this.mapboxMap.setAttributionVisibility(enabled ? View.VISIBLE : View.GONE);
    }

    /**
     * Returns whether the logo is enabled.
     *
     * @return True if the logo is enabled; false if the logo is disabled.
     */
    public boolean isAttributionEnabled() {
        return attributionEnabled;
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
    public void setAttributionGravity(int gravity) {
        this.attributionGravity = gravity;
        this.mapboxMap.setAttributionGravity(gravity);
    }

    /**
     * Returns the gravity value of the logo
     *
     * @return The gravity
     */
    public int getAttributionGravity() {
        return attributionGravity;
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
    public void setAttributionMargins(int left, int top, int right, int bottom) {
        this.attributionMargins = new int[]{left, top, right, bottom};
        this.mapboxMap.setAttributionMargins(left, top, right, bottom);
    }

    /**
     * Returns the left side margin of the logo
     *
     * @return The left margin in pixels
     */
    public int getAttributionMarginLeft(){
        return attributionMargins[0];
    }

    /**
     * Returns the top side margin of the logo
     *
     * @return The top margin in pixels
     */
    public int getAttributionMarginTop(){
        return attributionMargins[1];
    }

    /**
     * Returns the right side margin of the logo
     *
     * @return The right margin in pixels
     */
    public int getAttributionMarginRight(){
        return attributionMargins[2];
    }

    /**
     * Returns the bottom side margin of the logo
     *
     * @return The bottom margin in pixels
     */
    public int getAttributionMarginBottom(){
        return attributionMargins[3];
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
        this.rotateGesturesEnabled = rotateGesturesEnabled;
    }

    /**
     * Returns whether the user may rotate the map.
     *
     * @return If true, rotating is enabled.
     */
    public boolean isRotateGesturesEnabled() {
        return rotateGesturesEnabled;
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
        this.tiltGesturesEnabled = tiltGesturesEnabled;
    }

    /**
     * Returns whether the user may tilt the map.
     *
     * @return If true, tilting is enabled.
     */
    public boolean isTiltGesturesEnabled() {
        return tiltGesturesEnabled;
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
        this.zoomGesturesEnabled = zoomGesturesEnabled;
    }

    /**
     * Returns whether the user may zoom the map.
     *
     * @return If true, zooming is enabled.
     */
    public boolean isZoomGesturesEnabled() {
        return zoomGesturesEnabled;
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
        this.scrollGesturesEnabled = scrollGesturesEnabled;
    }

    /**
     * Returns whether the user may scroll around the map.
     *
     * @return If true, scrolling is enabled.
     */
    public boolean isScrollGesturesEnabled() {
        return scrollGesturesEnabled;
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
}
