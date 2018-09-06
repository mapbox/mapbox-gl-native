package com.mapbox.mapboxsdk.location;

import android.annotation.SuppressLint;
import android.content.Context;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresPermission;
import android.support.annotation.StyleRes;
import android.support.v7.app.AppCompatDelegate;
import android.view.WindowManager;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineListener;
import com.mapbox.android.core.location.LocationEnginePriority;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraIdleListener;
import com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveListener;
import com.mapbox.mapboxsdk.maps.MapboxMap.OnMapClickListener;

import java.util.concurrent.CopyOnWriteArrayList;

import static android.Manifest.permission.ACCESS_COARSE_LOCATION;
import static android.Manifest.permission.ACCESS_FINE_LOCATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_TILT_ANIM_DURATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_ZOOM_ANIM_DURATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.STATE_LOCATION_CAMERA_MODE;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.STATE_LOCATION_ENABLED;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.STATE_LOCATION_LAST_LOCATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.STATE_LOCATION_OPTIONS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.STATE_LOCATION_RENDER_MODE;

/**
 * The Location layer plugin provides location awareness to your mobile application. Enabling this
 * plugin provides a contextual experience to your users by showing an icon representing the users
 * current location. A few different modes are offered to provide the right context to your users at
 * the correct time. {@link RenderMode#NORMAL} simply shows the users location on the map
 * represented as a dot. {@link RenderMode#COMPASS} mode allows you to display an arrow icon
 * (by default) that points in the direction the device is pointing in.
 * {@link RenderMode#GPS} can be used in conjunction with our Navigation SDK to
 * display a larger icon (customized with {@link LocationComponentOptions#gpsDrawable()}) we call the user puck.
 * <p>
 * This plugin also offers the ability to set a map camera behavior for tracking the user
 * location. These different {@link CameraMode}s will track, stop tracking the location based on the
 * mode set with {@link LocationComponent#setCameraMode(int)}.
 * <p>
 * Lastly, {@link LocationComponent#setLocationComponentEnabled(boolean)} can be used
 * to disable the Location Layer but keep the instance around till the activity is destroyed.
 * <p>
 * Using this plugin requires you to request permission beforehand manually or using
 * {@link com.mapbox.android.core.permissions.PermissionsManager}. Either
 * {@code ACCESS_COARSE_LOCATION} or {@code ACCESS_FINE_LOCATION} permissions can be requested and
 * this plugin work as expected.
 * <p>
 * When instantiating the plugin for the first time, the map's max/min zoom levels will be set to
 * {@link LocationComponentOptions#MAX_ZOOM_DEFAULT} and {@link LocationComponentOptions#MIN_ZOOM_DEFAULT} respectively.
 * You can adjust the zoom range with {@link LocationComponentOptions#maxZoom()} and
 * {@link LocationComponentOptions#minZoom()}.
 * <p>
 * When an activity, or a fragment, that contains the plugin is destroyed and recreated,
 * the plugin will restore its state, which is:
 * <br/>
 * - If the plugin was enabled, last location will be displayed.
 * You still need to activate the plugin, or just provide the {@link LocationEngine}.
 * <br/>
 * - {@link CameraMode} and {@link RenderMode} will be restored.
 * <br/>
 * - {@link LocationComponentOptions} will be restored.
 */
public final class LocationComponent {
  private static final String TAG = "Mbgl-LocationComponent";

  private final MapboxMap mapboxMap;
  private LocationComponentOptions options;
  private LocationEngine locationEngine;
  private CompassEngine compassEngine;
  private boolean usingInternalLocationEngine;

  private LocationLayerController locationLayerController;
  private LocationCameraController locationCameraController;

  private LocationAnimatorCoordinator locationAnimatorCoordinator;

  /**
   * Holds last location which is being returned in the {@link #getLastKnownLocation()}
   * when there is no {@link #locationEngine} set or when the last location returned by the engine is null.
   */
  private Location lastLocation;
  private CameraPosition lastCameraPosition;

  /**
   * Indicates that the plugin is enabled and should be displaying location if Mapbox components are available and
   * the lifecycle is in a resumed state.
   */
  private boolean isEnabled;

