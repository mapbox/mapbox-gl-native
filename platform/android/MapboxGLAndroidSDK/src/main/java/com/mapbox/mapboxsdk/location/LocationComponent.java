package com.mapbox.mapboxsdk.location;

import android.annotation.SuppressLint;
import android.content.Context;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresPermission;
import android.support.annotation.StyleRes;
import android.support.annotation.VisibleForTesting;
import android.view.WindowManager;
import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineCallback;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.android.core.location.LocationEngineResult;
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

import java.lang.ref.WeakReference;
import java.util.concurrent.CopyOnWriteArrayList;

import static android.Manifest.permission.ACCESS_COARSE_LOCATION;
import static android.Manifest.permission.ACCESS_FINE_LOCATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_FASTEST_INTERVAL_MILLIS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_INTERVAL_MILLIS;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_TILT_ANIM_DURATION;
import static com.mapbox.mapboxsdk.location.LocationComponentConstants.DEFAULT_TRACKING_ZOOM_ANIM_DURATION;

/**
 * The Location Component provides location awareness to your mobile application. Enabling this
 * component provides a contextual experience to your users by showing an icon representing the users
 * current location. A few different modes are offered to provide the right context to your users at
 * the correct time. {@link RenderMode#NORMAL} simply shows the users location on the map
 * represented as a dot. {@link RenderMode#COMPASS} mode allows you to display an arrow icon
 * (by default) that points in the direction the device is pointing in.
 * {@link RenderMode#GPS} can be used in conjunction with our Navigation SDK to
 * display a larger icon (customized with {@link LocationComponentOptions#gpsDrawable()}) we call the user puck.
 * <p>
 * This component also offers the ability to set a map camera behavior for tracking the user
 * location. These different {@link CameraMode}s will track, stop tracking the location based on the
 * mode set with {@link LocationComponent#setCameraMode(int)}.
 * <p>
 * <strong>
 * To get the component object use {@link MapboxMap#getLocationComponent()} and activate it with
 * {@link #activateLocationComponent(Context)} or one of the overloads.
 * Then, manage its visibility with {@link #setLocationComponentEnabled(boolean)}.
 * </strong>
 * <p>
 * Using this component requires you to request permission beforehand manually or using
 * {@link com.mapbox.android.core.permissions.PermissionsManager}. Either
 * {@code ACCESS_COARSE_LOCATION} or {@code ACCESS_FINE_LOCATION} permissions can be requested for
 * this component to work as expected.
 * <p>
 * This component offers a default, built-in {@link LocationEngine} with some of the activation methods.
 * This engine will be obtained by {@link LocationEngineProvider#getBestLocationEngine(Context, boolean)} which defaults
 * to the {@link com.mapbox.android.core.location.MapboxFusedLocationEngineImpl}. If you'd like to utilize Google
 * Play Services
 * for more precise location updates, simply add the Google Play Location Services dependency in your build script.
 * This will make the default engine the {@link com.mapbox.android.core.location.GoogleLocationEngineImpl} instead.
 * After a custom engine is passed to the component, or the built-in is initialized,
 * the location updates are going to be requested with the {@link LocationEngineRequest}, either a default one,
 * or the one passed during the activation.
 * When using any engine, requesting/removing the location updates is going to be managed internally.
 * <p>
 * You can also push location updates to the component without any internal engine management.
 * To achieve that, use {@link #activateLocationComponent(Context, boolean)} with false.
 * No engine is going to be initialized and you can push location updates with {@link #forceLocationUpdate(Location)}.
 * <p>
 * For location puck animation purposes, like navigation,
 * we recommend limiting the maximum zoom level of the map for the best user experience.
 * <p>
 * Location Component doesn't support state saving out-of-the-box.
 */
public final class LocationComponent {
  private static final String TAG = "Mbgl-LocationComponent";

