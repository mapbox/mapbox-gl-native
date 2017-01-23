package com.mapbox.mapboxsdk.maps;

import android.Manifest;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.ContextCompat;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import timber.log.Timber;

/**
 * Settings for the user location and bearing tracking of a MapboxMap.
 */
public final class TrackingSettings {

  private final MyLocationView myLocationView;
  private final UiSettings uiSettings;
  private final FocalPointChangeListener focalPointChangedListener;
  private final CameraZoomInvalidator zoomInvalidator;
  private LocationListener myLocationListener;

  private boolean myLocationEnabled;
  private boolean dismissLocationTrackingOnGesture = true;
  private boolean dismissBearingTrackingOnGesture = true;

  private MapboxMap.OnMyLocationTrackingModeChangeListener onMyLocationTrackingModeChangeListener;
  private MapboxMap.OnMyBearingTrackingModeChangeListener onMyBearingTrackingModeChangeListener;

  TrackingSettings(@NonNull MyLocationView myLocationView, UiSettings uiSettings,
                   FocalPointChangeListener focalPointChangedListener, CameraZoomInvalidator zoomInvalidator) {
    this.myLocationView = myLocationView;
    this.focalPointChangedListener = focalPointChangedListener;
    this.uiSettings = uiSettings;
    this.zoomInvalidator = zoomInvalidator;
  }

  void initialise(MapboxMapOptions options) {
    setMyLocationEnabled(options.getLocationEnabled());
  }

  void onSaveInstanceState(Bundle outState) {
    outState.putInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, getMyLocationTrackingMode());
    outState.putInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, getMyBearingTrackingMode());
    outState.putBoolean(MapboxConstants.STATE_MY_LOCATION_TRACKING_DISMISS, isDismissLocationTrackingOnGesture());
    outState.putBoolean(MapboxConstants.STATE_MY_BEARING_TRACKING_DISMISS, isDismissBearingTrackingOnGesture());
    outState.putBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED, isMyLocationEnabled());
  }

  void onRestoreInstanceState(Bundle savedInstanceState) {
    try {
      setMyLocationEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED));
    } catch (SecurityException ignore) {
      // User did not accept location permissions
    }
    //noinspection ResourceType
    setMyLocationTrackingMode(savedInstanceState.getInt(
      MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, MyLocationTracking.TRACKING_NONE));
    //noinspection ResourceType
    setMyBearingTrackingMode(savedInstanceState.getInt(
      MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, MyBearingTracking.NONE));
    setDismissLocationTrackingOnGesture(savedInstanceState.getBoolean(
      MapboxConstants.STATE_MY_LOCATION_TRACKING_DISMISS, true));
    setDismissBearingTrackingOnGesture(savedInstanceState.getBoolean(
      MapboxConstants.STATE_MY_BEARING_TRACKING_DISMISS, true));
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
    myLocationView.setMyLocationTrackingMode(myLocationTrackingMode);

    if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
      zoomInvalidator.zoomTo(2.0);
      focalPointChangedListener.onFocalPointChanged(myLocationView.getCenter());
    } else {
      focalPointChangedListener.onFocalPointChanged(null);
    }

    if (onMyLocationTrackingModeChangeListener != null) {
      onMyLocationTrackingModeChangeListener.onMyLocationTrackingModeChange(myLocationTrackingMode);
    }
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
    return myLocationView.getMyLocationTrackingMode();
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
    myLocationView.setMyBearingTrackingMode(myBearingTrackingMode);
    if (onMyBearingTrackingModeChangeListener != null) {
      onMyBearingTrackingModeChangeListener.onMyBearingTrackingModeChange(myBearingTrackingMode);
    }
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
    return myLocationView.getMyBearingTrackingMode();
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
    return myLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE;
  }

  /**
   * Returns if bearing tracking disabled
   *
   * @return True if bearing tracking is disabled.
   */
  public boolean isBearingTrackingDisabled() {
    return myLocationView.getMyBearingTrackingMode() == MyBearingTracking.NONE;
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
    return uiSettings.isRotateGesturesEnabled()
      && (dismissBearingTrackingOnGesture
      || myLocationView.getMyBearingTrackingMode() == MyBearingTracking.NONE
      || myLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE);
  }

  /**
   * Returns if scroll gesture are currently enabled.
   *
   * @return True if scroll gestures are currently enabled.
   */
  public boolean isScrollGestureCurrentlyEnabled() {
    return uiSettings.isScrollGesturesEnabled()
      && (dismissLocationTrackingOnGesture
      || myLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE);
  }

  /**
   * Reset the tracking modes as necessary. Location tracking is reset if the map center is changed,
   * bearing tracking if there is a rotation.
   *
   * @param translate true if translation
   * @param rotate    true if rotation
   */
  void resetTrackingModesIfRequired(boolean translate, boolean rotate) {
    // if tracking is on, and we should dismiss tracking with gestures, and this is a scroll action, turn tracking off
    if (translate && !isLocationTrackingDisabled() && isDismissLocationTrackingOnGesture()) {
      setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
    }

    // reset bearing tracking only on rotate
    if (rotate && !isBearingTrackingDisabled() && isDismissBearingTrackingOnGesture()) {
      setMyBearingTrackingMode(MyBearingTracking.NONE);
    }
  }

  void resetTrackingModesIfRequired(CameraPosition cameraPosition) {
    resetTrackingModesIfRequired(cameraPosition.target != null, cameraPosition.bearing != -1);
  }

  Location getMyLocation() {
    return myLocationView.getLocation();
  }

  void setOnMyLocationChangeListener(@Nullable final MapboxMap.OnMyLocationChangeListener listener) {
    if (listener != null) {
      myLocationListener = new LocationListener() {
        @Override
        public void onLocationChanged(Location location) {
          if (listener != null) {
            listener.onMyLocationChange(location);
          }
        }
      };
      LocationServices.getLocationServices(myLocationView.getContext()).addLocationListener(myLocationListener);
    } else {
      LocationServices.getLocationServices(myLocationView.getContext()).removeLocationListener(myLocationListener);
      myLocationListener = null;
    }
  }

  boolean isPermissionsAccepted() {
    return (ContextCompat.checkSelfPermission(myLocationView.getContext(),
      Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED)
      || ContextCompat.checkSelfPermission(myLocationView.getContext(),
      Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
  }

  void setOnMyLocationTrackingModeChangeListener(MapboxMap.OnMyLocationTrackingModeChangeListener listener) {
    this.onMyLocationTrackingModeChangeListener = listener;
  }

  void setOnMyBearingTrackingModeChangeListener(MapboxMap.OnMyBearingTrackingModeChangeListener listener) {
    this.onMyBearingTrackingModeChangeListener = listener;
  }

  MyLocationView getMyLocationView() {
    return myLocationView;
  }


  boolean isMyLocationEnabled() {
    return myLocationEnabled;
  }

  void setMyLocationEnabled(boolean locationEnabled) {
    if (!isPermissionsAccepted()) {
      Timber.e("Could not activate user location tracking: "
        + "user did not accept the permission or permissions were not requested.");
      return;
    }
    myLocationEnabled = locationEnabled;
    myLocationView.setEnabled(locationEnabled);
  }

  void update() {
    if (!myLocationView.isEnabled()) {
      return;
    }
    myLocationView.update();
  }

  void onStart() {
    myLocationView.onStart();
  }

  void onStop() {
    myLocationView.onStop();
  }

  interface CameraZoomInvalidator {
    void zoomTo(double zoomLevel);
  }
}