  /**
   * Indicated that plugin's lifecycle {@link #onStart()} method has been called or the plugin is initialized..
   * This allows Mapbox components enter started state and display data, and adds state safety for methods like
   * {@link #setLocationComponentEnabled(boolean)}
   * <p>
   * Initialized in a started state because the plugin can be instantiated after lifecycle's onStart() and
   * the developer might not register the lifecycle observer but call lifecycle methods manually instead.
   */
  private boolean isComponentStarted;

  /**
   * Indicates if Mapbox components are ready to be interacted with. This can differ from {@link #isComponentStarted}
   * if the Mapbox style is being reloaded.
   */
  private boolean isLayerReady;

  private StaleStateManager staleStateManager;
  private final CopyOnWriteArrayList<OnLocationStaleListener> onLocationStaleListeners
    = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnLocationComponentClickListener> onLocationComponentClickListeners
    = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnLocationComponentLongClickListener> onLocationComponentLongClickListeners
    = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraTrackingChangedListener> onCameraTrackingChangedListeners
    = new CopyOnWriteArrayList<>();

  /**
   * Construct a LocationComponent. In order to display location,
   * the location layer has to be activated with {@link LocationComponent#activateLocationComponent(Context)},
   * or one of the overloads.
   *
   * @param mapboxMap the MapboxMap to apply the LocationComponent with
   */
  public LocationComponent(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    options = LocationComponentOptions.createFromAttributes(context, R.style.mapbox_LocationComponent);
    initialize(context);
  }

