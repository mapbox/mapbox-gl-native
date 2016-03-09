package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;

/**
 * Settings for the user location and bearing tracking of a MapboxMap.
 */
public class TrackingSettings {

    private MapView mapView;
    private UiSettings uiSettings;
    private boolean dismissLocationTrackingOnGesture = true;
    private boolean dismissBearingTrackingOnGesture = true;

    @MyLocationTracking.Mode
    private int mMyLocationTrackingMode;

    @MyBearingTracking.Mode
    private int mMyBearingTrackingMode;

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
        mMyLocationTrackingMode = myLocationTrackingMode;
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
        return mMyLocationTrackingMode;
    }

    /**
     * <p>
     * Set the current my bearing tracking mode.
     * </p>
     * Shows the direction the user is heading.
     * <p>
     * When location tracking is disabled the direction of {@link UserLocationView}  is rotated
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
        mMyBearingTrackingMode = myBearingTrackingMode;
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
        return mMyBearingTrackingMode;
    }

    public boolean isDismissTrackingOnGesture() {
        return dismissLocationTrackingOnGesture && dismissBearingTrackingOnGesture;
    }

    public void setDismissTrackingOnGesture(boolean dismissTrackingOnGesture) {
        dismissLocationTrackingOnGesture = dismissTrackingOnGesture;
        dismissBearingTrackingOnGesture = dismissTrackingOnGesture;
        validateGesturesForTrackingModes();
    }

    private void validateGesturesForTrackingModes() {
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
            if (myBearingTrackingMode == MyBearingTracking.NONE) {
                uiSettings.setRotateGesturesEnabled(true);
            } else {
                uiSettings.setRotateGesturesEnabled(false);
            }
        } else {
            uiSettings.setRotateGesturesEnabled(true);
        }
    }

    public void setDismissLocationTrackingOnGesture(boolean dismissLocationTrackingOnGesture) {
        this.dismissLocationTrackingOnGesture = dismissLocationTrackingOnGesture;
        validateGesturesForLocationTrackingMode();
    }

    public boolean isDismissLocationTrackingOnGesture() {
        return dismissLocationTrackingOnGesture;
    }

    public void setDismissBearingTrackingOnGesture(boolean dismissBearingTrackingOnGesture) {
        this.dismissBearingTrackingOnGesture = dismissBearingTrackingOnGesture;
        validateGesturesForBearingTrackingMode();
    }

    public boolean isDismissBearingTrackingOnGesture() {
        return dismissBearingTrackingOnGesture;
    }

    public boolean isLocationTrackingDisabled(){
        return mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE;
    }
}
