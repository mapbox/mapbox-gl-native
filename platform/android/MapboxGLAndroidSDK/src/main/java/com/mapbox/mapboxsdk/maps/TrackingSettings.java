package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

/**
 * Settings for the user location and bearing tracking of a MapboxMap.
 */
public class TrackingSettings {

    private MapView mapView;
    private UiSettings uiSettings;
    private boolean dismissLocationTrackingOnGesture = true;
    private boolean dismissBearingTrackingOnGesture = true;

    @MyLocationTracking.Mode
    private int myLocationTrackingMode;

    @MyBearingTracking.Mode
    private int myBearingTrackingMode;

    TrackingSettings(@NonNull MapView mapView, UiSettings uiSettings) {
        this.mapView = mapView;
        this.uiSettings = uiSettings;
    }

    /**
     * <p>
     * Set the current my location tracking mode.
     * </p>
     * <p>
     * Will enable my location if not active.
     * </p>
     * See {@link MyLocationTracking} for different values.
     *
     * @param myLocationTrackingMode The location tracking mode to be used.
     * @throws SecurityException if no suitable permission is present
     * @see MyLocationTracking
     */
    @UiThread
    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        this.myLocationTrackingMode = myLocationTrackingMode;
        mapView.setMyLocationTrackingMode(myLocationTrackingMode);
    }

    /**
     * Returns the current user location tracking mode.
     *
     * @return The current user location tracking mode.
     * One of the values from {@link MyLocationTracking.Mode}.
     * @see MyLocationTracking.Mode
     */
    @UiThread
    @MyLocationTracking.Mode
    public int getMyLocationTrackingMode() {
        return myLocationTrackingMode;
    }

    /**
     * <p>
     * Set the current my bearing tracking mode.
     * </p>
     * Shows the direction the user is heading.
     * <p>
     * When location tracking is disabled the direction of {@link MyLocationView} is rotated. When
     * location tracking is enabled the {@link MapView} is rotated based on the bearing value.
     * </p>
     * See {@link MyBearingTracking} for different values.
     *
     * @param myBearingTrackingMode The bearing tracking mode to be used.
     * @throws SecurityException if no suitable permission is present
     * @see MyBearingTracking
     */
    @UiThread
    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        this.myBearingTrackingMode = myBearingTrackingMode;
        mapView.setMyBearingTrackingMode(myBearingTrackingMode);
    }

    /**
     * Returns the current user bearing tracking mode.
     * See {@link MyBearingTracking} for possible return values.
     *
     * @return the current user bearing tracking mode.
     * @see MyBearingTracking
     */
    @UiThread
    @MyBearingTracking.Mode
    public int getMyBearingTrackingMode() {
        return myBearingTrackingMode;
    }

    /**
     * Returns if the tracking modes will be dismissed when a gesture occurs.
     *
     * @return True to indicate the tracking modes will be dismissed.
     * @deprecated use @link #isAllDismissTrackingOnGestureinstead
     */
    @Deprecated
    public boolean isDismissTrackingOnGesture() {
        return dismissLocationTrackingOnGesture && dismissBearingTrackingOnGesture;
    }

    /**
     * Returns if all tracking modes will be dismissed when a gesture occurs.
     *
     * @return True to indicate that location and bearing tracking will be dismissed.
     */
    public boolean isAllDismissTrackingOnGesture() {
        return dismissLocationTrackingOnGesture && dismissBearingTrackingOnGesture;
    }

    /**
     * Set the dismissal of the tracking modes if a gesture occurs.
     *
     * @param dismissTrackingOnGesture True to dismiss the tracking modes.
     * @deprecated use @link #setDismissAllTrackingOnGesture instead
     */
    @Deprecated
    public void setDismissTrackingOnGesture(boolean dismissTrackingOnGesture) {
        setDismissAllTrackingOnGesture(dismissTrackingOnGesture);
    }

    /**
     * Set the dismissal of the tracking modes if a gesture occurs.
     *
     * @param dismissTrackingOnGesture True to dismiss all the tracking modes.
     */
    public void setDismissAllTrackingOnGesture(boolean dismissTrackingOnGesture) {
        dismissLocationTrackingOnGesture = dismissTrackingOnGesture;
        dismissBearingTrackingOnGesture = dismissTrackingOnGesture;
    }

    /**
     * Set the dismissal of the tracking modes if a gesture occurs.
     *
     * @param dismissLocationTrackingOnGesture True to dismiss the location tracking mode.
     */
    public void setDismissLocationTrackingOnGesture(boolean dismissLocationTrackingOnGesture) {
        this.dismissLocationTrackingOnGesture = dismissLocationTrackingOnGesture;
    }

    /**
     * Returns if the location tracking will be disabled when a gesture occurs
     *
     * @return True if location tracking will be disabled.
     */
    public boolean isDismissLocationTrackingOnGesture() {
        return dismissLocationTrackingOnGesture;
    }

    /**
     * Set the dismissal of the bearing tracking modes if a gesture occurs.
     *
     * @param dismissBearingTrackingOnGesture True to dimsiss the bearinf tracking mode
     */
    public void setDismissBearingTrackingOnGesture(boolean dismissBearingTrackingOnGesture) {
        this.dismissBearingTrackingOnGesture = dismissBearingTrackingOnGesture;
    }

    /**
     * Returns if bearing will disabled when a gesture occurs
     *
     * @return True if bearing tracking will be disabled
     */
    public boolean isDismissBearingTrackingOnGesture() {
        return dismissBearingTrackingOnGesture;
    }

    /**
     * Returns if location tracking is disabled
     *
     * @return True if location tracking is disabled.
     */
    public boolean isLocationTrackingDisabled() {
        return myLocationTrackingMode == MyLocationTracking.TRACKING_NONE;
    }

    /**
     * Returns if bearing tracking disabled
     *
     * @return True if bearing tracking is disabled.
     */
    public boolean isBearingTrackingDisabled() {
        return myBearingTrackingMode == MyBearingTracking.NONE;
    }

    /**
     * Returns if rotate gesture are currently enabled.
     *
     * @return True if rotate gestures are currently enabled.
     */
    public boolean isRotateGestureCurrentlyEnabled() {
        // rotate gestures are recognised if:
        //    The user settings are enabled AND;
        //    EITHER bearing tracking is dismissed on gesture OR there is no bearing tracking
        return uiSettings.isRotateGesturesEnabled() &&
                (dismissBearingTrackingOnGesture || myBearingTrackingMode == MyBearingTracking.NONE || (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE));
    }

    /**
     * Returns if scroll gesture are currently enabled.
     *
     * @return True if scroll gestures are currently enabled.
     */
    public boolean isScrollGestureCurrentlyEnabled() {
        return uiSettings.isScrollGesturesEnabled() &&
                (dismissLocationTrackingOnGesture || myLocationTrackingMode == MyLocationTracking.TRACKING_NONE);
    }

    /**
     * Reset the tracking modes as necessary. Location tracking is reset if the map center is changed,
     * bearing tracking if there is a rotation.
     *
     * @param translate
     * @param rotate
     */
    public void resetTrackingModesIfRequired(boolean translate, boolean rotate) {
        // if tracking is on, and we should dismiss tracking with gestures, and this is a scroll action, turn tracking off
        if (translate && !isLocationTrackingDisabled() && isDismissLocationTrackingOnGesture()) {
            setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
        }

        // reset bearing tracking only on rotate
        if (rotate && !isBearingTrackingDisabled() && isDismissBearingTrackingOnGesture()) {
            setMyBearingTrackingMode(MyBearingTracking.NONE);
        }
    }

    public void resetTrackingModesIfRequired(CameraPosition cameraPosition) {
        resetTrackingModesIfRequired(cameraPosition.target != null, cameraPosition.bearing != -1);
    }
}