  /**
   * This method will show or hide the location icon and enable or disable the camera
   * tracking the location.
   *
   * @param isEnabled true to show layers and enable camera, false otherwise
   */
  private void setLocationComponentEnabled(boolean isEnabled) {
    if (isEnabled) {
      enableLocationComponent();
    } else {
      disableLocationComponent();
    }
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   * <p>
   * <strong>Note</strong>: This method will initialize and use an internal {@link LocationEngine}.
   *
   * @param context the context
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context) {
    activateLocationComponent(context, LocationComponentOptions.createFromAttributes(context, R.style
      .mapbox_LocationComponent));
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   *
   * @param context                  the context
   * @param useDefaultLocationEngine true if you want to initialize and use the built-in location engine or false if
   *                                 there should be no location engine initialized
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, boolean useDefaultLocationEngine) {
    if (useDefaultLocationEngine) {
      activateLocationComponent(context, R.style.mapbox_LocationComponent);
    } else {
      activateLocationComponent(context, null, R.style.mapbox_LocationComponent);
    }
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   * <p>
   * <strong>Note</strong>: This method will initialize and use an internal {@link LocationEngine}.
   *
   * @param context  the context
   * @param styleRes the LocationComponent style res
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @StyleRes int styleRes) {
    activateLocationComponent(context, LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   * <p>
   * <strong>Note</strong>: This method will initialize and use an internal {@link LocationEngine}.
   * </p>
   *
   * @param context the context
   * @param options the options
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @NonNull LocationComponentOptions options) {
    applyStyle(options);
    initializeLocationEngine(context);
    setLocationComponentEnabled(true);
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   *
   * @param context        the context
   * @param locationEngine the engine, or null if you'd like to only force location updates
   * @param styleRes       the LocationComponent style res
   */
  public void activateLocationComponent(@NonNull Context context, @Nullable LocationEngine locationEngine,
                                        @StyleRes int styleRes) {
    activateLocationComponent(locationEngine, LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   *
   * @param context        the context
   * @param locationEngine the engine
   */
  public void activateLocationComponent(@NonNull Context context, @NonNull LocationEngine locationEngine) {
    activateLocationComponent(context, locationEngine, R.style.mapbox_LocationComponent);
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   *
   * @param locationEngine the engine, or null if you'd like to only force location updates
   * @param options        the options
   */
  public void activateLocationComponent(@Nullable LocationEngine locationEngine,
                                        @NonNull LocationComponentOptions options) {
    setLocationEngine(locationEngine);
    applyStyle(options);
    setLocationComponentEnabled(true);
  }

  /**
   * This method will hide the location icon and disable the camera tracking the location.
   */
  public void deactivateLocationComponent() {
    setLocationComponentEnabled(false);
  }

  /**
   * Returns whether the plugin is enabled, meaning that location can be displayed and camera modes can be used.
   *
   * @return true if the plugin is enabled, false otherwise
   */
  public boolean isLocationComponentEnabled() {
    return isEnabled;
  }

  /**
   * Sets the camera mode, which determines how the map camera will track the rendered location.
   * <p>
   * <ul>
   * <li>{@link CameraMode#NONE}: No camera tracking</li>
   * <li>{@link CameraMode#NONE_COMPASS}: Camera does not track location, but does track compass bearing</li>
   * <li>{@link CameraMode#NONE_GPS}: Camera does not track location, but does track GPS bearing</li>
   * <li>{@link CameraMode#TRACKING}: Camera tracks the user location</li>
   * <li>{@link CameraMode#TRACKING_COMPASS}: Camera tracks the user location, with bearing provided by a compass</li>
   * <li>{@link CameraMode#TRACKING_GPS}: Camera tracks the user location, with normalized bearing</li>
   * <li>{@link CameraMode#TRACKING_GPS_NORTH}: Camera tracks the user location, with bearing always set to north</li>
   * </ul>
   *
   * @param cameraMode one of the modes found in {@link CameraMode}
   */
  public void setCameraMode(@CameraMode.Mode int cameraMode) {
    locationCameraController.setCameraMode(cameraMode);
    boolean isGpsNorth = cameraMode == CameraMode.TRACKING_GPS_NORTH;
    locationAnimatorCoordinator.resetAllCameraAnimations(mapboxMap.getCameraPosition(), isGpsNorth);
  }

  /**
   * Provides the current camera mode being used to track
   * the location or compass updates.
   *
   * @return the current camera mode
   */
  @CameraMode.Mode
  public int getCameraMode() {
    return locationCameraController.getCameraMode();
  }

  /**
   * Sets the render mode, which determines how the location updates will be rendered on the map.
   * <p>
   * <ul>
   * <li>{@link RenderMode#NORMAL}: Shows user location, bearing ignored</li>
   * <li>{@link RenderMode#COMPASS}: Shows user location with bearing considered from compass</li>
   * <li>{@link RenderMode#GPS}: Shows user location with bearing considered from location</li>
   * </ul>
   *
   * @param renderMode one of the modes found in {@link RenderMode}
   */
  public void setRenderMode(@RenderMode.Mode int renderMode) {
    locationLayerController.setRenderMode(renderMode);
    updateLayerOffsets(true);
  }

  /**
   * Provides the current render mode being used to show
   * the location and/or compass updates on the map.
   *
   * @return the current render mode
   */
  @RenderMode.Mode
  public int getRenderMode() {
    return locationLayerController.getRenderMode();
  }

  /**
   * Returns the current location options being used.
   *
   * @return the current {@link LocationComponentOptions}
   */
  public LocationComponentOptions getLocationComponentOptions() {
    return options;
  }

  /**
   * Apply a new LocationLayerController style with a style resource.
   *
   * @param styleRes a XML style overriding some or all the options
   */
  public void applyStyle(@NonNull Context context, @StyleRes int styleRes) {
    applyStyle(LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * Apply a new LocationLayerController style with location layer options.
   *
   * @param options to update the current style
   */
  public void applyStyle(LocationComponentOptions options) {
    this.options = options;
    locationLayerController.applyStyle(options);
    locationCameraController.initializeOptions(options);
    staleStateManager.setEnabled(options.enableStaleState());
    staleStateManager.setDelayTime(options.staleStateTimeout());
    updateMapWithOptions(options);
  }

  /**
   * Zooms to the desired zoom level.
   * This API can only be used in pair with camera modes other than {@link CameraMode#NONE}.
   * If you are not using any of {@link CameraMode} modes,
   * use one of {@link MapboxMap#moveCamera(CameraUpdate)},
   * {@link MapboxMap#easeCamera(CameraUpdate)} or {@link MapboxMap#animateCamera(CameraUpdate)} instead.
   *
   * @param zoomLevel         The desired zoom level.
   * @param animationDuration The zoom animation duration.
   * @param callback          The callback with finish/cancel information
   */
  public void zoomWhileTracking(double zoomLevel, long animationDuration,
                                @Nullable MapboxMap.CancelableCallback callback) {
    if (!isLayerReady) {
      return;
    } else if (getCameraMode() == CameraMode.NONE) {
      Logger.e(TAG, String.format("%s%s",
        "LocationComponent#zoomWhileTracking method can only be used",
        " when a camera mode other than CameraMode#NONE is engaged."));
      return;
    }
    locationAnimatorCoordinator.feedNewZoomLevel(zoomLevel, mapboxMap.getCameraPosition(), animationDuration, callback);
  }

  /**
   * Zooms to the desired zoom level.
   * This API can only be used in pair with camera modes other than {@link CameraMode#NONE}.
   * If you are not using any of {@link CameraMode} modes,
   * use one of {@link MapboxMap#moveCamera(CameraUpdate)},
   * {@link MapboxMap#easeCamera(CameraUpdate)} or {@link MapboxMap#animateCamera(CameraUpdate)} instead.
   *
   * @param zoomLevel         The desired zoom level.
   * @param animationDuration The zoom animation duration.
   */
  public void zoomWhileTracking(double zoomLevel, long animationDuration) {
    zoomWhileTracking(zoomLevel, animationDuration, null);
  }

  /**
   * Zooms to the desired zoom level.
   * This API can only be used in pair with camera modes other than {@link CameraMode#NONE}.
   * If you are not using any of {@link CameraMode} modes,
   * use one of {@link MapboxMap#moveCamera(CameraUpdate)},
   * {@link MapboxMap#easeCamera(CameraUpdate)} or {@link MapboxMap#animateCamera(CameraUpdate)} instead.
   *
   * @param zoomLevel The desired zoom level.
   */
  public void zoomWhileTracking(double zoomLevel) {
    zoomWhileTracking(zoomLevel, DEFAULT_TRACKING_ZOOM_ANIM_DURATION, null);
  }

  /**
   * Cancels animation started by {@link #zoomWhileTracking(double, long, MapboxMap.CancelableCallback)}.
   */
  public void cancelZoomWhileTrackingAnimation() {
    locationAnimatorCoordinator.cancelZoomAnimation();
  }

  /**
   * Tilts the camera.
   * This API can only be used in pair with camera modes other than {@link CameraMode#NONE}.
   * If you are not using any of {@link CameraMode} modes,
   * use one of {@link MapboxMap#moveCamera(CameraUpdate)},
   * {@link MapboxMap#easeCamera(CameraUpdate)} or {@link MapboxMap#animateCamera(CameraUpdate)} instead.
   *
   * @param tilt              The desired camera tilt.
   * @param animationDuration The tilt animation duration.
   * @param callback          The callback with finish/cancel information
   */
  public void tiltWhileTracking(double tilt, long animationDuration,
                                @Nullable MapboxMap.CancelableCallback callback) {
    if (!isLayerReady) {
      return;
    } else if (getCameraMode() == CameraMode.NONE) {
      Logger.e(TAG, String.format("%s%s",
        "LocationComponent#tiltWhileTracking method can only be used",
        " when a camera mode other than CameraMode#NONE is engaged."));
      return;
    }
    locationAnimatorCoordinator.feedNewTilt(tilt, mapboxMap.getCameraPosition(), animationDuration, callback);
  }

  /**
   * Tilts the camera.
   * This API can only be used in pair with camera modes other than {@link CameraMode#NONE}.
   * If you are not using any of {@link CameraMode} modes,
   * use one of {@link MapboxMap#moveCamera(CameraUpdate)},
   * {@link MapboxMap#easeCamera(CameraUpdate)} or {@link MapboxMap#animateCamera(CameraUpdate)} instead.
   *
   * @param tilt              The desired camera tilt.
   * @param animationDuration The tilt animation duration.
   */
  public void tiltWhileTracking(double tilt, long animationDuration) {
    tiltWhileTracking(tilt, animationDuration, null);
  }

  /**
   * Tilts the camera.
   * This API can only be used in pair with camera modes other than {@link CameraMode#NONE}.
   * If you are not using any of {@link CameraMode} modes,
   * use one of {@link MapboxMap#moveCamera(CameraUpdate)},
   * {@link MapboxMap#easeCamera(CameraUpdate)} or {@link MapboxMap#animateCamera(CameraUpdate)} instead.
   *
   * @param tilt The desired camera tilt.
   */
  public void tiltWhileTracking(double tilt) {
    tiltWhileTracking(tilt, DEFAULT_TRACKING_TILT_ANIM_DURATION, null);
  }

  /**
   * Cancels animation started by {@link #tiltWhileTracking(double, long, MapboxMap.CancelableCallback)}.
   */
  public void cancelTiltWhileTrackingAnimation() {
    locationAnimatorCoordinator.cancelTiltAnimation();
  }

  /**
   * Use to either force a location update or to manually control when the user location gets
   * updated.
   *
   * @param location where the location icon is placed on the map
   */
  public void forceLocationUpdate(@Nullable Location location) {
    updateLocation(location, false);
  }

  /**
   * Set the location engine to update the current user location.
   * <p>
   * If {@code null} is passed in, all updates will occur through the
   * {@link LocationComponent#forceLocationUpdate(Location)} method.
   *
   * @param locationEngine a {@link LocationEngine} this plugin should use to handle updates
   */
  public void setLocationEngine(@Nullable LocationEngine locationEngine) {
    if (this.locationEngine != null) {
      // If internal location engines being used, extra steps need to be taken to deconstruct the
      // instance.
      if (usingInternalLocationEngine) {
        this.locationEngine.removeLocationUpdates();
        this.locationEngine.deactivate();
        usingInternalLocationEngine = false;
      }
      this.locationEngine.removeLocationEngineListener(locationEngineListener);
      this.locationEngine = null;
    }

    if (locationEngine != null) {
      this.locationEngine = locationEngine;
      if (isEnabled) {
        this.locationEngine.addLocationEngineListener(locationEngineListener);
      }
    }
  }

  /**
   * Returns the current {@link LocationEngine} being used for updating the user location layer.
   *
   * @return the {@link LocationEngine} being used to update the user location layer
   */
  @Nullable
  public LocationEngine getLocationEngine() {
    return locationEngine;
  }

  /**
   * Sets the compass engine used to provide compass heading values.
   *
   * @param compassEngine to be used
   */
  public void setCompassEngine(@NonNull CompassEngine compassEngine) {
    this.compassEngine.removeCompassListener(compassListener);
    this.compassEngine = compassEngine;
    compassEngine.addCompassListener(compassListener);
  }

  /**
   * Returns the compass engine used to provide compass heading values.
   *
   * @return compass engine currently being used
   */
  @NonNull
  public CompassEngine getCompassEngine() {
    return compassEngine;
  }

  /**
   * Get the last know location of the location layer plugin.
   *
   * @return the last known location
   */
  @Nullable
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public Location getLastKnownLocation() {
    Location location = locationEngine != null ? locationEngine.getLastLocation() : null;
    if (location == null) {
      location = lastLocation;
    }
    return location;
  }

  /**
   * Return the last known {@link CompassEngine} accuracy status of the location layer plugin.
   * <p>
   * The last known accuracy of the compass sensor, one of SensorManager.SENSOR_STATUS_*
   *
   * @return the last know compass accuracy bearing
   */
  public float getLastKnownCompassAccuracyStatus() {
    return compassEngine.getLastAccuracySensorStatus();
  }

  /**
   * Add a compass listener to get heading updates every second. Once the first listener gets added,
   * the sensor gets initiated and starts returning values.
   *
   * @param compassListener a {@link CompassListener} for listening into compass heading and
   *                        accuracy changes
   */
  public void addCompassListener(@NonNull CompassListener compassListener) {
    compassEngine.addCompassListener(compassListener);
  }

  /**
   * Remove a compass listener.
   *
   * @param compassListener the {@link CompassListener} which you'd like to remove from the listener
   *                        list.
   */
  public void removeCompassListener(@NonNull CompassListener compassListener) {
    compassEngine.removeCompassListener(compassListener);
  }

  /**
   * Adds a listener that gets invoked when the user clicks the location layer.
   *
   * @param listener The location layer click listener that is invoked when the
   *                 location layer is clicked
   */
  public void addOnLocationClickListener(@NonNull OnLocationComponentClickListener listener) {
    onLocationComponentClickListeners.add(listener);
  }

  /**
   * Removes the passed listener from the current list of location click listeners.
   *
   * @param listener to be removed
   */
  public void removeOnLocationClickListener(@NonNull OnLocationComponentClickListener listener) {
    onLocationComponentClickListeners.remove(listener);
  }

  /**
   * Adds a listener that gets invoked when the user long clicks the location layer.
   *
   * @param listener The location layer click listener that is invoked when the
   *                 location layer is clicked
   */
  public void addOnLocationLongClickListener(@NonNull OnLocationComponentLongClickListener listener) {
    onLocationComponentLongClickListeners.add(listener);
  }

  /**
   * Removes the passed listener from the current list of location long click listeners.
   *
   * @param listener to be removed
   */
  public void removeOnLocationLongClickListener(@NonNull OnLocationComponentLongClickListener listener) {
    onLocationComponentLongClickListeners.remove(listener);
  }

  /**
   * Adds a listener that gets invoked when camera tracking state changes.
   *
   * @param listener Listener that gets invoked when camera tracking state changes.
   */
  public void addOnCameraTrackingChangedListener(@NonNull OnCameraTrackingChangedListener listener) {
    onCameraTrackingChangedListeners.add(listener);
  }

  /**
   * Removes a listener that gets invoked when camera tracking state changes.
   *
   * @param listener Listener that gets invoked when camera tracking state changes.
   */
  public void removeOnCameraTrackingChangedListener(@NonNull OnCameraTrackingChangedListener listener) {
    onCameraTrackingChangedListeners.remove(listener);
  }

  /**
   * Adds the passed listener that gets invoked when user updates have stopped long enough for the last update
   * to be considered stale.
   * <p>
   * This timeout is set by {@link LocationComponentOptions#staleStateTimeout()}.
   *
   * @param listener invoked when last update is considered stale
   */
  public void addOnLocationStaleListener(@NonNull OnLocationStaleListener listener) {
    onLocationStaleListeners.add(listener);
  }

  /**
   * Removes the passed listener from the current list of stale listeners.
   *
   * @param listener to be removed from the list
   */
  public void removeOnLocationStaleListener(@NonNull OnLocationStaleListener listener) {
    onLocationStaleListeners.remove(listener);
  }

  /**
   * Internal use.
   */
  public void onStart() {
    isComponentStarted = true;
    onLocationLayerStart();
  }

  /**
   * Internal use.
   */
  public void onStop() {
    onLocationLayerStop();
    isComponentStarted = false;
  }

  /**
   * Internal use.
   */
  public void onSaveInstanceState(@NonNull Bundle outState) {
    outState.putBoolean(STATE_LOCATION_ENABLED, isEnabled);
    outState.putParcelable(STATE_LOCATION_OPTIONS, options);
    outState.putInt(STATE_LOCATION_RENDER_MODE, locationLayerController.getRenderMode());
    outState.putInt(STATE_LOCATION_CAMERA_MODE, locationCameraController.getCameraMode());
    outState.putParcelable(STATE_LOCATION_LAST_LOCATION, lastLocation);
  }

  /**
   * Internal use.
   */
  public void onRestoreInstanceState(@NonNull Bundle savedInstanceState) {
    updateLocation(savedInstanceState.getParcelable(STATE_LOCATION_LAST_LOCATION), true);
    setCameraMode(savedInstanceState.getInt(STATE_LOCATION_CAMERA_MODE));
    setRenderMode(savedInstanceState.getInt(STATE_LOCATION_RENDER_MODE));
    applyStyle(savedInstanceState.getParcelable(STATE_LOCATION_OPTIONS));
    setLocationComponentEnabled(savedInstanceState.getBoolean(STATE_LOCATION_ENABLED));
  }

  /**
   * Internal use.
   */
  public void onDestroy() {
    if (locationEngine != null && usingInternalLocationEngine) {
      locationEngine.deactivate();
    }
  }

  /**
   * Internal use.
   */
  public void onStartLoadingMap() {
    onLocationLayerStop();
  }

  /**
   * Internal use.
   */
  public void onFinishLoadingStyle() {
    locationLayerController.initializeComponents(options);
    locationCameraController.initializeOptions(options);
    onLocationLayerStart();
  }

  @SuppressLint("MissingPermission")
  private void onLocationLayerStart() {
    if (!isComponentStarted) {
      return;
    }

    if (!isLayerReady) {
      isLayerReady = true;
      if (mapboxMap != null) {
        mapboxMap.addOnCameraMoveListener(onCameraMoveListener);
        mapboxMap.addOnCameraIdleListener(onCameraIdleListener);
      }
      if (options.enableStaleState()) {
        staleStateManager.onStart();
      }
      compassEngine.onStart();
    }

    if (isEnabled) {
      if (locationEngine != null) {
        locationEngine.addLocationEngineListener(locationEngineListener);
        if (locationEngine.isConnected() && usingInternalLocationEngine) {
          locationEngine.requestLocationUpdates();
        }
      }
      setCameraMode(locationCameraController.getCameraMode());
      setLastLocation();
      setLastCompassHeading();
    }
  }

  private void onLocationLayerStop() {
    if (!isLayerReady || !isComponentStarted) {
      return;
    }

    isLayerReady = false;
    locationLayerController.hide();
    staleStateManager.onStop();
    compassEngine.onStop();
    locationAnimatorCoordinator.cancelAllAnimations();
    if (locationEngine != null) {
      if (usingInternalLocationEngine) {
        locationEngine.removeLocationUpdates();
      }
      locationEngine.removeLocationEngineListener(locationEngineListener);
    }
    if (mapboxMap != null) {
      mapboxMap.removeOnCameraMoveListener(onCameraMoveListener);
      mapboxMap.removeOnCameraIdleListener(onCameraIdleListener);
    }
  }

  private void initialize(@NonNull Context context) {
    AppCompatDelegate.setCompatVectorFromResourcesEnabled(true);

    mapboxMap.addOnMapClickListener(onMapClickListener);
    mapboxMap.addOnMapLongClickListener(onMapLongClickListener);

    LayerSourceProvider sourceProvider = new LayerSourceProvider();
    LayerFeatureProvider featureProvider = new LayerFeatureProvider();
    LayerBitmapProvider bitmapProvider = new LayerBitmapProvider(context);
    locationLayerController = new LocationLayerController(mapboxMap, sourceProvider, featureProvider, bitmapProvider,
      options);
    locationCameraController = new LocationCameraController(
      context, mapboxMap, cameraTrackingChangedListener, options, onCameraMoveInvalidateListener);
    locationAnimatorCoordinator = new LocationAnimatorCoordinator();
    locationAnimatorCoordinator.addLayerListener(locationLayerController);
    locationAnimatorCoordinator.addCameraListener(locationCameraController);

    WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
    SensorManager sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
    compassEngine = new LocationComponentCompassEngine(windowManager, sensorManager);
    compassEngine.addCompassListener(compassListener);
    staleStateManager = new StaleStateManager(onLocationStaleListener, options);

    updateMapWithOptions(options);

    setRenderMode(RenderMode.NORMAL);
    setCameraMode(CameraMode.NONE);
  }

  private void initializeLocationEngine(@NonNull Context context) {
    if (this.locationEngine != null) {
      if (usingInternalLocationEngine) {
        this.locationEngine.removeLocationUpdates();
        this.locationEngine.deactivate();
      }
      this.locationEngine.removeLocationEngineListener(locationEngineListener);
    }

    usingInternalLocationEngine = true;
    locationEngine = new LocationEngineProvider(context).obtainBestLocationEngineAvailable();
    locationEngine.setPriority(LocationEnginePriority.HIGH_ACCURACY);
    locationEngine.setFastestInterval(1000);
    locationEngine.addLocationEngineListener(locationEngineListener);
    locationEngine.activate();
  }

  private void enableLocationComponent() {
    isEnabled = true;
    onLocationLayerStart();
  }

  private void disableLocationComponent() {
    isEnabled = false;
    onLocationLayerStop();
  }

  private void updateMapWithOptions(final LocationComponentOptions options) {
    mapboxMap.setPadding(
      options.padding()[0], options.padding()[1], options.padding()[2], options.padding()[3]
    );

    mapboxMap.setMaxZoomPreference(options.maxZoom());
    mapboxMap.setMinZoomPreference(options.minZoom());
  }

  /**
   * Updates the user location icon.
   *
   * @param location the latest user location
   */
  private void updateLocation(final Location location, boolean fromLastLocation) {
    if (location == null) {
      return;
    } else if (!isLayerReady) {
      lastLocation = location;
      return;
    }

    if (isEnabled && isComponentStarted) {
      locationLayerController.show();
    }

    if (!fromLastLocation) {
      staleStateManager.updateLatestLocationTime();
    }
    CameraPosition currentCameraPosition = mapboxMap.getCameraPosition();
    boolean isGpsNorth = getCameraMode() == CameraMode.TRACKING_GPS_NORTH;
    locationAnimatorCoordinator.feedNewLocation(location, currentCameraPosition, isGpsNorth);
    updateAccuracyRadius(location, false);
    lastLocation = location;
  }

  private void updateCompassHeading(float heading) {
    locationAnimatorCoordinator.feedNewCompassBearing(heading, mapboxMap.getCameraPosition());
  }

  /**
   * If the locationEngine contains a last location value, we use it for the initial location layer
   * position.
   */
  @SuppressLint("MissingPermission")
  private void setLastLocation() {
    updateLocation(getLastKnownLocation(), true);
  }

  private void setLastCompassHeading() {
    updateCompassHeading(compassEngine.getLastHeading());
  }

  @SuppressLint("MissingPermission")
  private void updateLayerOffsets(boolean forceUpdate) {
    CameraPosition position = mapboxMap.getCameraPosition();
    if (lastCameraPosition == null || forceUpdate) {
      lastCameraPosition = position;
      locationLayerController.updateForegroundBearing((float) position.bearing);
      locationLayerController.updateForegroundOffset(position.tilt);
      updateAccuracyRadius(getLastKnownLocation(), true);
      return;
    }

    if (position.bearing != lastCameraPosition.bearing) {
      locationLayerController.updateForegroundBearing((float) position.bearing);
    }
    if (position.tilt != lastCameraPosition.tilt) {
      locationLayerController.updateForegroundOffset(position.tilt);
    }
    if (position.zoom != lastCameraPosition.zoom) {
      updateAccuracyRadius(getLastKnownLocation(), true);
    }
    lastCameraPosition = position;
  }

  private void updateAccuracyRadius(Location location, boolean noAnimation) {
    locationAnimatorCoordinator.feedNewAccuracyRadius(Utils.calculateZoomLevelRadius(mapboxMap, location), noAnimation);
  }

  private OnCameraMoveListener onCameraMoveListener = new OnCameraMoveListener() {
    @Override
    public void onCameraMove() {
      updateLayerOffsets(false);
    }
  };

  private OnCameraIdleListener onCameraIdleListener = new OnCameraIdleListener() {
    @Override
    public void onCameraIdle() {
      updateLayerOffsets(false);
    }
  };

  private OnMapClickListener onMapClickListener = new OnMapClickListener() {
    @Override
    public void onMapClick(@NonNull LatLng point) {
      if (!onLocationComponentClickListeners.isEmpty() && locationLayerController.onMapClick(point)) {
        for (OnLocationComponentClickListener listener : onLocationComponentClickListeners) {
          listener.onLocationComponentClick();
        }
      }
    }
  };

  private MapboxMap.OnMapLongClickListener onMapLongClickListener = new MapboxMap.OnMapLongClickListener() {
    @Override
    public void onMapLongClick(@NonNull LatLng point) {
      if (!onLocationComponentLongClickListeners.isEmpty() && locationLayerController.onMapClick(point)) {
        for (OnLocationComponentLongClickListener listener : onLocationComponentLongClickListeners) {
          listener.onLocationComponentLongClick();
        }
      }
    }
  };

  private OnLocationStaleListener onLocationStaleListener = new OnLocationStaleListener() {
    @Override
    public void onStaleStateChange(boolean isStale) {
      locationLayerController.setLocationsStale(isStale);

      for (OnLocationStaleListener listener : onLocationStaleListeners) {
        listener.onStaleStateChange(isStale);
      }
    }
  };

  private OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = new OnCameraMoveInvalidateListener() {
    @Override
    public void onInvalidateCameraMove() {
      onCameraMoveListener.onCameraMove();
    }
  };

  private CompassListener compassListener = new CompassListener() {
    @Override
    public void onCompassChanged(float userHeading) {
      updateCompassHeading(userHeading);
    }

    @Override
    public void onCompassAccuracyChange(int compassStatus) {
      // Currently don't handle this inside SDK
    }
  };

  private LocationEngineListener locationEngineListener = new LocationEngineListener() {
    @Override
    @SuppressWarnings( {"MissingPermission"})
    public void onConnected() {
      if (usingInternalLocationEngine && isLayerReady && isEnabled) {
        locationEngine.requestLocationUpdates();
      }
    }

    @Override
    public void onLocationChanged(Location location) {
      updateLocation(location, false);
    }
  };

  private OnCameraTrackingChangedListener cameraTrackingChangedListener = new OnCameraTrackingChangedListener() {
    @Override
    public void onCameraTrackingDismissed() {
      for (OnCameraTrackingChangedListener listener : onCameraTrackingChangedListeners) {
        listener.onCameraTrackingDismissed();
      }
    }

    @Override
    public void onCameraTrackingChanged(int currentMode) {
      locationAnimatorCoordinator.cancelZoomAnimation();
      locationAnimatorCoordinator.cancelTiltAnimation();
      for (OnCameraTrackingChangedListener listener : onCameraTrackingChangedListeners) {
        listener.onCameraTrackingChanged(currentMode);
      }
    }
  };
}