  @NonNull
  private final MapboxMap mapboxMap;
  private LocationComponentOptions options;
  @NonNull
  private InternalLocationEngineProvider internalLocationEngineProvider = new InternalLocationEngineProvider();
  @Nullable
  private LocationEngine locationEngine;
  @NonNull
  private LocationEngineRequest locationEngineRequest =
    new LocationEngineRequest.Builder(DEFAULT_INTERVAL_MILLIS)
      .setFastestInterval(DEFAULT_FASTEST_INTERVAL_MILLIS)
      .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY)
      .build();
  private LocationEngineCallback<LocationEngineResult> currentLocationEngineListener
    = new CurrentLocationEngineCallback(this);
  private LocationEngineCallback<LocationEngineResult> lastLocationEngineListener
    = new LastLocationEngineCallback(this);

  private CompassEngine compassEngine;

  private LocationLayerController locationLayerController;
  private LocationCameraController locationCameraController;

  private LocationAnimatorCoordinator locationAnimatorCoordinator;

  /**
   * Holds last location which is being returned in the {@link #getLastKnownLocation()}
   * when there is no {@link #locationEngine} set or when the last location returned by the engine is null.
   */
  @Nullable
  private Location lastLocation;
  private CameraPosition lastCameraPosition;

  /**
   * Indicates whether the component has been initialized.
   */
  private boolean isComponentInitialized;

  /**
   * Indicates whether the style has been initialized.
   */
  private boolean isStyleInitialized;

  /**
   * Indicates that the component is enabled and should be displaying location if Mapbox components are available and
   * the lifecycle is in a started state.
   */
  private boolean isEnabled;

  /**
   * Indicated that component's lifecycle {@link #onStart()} method has been called.
   * This allows Mapbox components enter started state and display data, and adds state safety for methods like
   * {@link #setLocationComponentEnabled(boolean)}
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
  private final CopyOnWriteArrayList<OnLocationClickListener> onLocationClickListeners
    = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnLocationLongClickListener> onLocationLongClickListeners
    = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraTrackingChangedListener> onCameraTrackingChangedListeners
    = new CopyOnWriteArrayList<>();

  /**
   * Internal use.
   * <p>
   * To get the component object use {@link MapboxMap#getLocationComponent()}.
   */
  public LocationComponent(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
  }

  @VisibleForTesting
  LocationComponent(@NonNull MapboxMap mapboxMap,
                    @NonNull LocationEngineCallback<LocationEngineResult> currentlistener,
                    @NonNull LocationEngineCallback<LocationEngineResult> lastListener,
                    @NonNull LocationLayerController locationLayerController,
                    @NonNull LocationCameraController locationCameraController,
                    @NonNull LocationAnimatorCoordinator locationAnimatorCoordinator,
                    @NonNull StaleStateManager staleStateManager,
                    @NonNull CompassEngine compassEngine,
                    @NonNull InternalLocationEngineProvider internalLocationEngineProvider) {
    this.mapboxMap = mapboxMap;
    this.currentLocationEngineListener = currentlistener;
    this.lastLocationEngineListener = lastListener;
    this.locationLayerController = locationLayerController;
    this.locationCameraController = locationCameraController;
    this.locationAnimatorCoordinator = locationAnimatorCoordinator;
    this.staleStateManager = staleStateManager;
    this.compassEngine = compassEngine;
    this.internalLocationEngineProvider = internalLocationEngineProvider;
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   * <p>
   * <strong>Note</strong>: This method will initialize and use an internal {@link LocationEngine} when enabled.
   *
   * @param context the context
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context) {
    activateLocationComponent(context,
      LocationComponentOptions.createFromAttributes(context, R.style.mapbox_LocationComponent));
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
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
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   *
   * @param context                  the context
   * @param useDefaultLocationEngine true if you want to initialize and use the built-in location engine or false if
   *                                 there should be no location engine initialized
   * @param locationEngineRequest    the location request
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, boolean useDefaultLocationEngine,
                                        @NonNull LocationEngineRequest locationEngineRequest) {
    setLocationEngineRequest(locationEngineRequest);
    if (useDefaultLocationEngine) {
      activateLocationComponent(context, R.style.mapbox_LocationComponent);
    } else {
      activateLocationComponent(context, null, R.style.mapbox_LocationComponent);
    }
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   * <p>
   * <strong>Note</strong>: This method will initialize and use an internal {@link LocationEngine} when enabled.
   *
   * @param context  the context
   * @param styleRes the LocationComponent style res
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @StyleRes int styleRes) {
    activateLocationComponent(context, LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   * <p>
   * <strong>Note</strong>: This method will initialize and use an internal {@link LocationEngine} when enabled.
   * </p>
   *
   * @param context the context
   * @param options the options
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @NonNull LocationComponentOptions options) {
    initialize(context, options);
    initializeLocationEngine(context);
    applyStyle(options);
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   *
   * @param context        the context
   * @param locationEngine the engine, or null if you'd like to only force location updates
   * @param styleRes       the LocationComponent style res
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @Nullable LocationEngine locationEngine,
                                        @StyleRes int styleRes) {
    activateLocationComponent(context, locationEngine,
      LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   *
   * @param context               the context
   * @param locationEngine        the engine, or null if you'd like to only force location updates
   * @param locationEngineRequest the location request
   * @param styleRes              the LocationComponent style res
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @Nullable LocationEngine locationEngine,
                                        @NonNull LocationEngineRequest locationEngineRequest, @StyleRes int styleRes) {
    activateLocationComponent(context, locationEngine, locationEngineRequest,
      LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   *
   * @param context        the context
   * @param locationEngine the engine
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @NonNull LocationEngine locationEngine) {
    activateLocationComponent(context, locationEngine, R.style.mapbox_LocationComponent);
  }

  /**
   * This method will show the location icon and enable the camera tracking the location.
   *
   * @param context               the context
   * @param locationEngine        the engine
   * @param locationEngineRequest the location request
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @NonNull LocationEngine locationEngine,
                                        @NonNull LocationEngineRequest locationEngineRequest) {
    activateLocationComponent(context, locationEngine, locationEngineRequest, R.style.mapbox_LocationComponent);
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   *
   * @param locationEngine the engine, or null if you'd like to only force location updates
   * @param options        the options
   */
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public void activateLocationComponent(@NonNull Context context, @Nullable LocationEngine locationEngine,
                                        @NonNull LocationComponentOptions options) {
    initialize(context, options);
    setLocationEngine(locationEngine);
    applyStyle(options);
  }

  /**
   * This method initializes the component and needs to be called before any other operations are performed.
   * Afterwards, you can manage component's visibility by {@link #setLocationComponentEnabled(boolean)}.
   *
   * @param context               the context
   * @param locationEngine        the engine, or null if you'd like to only force location updates
   * @param locationEngineRequest the location request
   * @param options               the options
   */
  public void activateLocationComponent(@NonNull Context context, @Nullable LocationEngine locationEngine,
                                        @NonNull LocationEngineRequest locationEngineRequest,
                                        @NonNull LocationComponentOptions options) {
    initialize(context, options);
    setLocationEngineRequest(locationEngineRequest);
    setLocationEngine(locationEngine);
    applyStyle(options);
  }

  /**
   * Manage component's visibility after activation.
   *
   * @param isEnabled true if the plugin should be visible and listen for location updates, false otherwise.
   */
  public void setLocationComponentEnabled(boolean isEnabled) {
    if (isEnabled) {
      enableLocationComponent();
    } else {
      disableLocationComponent();
    }
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
   * Provides the current camera mode being used to track the location or compass updates.
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
   * Apply a new component style with a style resource.
   *
   * @param styleRes a XML style overriding some or all the options
   */
  public void applyStyle(@NonNull Context context, @StyleRes int styleRes) {
    applyStyle(LocationComponentOptions.createFromAttributes(context, styleRes));
  }

  /**
   * Apply a new component style with location component options.
   *
   * @param options to update the current style
   */
  public void applyStyle(@NonNull LocationComponentOptions options) {
    this.options = options;
    locationLayerController.applyStyle(options);
    locationCameraController.initializeOptions(options);
    staleStateManager.setEnabled(options.enableStaleState());
    staleStateManager.setDelayTime(options.staleStateTimeout());
    locationAnimatorCoordinator.setTrackingAnimationDurationMultiplier(options.trackingAnimationDurationMultiplier());
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
   * If {@code null} is passed in, all updates will have to occur through the
   * {@link LocationComponent#forceLocationUpdate(Location)} method.
   *
   * @param locationEngine a {@link LocationEngine} this component should use to handle updates
   */
  @SuppressLint("MissingPermission")
  public void setLocationEngine(@Nullable LocationEngine locationEngine) {
    if (this.locationEngine != null) {
      // If internal location engines being used, extra steps need to be taken to deconstruct the
      // instance.
      this.locationEngine.removeLocationUpdates(currentLocationEngineListener);
      this.locationEngine = null;
    }

    if (locationEngine != null) {
      this.locationEngine = locationEngine;
      if (isLayerReady && isEnabled) {
        setLastLocation();
        locationEngine.requestLocationUpdates(
          locationEngineRequest, currentLocationEngineListener, Looper.getMainLooper());
      }
    }
  }

  /**
   * Set the location request that's going to be used when requesting location updates.
   *
   * @param locationEngineRequest the location request
   */
  public void setLocationEngineRequest(@NonNull LocationEngineRequest locationEngineRequest) {
    this.locationEngineRequest = locationEngineRequest;

    // reset internal LocationEngine ref to re-request location updates if needed
    setLocationEngine(locationEngine);
  }

  /**
   * Get the location request that's going to be used when requesting location updates.
   */
  @NonNull
  public LocationEngineRequest getLocationEngineRequest() {
    return locationEngineRequest;
  }

  /**
   * Returns the current {@link LocationEngine} being used for updating the user location.
   *
   * @return the {@link LocationEngine} being used to update the user location
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
   * Get the last know location of the location component.
   *
   * @return the last known location
   */
  @Nullable
  @RequiresPermission(anyOf = {ACCESS_FINE_LOCATION, ACCESS_COARSE_LOCATION})
  public Location getLastKnownLocation() {
    return lastLocation;
  }

  /**
   * Return the last known {@link CompassEngine} accuracy status of the location component.
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
   * Adds a listener that gets invoked when the user clicks the displayed location.
   * <p>
   * If there are registered location click listeners and the location is clicked,
   * only {@link OnLocationClickListener#onLocationComponentClick()} is going to be delivered,
   * {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnMapClickListener#onMapClick(LatLng)} is going to be consumed
   * and not pushed to the listeners registered after the component's activation.
   *
   * @param listener The location click listener that is invoked when the
   *                 location is clicked
   */
  public void addOnLocationClickListener(@NonNull OnLocationClickListener listener) {
    onLocationClickListeners.add(listener);
  }

  /**
   * Removes the passed listener from the current list of location click listeners.
   *
   * @param listener to be removed
   */
  public void removeOnLocationClickListener(@NonNull OnLocationClickListener listener) {
    onLocationClickListeners.remove(listener);
  }

  /**
   * Adds a listener that gets invoked when the user long clicks the displayed location.
   * <p>
   * If there are registered location long click listeners and the location is long clicked,
   * only {@link OnLocationLongClickListener#onLocationComponentLongClick()} is going to be delivered,
   * {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnMapLongClickListener#onMapLongClick(LatLng)} is going to be consumed
   * and not pushed to the listeners registered after the component's activation.
   *
   * @param listener The location click listener that is invoked when the
   *                 location is clicked
   */
  public void addOnLocationLongClickListener(@NonNull OnLocationLongClickListener listener) {
    onLocationLongClickListeners.add(listener);
  }

  /**
   * Removes the passed listener from the current list of location long click listeners.
   *
   * @param listener to be removed
   */
  public void removeOnLocationLongClickListener(@NonNull OnLocationLongClickListener listener) {
    onLocationLongClickListeners.remove(listener);
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
  public void onDestroy() {
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
    isStyleInitialized = true;
    if (isComponentInitialized) {
      locationLayerController.initializeComponents(options);
      locationCameraController.initializeOptions(options);
      onLocationLayerStart();
    }
  }

  @SuppressLint("MissingPermission")
  private void onLocationLayerStart() {
    if (!isComponentInitialized || !isComponentStarted) {
      return;
    }

    if (!isLayerReady) {
      isLayerReady = true;
      mapboxMap.addOnCameraMoveListener(onCameraMoveListener);
      mapboxMap.addOnCameraIdleListener(onCameraIdleListener);
      if (options.enableStaleState()) {
        staleStateManager.onStart();
      }
      compassEngine.onStart();
    }

    if (isEnabled) {
      if (locationEngine != null) {
        try {
          locationEngine.requestLocationUpdates(
            locationEngineRequest, currentLocationEngineListener, Looper.getMainLooper());
        } catch (SecurityException se) {
          Logger.e(TAG, "Unable to request location updates", se);
        }
      }
      setCameraMode(locationCameraController.getCameraMode());
      setLastLocation();
      setLastCompassHeading();
    }
  }

  private void onLocationLayerStop() {
    if (!isComponentInitialized || !isLayerReady || !isComponentStarted) {
      return;
    }

    isLayerReady = false;
    locationLayerController.hide();
    staleStateManager.onStop();
    compassEngine.onStop();
    locationAnimatorCoordinator.cancelAllAnimations();
    if (locationEngine != null) {
      locationEngine.removeLocationUpdates(currentLocationEngineListener);
    }
    mapboxMap.removeOnCameraMoveListener(onCameraMoveListener);
    mapboxMap.removeOnCameraIdleListener(onCameraIdleListener);
  }

  private void initialize(@NonNull Context context, @NonNull LocationComponentOptions options) {
    if (isComponentInitialized) {
      return;
    }
    isComponentInitialized = true;
    this.options = options;

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
    locationAnimatorCoordinator.setTrackingAnimationDurationMultiplier(options.trackingAnimationDurationMultiplier());

    WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
    SensorManager sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
    compassEngine = new LocationComponentCompassEngine(windowManager, sensorManager);
    compassEngine.addCompassListener(compassListener);
    staleStateManager = new StaleStateManager(onLocationStaleListener, options);

    updateMapWithOptions(options);

    setRenderMode(RenderMode.NORMAL);
    setCameraMode(CameraMode.NONE);

    if (isStyleInitialized) {
      onFinishLoadingStyle();
    }

    onLocationLayerStart();
  }

  private void initializeLocationEngine(@NonNull Context context) {
    if (this.locationEngine != null) {
      this.locationEngine.removeLocationUpdates(currentLocationEngineListener);
    }
    locationEngine = internalLocationEngineProvider.getBestLocationEngine(context, false);
  }

  private void enableLocationComponent() {
    isEnabled = true;
    onLocationLayerStart();
  }

  private void disableLocationComponent() {
    isEnabled = false;
    onLocationLayerStop();
  }

  private void updateMapWithOptions(@NonNull LocationComponentOptions options) {
    int[] padding = options.padding();
    if (padding != null) {
      mapboxMap.setPadding(
        padding[0], padding[1], padding[2], padding[3]
      );
    }
  }

  /**
   * Updates the user location icon.
   *
   * @param location the latest user location
   */
  private void updateLocation(@Nullable final Location location, boolean fromLastLocation) {
    if (location == null) {
      return;
    } else if (!isLayerReady) {
      lastLocation = location;
      return;
    }

    showLocationLayerIfHidden();

    if (!fromLastLocation) {
      staleStateManager.updateLatestLocationTime();
    }
    CameraPosition currentCameraPosition = mapboxMap.getCameraPosition();
    boolean isGpsNorth = getCameraMode() == CameraMode.TRACKING_GPS_NORTH;
    locationAnimatorCoordinator.feedNewLocation(location, currentCameraPosition, isGpsNorth);
    updateAccuracyRadius(location, false);
    lastLocation = location;
  }

  private void showLocationLayerIfHidden() {
    boolean isLocationLayerHidden = locationLayerController.isHidden();
    if (isEnabled && isComponentStarted && isLocationLayerHidden) {
      locationLayerController.show();
    }
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
    if (locationEngine != null) {
      locationEngine.getLastLocation(lastLocationEngineListener);
    } else {
      updateLocation(getLastKnownLocation(), true);
    }
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

  @NonNull
  private OnCameraMoveListener onCameraMoveListener = new OnCameraMoveListener() {
    @Override
    public void onCameraMove() {
      updateLayerOffsets(false);
    }
  };

  @NonNull
  private OnCameraIdleListener onCameraIdleListener = new OnCameraIdleListener() {
    @Override
    public void onCameraIdle() {
      updateLayerOffsets(false);
    }
  };

  @NonNull
  private OnMapClickListener onMapClickListener = new OnMapClickListener() {
    @Override
    public boolean onMapClick(@NonNull LatLng point) {
      if (!onLocationClickListeners.isEmpty() && locationLayerController.onMapClick(point)) {
        for (OnLocationClickListener listener : onLocationClickListeners) {
          listener.onLocationComponentClick();
        }
        return true;
      }
      return false;
    }
  };

  @NonNull
  private MapboxMap.OnMapLongClickListener onMapLongClickListener = new MapboxMap.OnMapLongClickListener() {
    @Override
    public boolean onMapLongClick(@NonNull LatLng point) {
      if (!onLocationLongClickListeners.isEmpty() && locationLayerController.onMapClick(point)) {
        for (OnLocationLongClickListener listener : onLocationLongClickListeners) {
          listener.onLocationComponentLongClick();
        }
        return true;
      }
      return false;
    }
  };

  @NonNull
  private OnLocationStaleListener onLocationStaleListener = new OnLocationStaleListener() {
    @Override
    public void onStaleStateChange(boolean isStale) {
      locationLayerController.setLocationsStale(isStale);

      for (OnLocationStaleListener listener : onLocationStaleListeners) {
        listener.onStaleStateChange(isStale);
      }
    }
  };

  @NonNull
  private OnCameraMoveInvalidateListener onCameraMoveInvalidateListener = new OnCameraMoveInvalidateListener() {
    @Override
    public void onInvalidateCameraMove() {
      onCameraMoveListener.onCameraMove();
    }
  };

  @NonNull
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

  @VisibleForTesting
  static final class CurrentLocationEngineCallback implements LocationEngineCallback<LocationEngineResult> {
    private final WeakReference<LocationComponent> componentWeakReference;

    CurrentLocationEngineCallback(LocationComponent component) {
      this.componentWeakReference = new WeakReference<>(component);
    }

    @Override
    public void onSuccess(LocationEngineResult result) {
      LocationComponent component = componentWeakReference.get();
      if (component != null) {
        component.updateLocation(result.getLastLocation(), false);
      }
    }

    @Override
    public void onFailure(@NonNull Exception exception) {
      Logger.e(TAG, "Failed to obtain location update", exception);
    }
  }

  @VisibleForTesting
  static final class LastLocationEngineCallback implements LocationEngineCallback<LocationEngineResult> {
    private final WeakReference<LocationComponent> componentWeakReference;

    LastLocationEngineCallback(LocationComponent component) {
      this.componentWeakReference = new WeakReference<>(component);
    }

    @Override
    public void onSuccess(LocationEngineResult result) {
      LocationComponent component = componentWeakReference.get();
      if (component != null) {
        component.updateLocation(result.getLastLocation(), true);
      }
    }

    @Override
    public void onFailure(@NonNull Exception exception) {
      Logger.e(TAG, "Failed to obtain last location update", exception);
    }
  }

  @NonNull
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

  static class InternalLocationEngineProvider {
    LocationEngine getBestLocationEngine(@NonNull Context context, boolean background) {
      return LocationEngineProvider.getBestLocationEngine(context, background);
    }
  }
}