package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

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
        validateGesturesForLocationTrackingMode();
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
     * When location tracking is disabled the direction of {@link MyLocationView}  is rotated
     * When location tracking is enabled the {@link MapView} is rotated based on bearing value.
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
        validateGesturesForBearingTrackingMode();
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
        validateAllGesturesForTrackingModes();
    }

    /**
     * Set the dismissal of the tracking modes if a gesture occurs.
     *
     * @param dismissLocationTrackingOnGesture True to dismiss the location tracking mode.
     */
    public void setDismissLocationTrackingOnGesture(boolean dismissLocationTrackingOnGesture) {
        this.dismissLocationTrackingOnGesture = dismissLocationTrackingOnGesture;
        validateGesturesForLocationTrackingMode();
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
        validateGesturesForBearingTrackingMode();
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
     * Retyrns uf bearing tracking is disabled
     *
     * @return True if bearing tracking will be disabled.
     */
    public boolean isBearingTrackingDisabled() {
        return myBearingTrackingMode == MyBearingTracking.NONE;
    }

    private void validateAllGesturesForTrackingModes() {
        validateGesturesForBearingTrackingMode();
        validateGesturesForLocationTrackingMode();
    }

    private void validateGesturesForLocationTrackingMode() {
        int myLocationTrackingMode = getMyLocationTrackingMode();
        if (!dismissLocationTrackingOnGesture) {
            if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                uiSettings.setScrollGesturesEnabled(true);
            } else {
                uiSettings.setScrollGesturesEnabled(false);
            }
        } else {
            uiSettings.setScrollGesturesEnabled(true);
        }
    }

    private void validateGesturesForBearingTrackingMode() {
        int myBearingTrackingMode = getMyBearingTrackingMode();
        if (!dismissBearingTrackingOnGesture) {
            if (myBearingTrackingMode == MyBearingTracking.NONE || myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                uiSettings.setRotateGesturesEnabled(true);
            } else {
                uiSettings.setRotateGesturesEnabled(false);
            }
        } else {
            uiSettings.setRotateGesturesEnabled(true);
        }
    }
}
