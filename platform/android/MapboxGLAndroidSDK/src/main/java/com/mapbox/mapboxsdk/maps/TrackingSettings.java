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
    private boolean dismissTrackingOnGesture = true;

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
        validateGesturesForTrackingModes();
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
    }

    /**
     * Returns the current user bearing tracking mode.
     * See {@link MyBearingTracking} for possible return values.
     *
     * @return the current user bearing tracking mode.
     * @see MyBearingTracking
     */
    @UiThread
    @MyLocationTracking.Mode
    public int getMyBearingTrackingMode() {
        return mMyBearingTrackingMode;
    }

    public boolean isDismissTrackingOnGesture() {
        return dismissTrackingOnGesture;
    }

    public void setDismissTrackingOnGesture(boolean dismissTrackingOnGesture) {
        this.dismissTrackingOnGesture = dismissTrackingOnGesture;
        validateGesturesForTrackingModes();
    }

    private void validateGesturesForTrackingModes() {
        if (!dismissTrackingOnGesture) {
            int myLocationTrackingMode = getMyLocationTrackingMode();
            int myBearingTrackingMode = getMyBearingTrackingMode();

            // Enable/disable gestures based on tracking mode
            if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                uiSettings.setScrollGesturesEnabled(true);
                uiSettings.setRotateGesturesEnabled(true);
            } else {
                uiSettings.setScrollGesturesEnabled(false);
                uiSettings.setRotateGesturesEnabled((myBearingTrackingMode == MyBearingTracking.NONE));
            }
        }
    }

    public boolean isLocationTrackingDisabled(){
        return mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE;
    }
}
