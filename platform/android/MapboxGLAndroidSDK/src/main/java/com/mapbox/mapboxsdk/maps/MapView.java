package com.mapbox.mapboxsdk.maps;

import android.app.Activity;
import android.app.Dialog;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.SurfaceTexture;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;

import com.mapbox.mapboxsdk.MapboxAccountManager;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.telemetry.MapboxEvent;
import com.mapbox.mapboxsdk.telemetry.MapboxEventManager;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import timber.log.Timber;

/**
 * <p>
 * A {@code MapView} provides an embeddable map interface.
 * You use this class to display map information and to manipulate the map contents from your application.
 * You can center the map on a given coordinate, specify the size of the area you want to display,
 * and style the features of the map to fit your application's use case.
 * </p>
 * <p>
 * Use of {@code MapView} requires a Mapbox API access token.
 * Obtain an access token on the <a href="https://www.mapbox.com/studio/account/tokens/">Mapbox account page</a>.
 * </p>
 * <strong>Warning:</strong> Please note that you are responsible for getting permission to use the map data,
 * and for ensuring your use adheres to the relevant terms of use.
 */
public class MapView extends FrameLayout {

    private MapboxMap mapboxMap;

    private boolean initialLoad;
    private boolean destroyed;

    private NativeMapView nativeMapView;
    private boolean hasSurface = false;

    private CompassView compassView;
    private MyLocationView myLocationView;
    private LocationListener myLocationListener;

    private MapGestureDetector mapGestureDetector;
    private MapKeyListener mapKeyListener;

    private ConnectivityReceiver connectivityReceiver;

    private List<OnMapReadyCallback> onMapReadyCallbackList = new ArrayList<>();
    private SnapshotRequest snapshotRequest;
    private ZoomButtonsController zoomButtonsController;

    private boolean onStartCalled;
    private boolean onStopCalled;

    @UiThread
    public MapView(@NonNull Context context) {
        super(context);
        initialize(context, MapboxMapOptions.createFromAttributes(context, null));
    }

    @UiThread
    public MapView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initialize(context, MapboxMapOptions.createFromAttributes(context, attrs));
    }

    @UiThread
    public MapView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context, MapboxMapOptions.createFromAttributes(context, attrs));
    }

    @UiThread
    public MapView(@NonNull Context context, @NonNull MapboxMapOptions options) {
        super(context);
        initialize(context, options);
    }

    private void initialize(@NonNull final Context context, @NonNull final MapboxMapOptions options) {
        if (isInEditMode()) {
            // if we are in an editor mode we show an image of a map
            LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_preview, this);
            return;
        }

        // TODO distill into singular purpose methods/classes
        initialLoad = true;
        View view = LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_internal, this);
        setWillNotDraw(false);

        if (options.getTextureMode()) {
            TextureView textureView = new TextureView(context);
            textureView.setSurfaceTextureListener(new SurfaceTextureListener());
            addView(textureView, 0);
        } else {
            SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surfaceView);
            surfaceView.getHolder().addCallback(new SurfaceCallback());
            surfaceView.setVisibility(View.VISIBLE);
        }

        nativeMapView = new NativeMapView(this);

        // inflate overlain Views
        compassView = (CompassView) view.findViewById(R.id.compassView);
        myLocationView = (MyLocationView) view.findViewById(R.id.userLocationView);
        ImageView logoView = (ImageView) view.findViewById(R.id.logoView);
        ImageView attributionsView = (ImageView) view.findViewById(R.id.attributionView);
        attributionsView.setOnClickListener(new AttributionOnClickListener(this));
        ViewGroup markerViewContainer = (ViewGroup) findViewById(R.id.markerViewContainer);

        // interface for focal point invalidation
        FocalPointInvalidator focalPointInvalidator = new FocalPointInvalidator();

        // interface for registering touch listeners
        RegisterTouchListener registerTouchListener = new RegisterTouchListener();

        // setup components for MapboxMap creation
        Projection projection = new Projection(nativeMapView);
        UiSettings uiSettings = new UiSettings(projection, focalPointInvalidator, compassView, attributionsView, logoView);
        TrackingSettings trackingSettings = new TrackingSettings(myLocationView, uiSettings, focalPointInvalidator);
        MyLocationViewSettings myLocationViewSettings = new MyLocationViewSettings(projection, myLocationView, trackingSettings);
        MarkerViewManager markerViewManager = new MarkerViewManager(markerViewContainer);
        AnnotationManager annotationManager = new AnnotationManager(nativeMapView, this, markerViewManager);
        Transform transform = new Transform(nativeMapView, annotationManager.getMarkerViewManager(), trackingSettings);
        mapboxMap = new MapboxMap(nativeMapView, transform, uiSettings, trackingSettings, myLocationViewSettings, projection, registerTouchListener, annotationManager);

        // active user input
        mapGestureDetector = new MapGestureDetector(context, mapboxMap.getTransform(), projection, uiSettings, trackingSettings, annotationManager);
        mapKeyListener = new MapKeyListener(mapboxMap.getTransform(), trackingSettings, uiSettings);

        // attach widgets to MapboxMap
        compassView.setMapboxMap(mapboxMap);
        myLocationView.setMapboxMap(mapboxMap);

        // Ensure this view is interactable
        setClickable(true);
        setLongClickable(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestDisallowInterceptTouchEvent(true);
        requestFocus();

        // Connectivity
        onConnectivityChanged(isConnected());

        // configure the zoom button controller
        zoomButtonsController = new ZoomButtonsController(MapView.this);
        zoomButtonsController.setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
        zoomButtonsController.setOnZoomListener(new OnZoomListener(mapboxMap));

        mapboxMap.initialise(context, options);

        // Shows the zoom controls
        if (!context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
            uiSettings.setZoomControlsEnabled(true);
        }
    }

    //
    // Lifecycle events
    //

    /**
     * <p>
     * You must call this method from the parent's {@link android.app.Activity#onCreate(Bundle)} or
     * {@link android.app.Fragment#onCreate(Bundle)}.
     * </p>
     * You must set a valid access token with {@link MapView#setAccessToken(String)} before you this method
     * or an exception will be thrown.
     *
     * @param savedInstanceState Pass in the parent's savedInstanceState.
     * @see MapView#setAccessToken(String)
     */
    @UiThread
    public void onCreate(@Nullable Bundle savedInstanceState) {
        // TODO distill into singular purpose methods/classes
        String accessToken = mapboxMap.getAccessToken();
        if (TextUtils.isEmpty(accessToken)) {
            accessToken = MapboxAccountManager.getInstance().getAccessToken();
            nativeMapView.setAccessToken(accessToken);
        } else {
            // user provided access token through xml attributes, need to start MapboxAccountManager
            MapboxAccountManager.start(getContext(), accessToken);
            nativeMapView.setAccessToken(accessToken);
        }

        // Force a check for an access token
        MapboxAccountManager.validateAccessToken(accessToken);
        nativeMapView.setAccessToken(accessToken);

        if (savedInstanceState != null && savedInstanceState.getBoolean(MapboxConstants.STATE_HAS_SAVED_STATE)) {

            // Get previous camera position
            CameraPosition cameraPosition = savedInstanceState.getParcelable(MapboxConstants.STATE_CAMERA_POSITION);
            if (cameraPosition != null) {
                mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder(cameraPosition).build()));
            }

            UiSettings uiSettings = mapboxMap.getUiSettings();
            uiSettings.setZoomGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_ENABLED));
            uiSettings.setZoomGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_ENABLED_CHANGE));
            uiSettings.setScrollGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_SCROLL_ENABLED));
            uiSettings.setScrollGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_SCROLL_ENABLED_CHANGE));
            uiSettings.setRotateGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ROTATE_ENABLED));
            uiSettings.setRotateGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_ROTATE_ENABLED_CHANGE));
            uiSettings.setTiltGesturesEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_TILT_ENABLED));
            uiSettings.setTiltGestureChangeAllowed(savedInstanceState.getBoolean(MapboxConstants.STATE_TILT_ENABLED_CHANGE));
            uiSettings.setZoomControlsEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_CONTROLS_ENABLED));

            // Compass
            uiSettings.setCompassEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_COMPASS_ENABLED));
            uiSettings.setCompassGravity(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_GRAVITY));
            uiSettings.setCompassMargins(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT),
                    savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP),
                    savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT),
                    savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM));
            uiSettings.setCompassFadeFacingNorth(savedInstanceState.getBoolean(MapboxConstants.STATE_COMPASS_FADE_WHEN_FACING_NORTH));

            // Logo
            uiSettings.setLogoEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_LOGO_ENABLED));
            uiSettings.setLogoGravity(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_GRAVITY));
            uiSettings.setLogoMargins(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_LEFT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_TOP)
                    , savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_RIGHT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_BOTTOM));

            // Attribution
            uiSettings.setAttributionEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ATTRIBUTION_ENABLED));
            uiSettings.setAttributionGravity(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_GRAVITY));
            uiSettings.setAttributionMargins(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_LEFT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_TOP)
                    , savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_RIGHT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_BOTTOM));

            mapboxMap.setDebugActive(savedInstanceState.getBoolean(MapboxConstants.STATE_DEBUG_ACTIVE));

            String styleUrl = savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL);
            if (!TextUtils.isEmpty(styleUrl)) {
                nativeMapView.setStyleUrl(savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL));
            }

            // User location
            try {
                mapboxMap.setMyLocationEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED));
            } catch (SecurityException ignore) {
                // User did not accept location permissions
            }

            TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
            //noinspection ResourceType
            trackingSettings.setMyLocationTrackingMode(
                    savedInstanceState.getInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, MyLocationTracking.TRACKING_NONE));
            //noinspection ResourceType
            trackingSettings.setMyBearingTrackingMode(
                    savedInstanceState.getInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, MyBearingTracking.NONE));
            trackingSettings.setDismissLocationTrackingOnGesture(
                    savedInstanceState.getBoolean(MapboxConstants.STATE_MY_LOCATION_TRACKING_DISMISS, true));
            trackingSettings.setDismissBearingTrackingOnGesture(
                    savedInstanceState.getBoolean(MapboxConstants.STATE_MY_BEARING_TRACKING_DISMISS, true));
        } else if (savedInstanceState == null) {
            // Start Telemetry (authorization determined in initial MapboxEventManager constructor)
            Timber.i("MapView start Telemetry...");
            MapboxEventManager eventManager = MapboxEventManager.getMapboxEventManager();
            eventManager.initialize(getContext(), getAccessToken());
        }

        // Initialize EGL
        nativeMapView.initializeDisplay();
        nativeMapView.initializeContext();

        // Add annotation deselection listener
        addOnMapChangedListener(new OnMapChangedListener() {
            @Override
            public void onMapChanged(@MapChange int change) {

                // TODO extract logic into separate OnMapReady and Update Component
                if (change == DID_FINISH_LOADING_STYLE && initialLoad) {
                    initialLoad = false;
                    mapboxMap.getAnnotationManager().reloadMarkers();
                    mapboxMap.getAnnotationManager().adjustTopOffsetPixels(mapboxMap);

                    // Notify listeners the map is ready
                    if (onMapReadyCallbackList.size() > 0) {
                        Iterator<OnMapReadyCallback> iterator = onMapReadyCallbackList.iterator();
                        while (iterator.hasNext()) {
                            OnMapReadyCallback callback = iterator.next();
                            callback.onMapReady(mapboxMap);
                            iterator.remove();
                        }
                    }

                    // invalidate camera to update overlain views with correct tilt value
                    mapboxMap.invalidateCameraPosition();

                } else if (change == REGION_IS_CHANGING || change == REGION_DID_CHANGE || change == DID_FINISH_LOADING_MAP) {
                    mapboxMap.getMarkerViewManager().scheduleViewMarkerInvalidation();

                    compassView.update(mapboxMap.getTransform().getBearing());
                    myLocationView.update();
                    mapboxMap.getMarkerViewManager().update();

                    for (InfoWindow infoWindow : mapboxMap.getInfoWindows()) {
                        infoWindow.update();
                    }
                }

            }
        });

        // Fire MapLoad
        if (savedInstanceState == null) {
            Hashtable<String, Object> evt = new Hashtable<>();
            evt.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_MAP_LOAD);
            evt.put(MapboxEvent.ATTRIBUTE_CREATED, MapboxEventManager.generateCreateDate());
            MapboxEventManager.getMapboxEventManager().pushEvent(evt);
        }
    }

    /**
     * You must call this method from the parent's {@link android.app.Activity#onSaveInstanceState(Bundle)}
     * or {@link android.app.Fragment#onSaveInstanceState(Bundle)}.
     *
     * @param outState Pass in the parent's outState.
     */

    @UiThread
    public void onSaveInstanceState(@NonNull Bundle outState) {
        outState.putBoolean(MapboxConstants.STATE_HAS_SAVED_STATE, true);
        outState.putParcelable(MapboxConstants.STATE_CAMERA_POSITION, mapboxMap.getCameraPosition());
        outState.putBoolean(MapboxConstants.STATE_DEBUG_ACTIVE, mapboxMap.isDebugActive());
        outState.putString(MapboxConstants.STATE_STYLE_URL, nativeMapView.getStyleUrl());
        outState.putBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED, mapboxMap.isMyLocationEnabled());

        // TrackingSettings
        TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
        outState.putInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, trackingSettings.getMyLocationTrackingMode());
        outState.putInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, trackingSettings.getMyBearingTrackingMode());
        outState.putBoolean(MapboxConstants.STATE_MY_LOCATION_TRACKING_DISMISS, trackingSettings.isDismissLocationTrackingOnGesture());
        outState.putBoolean(MapboxConstants.STATE_MY_BEARING_TRACKING_DISMISS, trackingSettings.isDismissBearingTrackingOnGesture());

        // UiSettings
        UiSettings uiSettings = mapboxMap.getUiSettings();
        outState.putBoolean(MapboxConstants.STATE_ZOOM_ENABLED, uiSettings.isZoomGesturesEnabled());
        outState.putBoolean(MapboxConstants.STATE_ZOOM_ENABLED_CHANGE, uiSettings.isZoomGestureChangeAllowed());
        outState.putBoolean(MapboxConstants.STATE_SCROLL_ENABLED, uiSettings.isScrollGesturesEnabled());
        outState.putBoolean(MapboxConstants.STATE_SCROLL_ENABLED_CHANGE, uiSettings.isScrollGestureChangeAllowed());
        outState.putBoolean(MapboxConstants.STATE_ROTATE_ENABLED, uiSettings.isRotateGesturesEnabled());
        outState.putBoolean(MapboxConstants.STATE_ROTATE_ENABLED_CHANGE, uiSettings.isRotateGestureChangeAllowed());
        outState.putBoolean(MapboxConstants.STATE_TILT_ENABLED, uiSettings.isTiltGesturesEnabled());
        outState.putBoolean(MapboxConstants.STATE_TILT_ENABLED_CHANGE, uiSettings.isTiltGestureChangeAllowed());
        outState.putBoolean(MapboxConstants.STATE_ZOOM_CONTROLS_ENABLED, uiSettings.isZoomControlsEnabled());

        // UiSettings - Compass
        outState.putBoolean(MapboxConstants.STATE_COMPASS_ENABLED, uiSettings.isCompassEnabled());
        outState.putInt(MapboxConstants.STATE_COMPASS_GRAVITY, uiSettings.getCompassGravity());
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT, uiSettings.getCompassMarginLeft());
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP, uiSettings.getCompassMarginTop());
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM, uiSettings.getCompassMarginBottom());
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT, uiSettings.getCompassMarginRight());
        outState.putBoolean(MapboxConstants.STATE_COMPASS_FADE_WHEN_FACING_NORTH, uiSettings.isCompassFadeWhenFacingNorth());

        // UiSettings - Logo
        outState.putInt(MapboxConstants.STATE_LOGO_GRAVITY, uiSettings.getLogoGravity());
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_LEFT, uiSettings.getLogoMarginLeft());
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_TOP, uiSettings.getLogoMarginTop());
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_RIGHT, uiSettings.getLogoMarginRight());
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_BOTTOM, uiSettings.getLogoMarginBottom());
        outState.putBoolean(MapboxConstants.STATE_LOGO_ENABLED, uiSettings.isLogoEnabled());

        // UiSettings - Attribution
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_GRAVITY, uiSettings.getAttributionGravity());
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_LEFT, uiSettings.getAttributionMarginLeft());
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_TOP, uiSettings.getAttributionMarginTop());
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_RIGHT, uiSettings.getAttributionMarginRight());
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_BOTTOM, uiSettings.getAttributionMarginBottom());
        outState.putBoolean(MapboxConstants.STATE_ATTRIBUTION_ENABLED, uiSettings.isAttributionEnabled());
    }

    /**
     * You must call this method from the parent's {@link Activity#onStart()} or {@link Fragment#onStart()}
     */
    @UiThread
    public void onStart() {
        onStartCalled = true;

        // Register for connectivity changes
        connectivityReceiver = new ConnectivityReceiver();
        getContext().registerReceiver(connectivityReceiver, new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));

        nativeMapView.update();
        myLocationView.onStart();

        if (TextUtils.isEmpty(nativeMapView.getStyleUrl())) {
            // if user hasn't loaded a Style yet, load default for them instead
            nativeMapView.setStyleUrl(Style.MAPBOX_STREETS);
        }
    }

    /**
     * You must call this method from the parent's {@link Activity#onResume()} or {@link Fragment#onResume()}.
     */
    @UiThread
    public void onResume() {
        if (!onStartCalled) {
            // TODO: 26/10/16, can be removed after 5.0.0 release
            throw new IllegalStateException("MapView#onStart() was not called. " +
                    "You must call this method from the parent's {@link Activity#onStart()} or {@link Fragment#onStart()}.");
        }
    }

    /**
     * You must call this method from the parent's {@link Activity#onPause()} or {@link Fragment#onPause()}.
     */
    @UiThread
    public void onPause() {
        // replaced by onStop in v5.0.0, keep around for future development
    }

    /**
     * You must call this method from the parent's {@link Activity#onStop()} or {@link Fragment#onStop()}.
     */
    @UiThread
    public void onStop() {
        onStopCalled = true;

        // Unregister for connectivity changes
        if (connectivityReceiver != null) {
            getContext().unregisterReceiver(connectivityReceiver);
            connectivityReceiver = null;
        }

        myLocationView.onStop();
    }

    /**
     * You must call this method from the parent's {@link Activity#onDestroy()} or {@link Fragment#onDestroy()}.
     */
    @UiThread
    public void onDestroy() {
        if (!onStopCalled) {
            // TODO: 26/10/16, can be removed after 5.0.0 release
            throw new IllegalStateException("MapView#onStop() was not called. " +
                    "You must call this method from the parent's {@link Activity#onStop()} or {@link Fragment#onStop()}.");
        }

        destroyed = true;
        nativeMapView.terminateContext();
        nativeMapView.terminateDisplay();
        nativeMapView.destroySurface();
        nativeMapView.destroy();
        nativeMapView = null;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            if (mapboxMap.getUiSettings().isZoomControlsEnabled()) {
                zoomButtonsController.setVisible(true);
            }
        }
        return mapGestureDetector.onTouchEvent(event) || super.onTouchEvent(event);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return mapKeyListener.onKeyDown(keyCode, event) || super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event) {
        return mapKeyListener.onKeyLongPress(keyCode, event) || super.onKeyLongPress(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        return mapKeyListener.onKeyUp(keyCode, event) || super.onKeyUp(keyCode, event);
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event) {
        return mapKeyListener.onTrackballEvent(event) || super.onTrackballEvent(event);
    }

    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        return mapGestureDetector.onGenericMotionEvent(event) || super.onGenericMotionEvent(event);
    }

    @Override
    public boolean onHoverEvent(MotionEvent event) {
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_HOVER_ENTER:
            case MotionEvent.ACTION_HOVER_MOVE:
                // Show the zoom controls
                if (mapboxMap.getUiSettings().isZoomControlsEnabled()) {
                    zoomButtonsController.setVisible(true);
                }
                return true;

            case MotionEvent.ACTION_HOVER_EXIT:
                // Hide the zoom controls
                if (mapboxMap.getUiSettings().isZoomControlsEnabled()) {
                    zoomButtonsController.setVisible(false);
                }
                return true;

            default:
                // We are not interested in this event
                return false;
        }
    }

    /**
     * You must call this method from the parent's {@link Activity#onLowMemory()} or {@link Fragment#onLowMemory()}.
     */
    @UiThread
    public void onLowMemory() {
        nativeMapView.onLowMemory();
    }

    // Called when debug mode is enabled to update a FPS counter
    // Called via JNI from NativeMapView
    // Forward to any listener
    protected void onFpsChanged(final double fps) {
        post(new Runnable() {
            @Override
            public void run() {
                MapboxMap.OnFpsChangedListener listener = mapboxMap.getOnFpsChangedListener();
                if (listener != null) {
                    listener.onFpsChanged(fps);
                }
            }
        });
    }

    /**
     * <p>
     * Loads a new map style from the specified URL.
     * </p>
     * {@code url} can take the following forms:
     * <ul>
     * <li>{@code Style.*}: load one of the bundled styles in {@link Style}.</li>
     * <li>{@code mapbox://styles/<user>/<style>}:
     * retrieves the style from a <a href="https://www.mapbox.com/account/">Mapbox account.</a>
     * {@code user} is your username. {@code style} is the ID of your custom
     * style created in <a href="https://www.mapbox.com/studio">Mapbox Studio</a>.</li>
     * <li>{@code http://...} or {@code https://...}:
     * retrieves the style over the Internet from any web server.</li>
     * <li>{@code asset://...}:
     * reads the style from the APK {@code assets/} directory.
     * This is used to load a style bundled with your app.</li>
     * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
     * </ul>
     * <p>
     * This method is asynchronous and will return immediately before the style finishes loading.
     * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
     * </p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
     *
     * @param url The URL of the map style
     * @see Style
     */
    public void setStyleUrl(@NonNull String url) {
        if (destroyed) {
            return;
        }

        // stopgap for https://github.com/mapbox/mapbox-gl-native/issues/6242
        if (TextUtils.isEmpty(nativeMapView.getAccessToken())) {
            setAccessToken(MapboxAccountManager.getInstance().getAccessToken());
        }

        nativeMapView.setStyleUrl(url);
    }

    /**
     * <p>
     * DEPRECATED @see MapboxAccountManager#start(String)
     * </p>
     * <p>
     * Sets the current Mapbox access token used to load map styles and tiles.
     * </p>
     * <p>
     * You must set a valid access token before you call {@link MapView#onCreate(Bundle)}
     * or an exception will be thrown.
     * </p>
     *
     * @param accessToken Your public Mapbox access token.
     * @see MapView#onCreate(Bundle)
     * @deprecated As of release 4.1.0, replaced by {@link com.mapbox.mapboxsdk.MapboxAccountManager#start(Context, String)}
     */
    @Deprecated
    @UiThread
    public void setAccessToken(@NonNull String accessToken) {
        if (destroyed) {
            return;
        }
        // validateAccessToken does the null check
        if (!TextUtils.isEmpty(accessToken)) {
            accessToken = accessToken.trim();
        }
        MapboxAccountManager.validateAccessToken(accessToken);
        nativeMapView.setAccessToken(accessToken);
    }

    /**
     * <p>
     * DEPRECATED @see MapboxAccountManager#getAccessToken()
     * </p>
     * <p>
     * Returns the current Mapbox access token used to load map styles and tiles.
     * </p>
     *
     * @return The current Mapbox access token.
     * @deprecated As of release 4.1.0, replaced by {@link MapboxAccountManager#getAccessToken()}
     */
    @Deprecated
    @UiThread
    @Nullable
    public String getAccessToken() {
        if (destroyed) {
            return "";
        }
        return nativeMapView.getAccessToken();
    }

    //
    // Rendering
    //

    // Called when the map needs to be rerendered
    // Called via JNI from NativeMapView
    protected void onInvalidate() {
        postInvalidate();
    }

    @Override
    public void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (isInEditMode()) {
            return;
        }

        if (destroyed) {
            return;
        }

        if (!hasSurface) {
            return;
        }

        nativeMapView.render();
    }

    @Override
    protected void onSizeChanged(int width, int height, int oldw, int oldh) {
        if (destroyed) {
            return;
        }

        if (!isInEditMode()) {
            nativeMapView.resizeView(width, height);
        }
    }

    private class SurfaceCallback implements SurfaceHolder.Callback {

        private Surface surface;

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            nativeMapView.createSurface(surface = holder.getSurface());
            hasSurface = true;
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            if (destroyed) {
                return;
            }
            nativeMapView.resizeFramebuffer(width, height);
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            hasSurface = false;

            if (nativeMapView != null) {
                nativeMapView.destroySurface();
            }
            surface.release();
        }
    }

    // This class handles TextureView callbacks
    private class SurfaceTextureListener implements TextureView.SurfaceTextureListener {

        private Surface surface;

        // Called when the native surface texture has been created
        // Must do all EGL/GL ES initialization here
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            nativeMapView.createSurface(this.surface = new Surface(surface));
            nativeMapView.resizeFramebuffer(width, height);
            hasSurface = true;
        }

        // Called when the native surface texture has been destroyed
        // Must do all EGL/GL ES destruction here
        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            hasSurface = false;

            if (nativeMapView != null) {
                nativeMapView.destroySurface();
            }
            this.surface.release();
            return true;
        }

        // Called when the format or size of the native surface texture has been changed
        // Must handle window resizing here.
        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            if (destroyed) {
                return;
            }

            nativeMapView.resizeFramebuffer(width, height);
        }

        // Called when the SurfaceTexure frame is drawn to screen
        // Must sync with UI here
        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {
            if (destroyed) {
                return;
            }
            // TODO move to transform. java
            compassView.update(mapboxMap.getTransform().getBearing());
            myLocationView.update();
            mapboxMap.getMarkerViewManager().update();

            for (InfoWindow infoWindow : mapboxMap.getInfoWindows()) {
                infoWindow.update();
            }
        }
    }

    //
    // View events
    //

    // Called when view is no longer connected
    @Override
    @CallSuper
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        // Required by ZoomButtonController (from Android SDK documentation)
        if (mapboxMap.getUiSettings().isZoomControlsEnabled()) {
            zoomButtonsController.setVisible(false);
        }

        // make sure we don't leak location listener
        if (myLocationListener != null) {
            // cleanup to prevent memory leak
            LocationServices services = LocationServices.getLocationServices(getContext());
            services.removeLocationListener(myLocationListener);
            myLocationListener = null;
        }
    }

    // Called when view is hidden and shown
    @Override
    protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
        if (isInEditMode()) {
            return;
        }

        if (mapboxMap != null && mapboxMap.getUiSettings().isZoomControlsEnabled()) {
            zoomButtonsController.setVisible(visibility == View.VISIBLE);
        }
    }

    //
    // Connectivity events
    //

    // This class handles connectivity changes
    private class ConnectivityReceiver extends BroadcastReceiver {

        // Called when an action we are listening to in the manifest has been sent
        @Override
        public void onReceive(Context context, Intent intent) {
            if (!destroyed && intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
                boolean noConnectivity = intent.getBooleanExtra(ConnectivityManager.EXTRA_NO_CONNECTIVITY, false);
                onConnectivityChanged(!noConnectivity);
            }
        }
    }

    // Called when MapView is being created
    private boolean isConnected() {
        Context appContext = getContext().getApplicationContext();
        ConnectivityManager connectivityManager = (ConnectivityManager) appContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
        return (activeNetwork != null) && activeNetwork.isConnectedOrConnecting();
    }

    // Called when our Internet connectivity has changed
    private void onConnectivityChanged(boolean isConnected) {
        nativeMapView.setReachability(isConnected);
    }

    //
    // Map events
    //

    /**
     * <p>
     * Add a callback that's invoked when the displayed map view changes.
     * </p>
     * To remove the callback, use {@link MapView#removeOnMapChangedListener(OnMapChangedListener)}.
     *
     * @param listener The callback that's invoked on every frame rendered to the map view.
     * @see MapView#removeOnMapChangedListener(OnMapChangedListener)
     */
    public void addOnMapChangedListener(@Nullable OnMapChangedListener listener) {
        if (listener != null) {
            nativeMapView.addOnMapChangedListener(listener);
        }
    }

    /**
     * Remove a callback added with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}
     *
     * @param listener The previously added callback to remove.
     * @see MapView#addOnMapChangedListener(OnMapChangedListener)
     */
    public void removeOnMapChangedListener(@Nullable OnMapChangedListener listener) {
        if (listener != null) {
            nativeMapView.removeOnMapChangedListener(listener);
        }
    }

    // Called when the map view transformation has changed
    // Called via JNI from NativeMapView
    // Forward to any listeners
    protected void onMapChanged(int mapChange) {
        nativeMapView.onMapChangedEventDispatch(mapChange);
    }


    /**
     * Sets a callback object which will be triggered when the {@link MapboxMap} instance is ready to be used.
     *
     * @param callback The callback object that will be triggered when the map is ready to be used.
     */
    @UiThread
    public void getMapAsync(final OnMapReadyCallback callback) {
        if (!initialLoad && callback != null) {
            callback.onMapReady(mapboxMap);
        } else {
            if (callback != null) {
                onMapReadyCallbackList.add(callback);
            }
        }
    }

    MapboxMap getMapboxMap() {
        return mapboxMap;
    }

    void setMapboxMap(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
    }

    //
    // Snapshot API
    //

    @UiThread
    void snapshot(@NonNull final MapboxMap.SnapshotReadyCallback callback, @Nullable final Bitmap bitmap) {
        snapshotRequest = new SnapshotRequest(bitmap, callback);
        nativeMapView.scheduleTakeSnapshot();
        nativeMapView.render();
    }

    // Called when the snapshot method was executed
    // Called via JNI from NativeMapView
    // Forward to any listeners
    protected void onSnapshotReady(byte[] bytes) {
        if (snapshotRequest != null && bytes != null) {
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inBitmap = snapshotRequest.getBitmap();  // the old Bitmap to be reused
            options.inMutable = true;
            options.inSampleSize = 1;
            Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.length, options);

            MapboxMap.SnapshotReadyCallback callback = snapshotRequest.getCallback();
            if (callback != null) {
                callback.onSnapshotReady(bitmap);
            }
        }
    }

    private class SnapshotRequest {
        private Bitmap bitmap;
        private MapboxMap.SnapshotReadyCallback callback;

        SnapshotRequest(Bitmap bitmap, MapboxMap.SnapshotReadyCallback callback) {
            this.bitmap = bitmap;
            this.callback = callback;
        }

        public Bitmap getBitmap() {
            return bitmap;
        }

        public MapboxMap.SnapshotReadyCallback getCallback() {
            return callback;
        }
    }

    private static class AttributionOnClickListener implements View.OnClickListener, DialogInterface.OnClickListener {

        private static final int ATTRIBUTION_INDEX_IMPROVE_THIS_MAP = 2;
        private static final int ATTRIBUTION_INDEX_TELEMETRY_SETTINGS = 3;
        private MapView mapView;

        AttributionOnClickListener(MapView mapView) {
            super();
            this.mapView = mapView;
        }

        // Called when someone presses the attribution icon
        @Override
        public void onClick(View view) {
            AlertDialog.Builder builder = new AlertDialog.Builder(mapView.getContext(), R.style.mapbox_AlertDialogStyle);
            builder.setTitle(R.string.mapbox_attributionsDialogTitle);
            String[] items = mapView.getContext().getResources().getStringArray(R.array.mapbox_attribution_names);
            builder.setAdapter(new ArrayAdapter<>(mapView.getContext(), R.layout.mapbox_attribution_list_item, items), this);
            builder.show();
        }

        // Called when someone selects an attribution, 'Improve this map' adds location data to the url
        @Override
        public void onClick(DialogInterface dialog, int which) {
            final Context context = ((Dialog) dialog).getContext();
            if (which == ATTRIBUTION_INDEX_TELEMETRY_SETTINGS) {
                AlertDialog.Builder builder = new AlertDialog.Builder(context, R.style.mapbox_AlertDialogStyle);
                builder.setTitle(R.string.mapbox_attributionTelemetryTitle);
                builder.setMessage(R.string.mapbox_attributionTelemetryMessage);
                builder.setPositiveButton(R.string.mapbox_attributionTelemetryPositive, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        MapboxEventManager.getMapboxEventManager().setTelemetryEnabled(true);
                        dialog.cancel();
                    }
                });
                builder.setNeutralButton(R.string.mapbox_attributionTelemetryNeutral, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        String url = context.getResources().getStringArray(R.array.mapbox_attribution_links)[3];
                        Intent intent = new Intent(Intent.ACTION_VIEW);
                        intent.setData(Uri.parse(url));
                        context.startActivity(intent);
                        dialog.cancel();
                    }
                });
                builder.setNegativeButton(R.string.mapbox_attributionTelemetryNegative, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        MapboxEventManager.getMapboxEventManager().setTelemetryEnabled(false);
                        dialog.cancel();
                    }
                });

                builder.show();
                return;
            }
            String url = context.getResources().getStringArray(R.array.mapbox_attribution_links)[which];
            if (which == ATTRIBUTION_INDEX_IMPROVE_THIS_MAP) {
                CameraPosition cameraPosition = mapView.getMapboxMap().getCameraPosition();
                if (cameraPosition != null) {
                    url = String.format(url, cameraPosition.target.getLongitude(),
                            cameraPosition.target.getLatitude(), (int) cameraPosition.zoom);
                }
            }
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(url));
            context.startActivity(intent);
        }
    }

    /**
     * Definition of a map change event.
     *
     * @see MapView.OnMapChangedListener#onMapChanged(int)
     */
    @IntDef({REGION_WILL_CHANGE,
            REGION_WILL_CHANGE_ANIMATED,
            REGION_IS_CHANGING,
            REGION_DID_CHANGE,
            REGION_DID_CHANGE_ANIMATED,
            WILL_START_LOADING_MAP,
            DID_FINISH_LOADING_MAP,
            DID_FAIL_LOADING_MAP,
            WILL_START_RENDERING_FRAME,
            DID_FINISH_RENDERING_FRAME,
            DID_FINISH_RENDERING_FRAME_FULLY_RENDERED,
            WILL_START_RENDERING_MAP,
            DID_FINISH_RENDERING_MAP,
            DID_FINISH_RENDERING_MAP_FULLY_RENDERED,
            DID_FINISH_LOADING_STYLE,
            SOURCE_DID_CHANGE
    })
    @Retention(RetentionPolicy.SOURCE)
    public @interface MapChange {
    }

    /**
     * This event is triggered whenever the currently displayed map region is about to changing
     * without an animation.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_WILL_CHANGE = 0;

    /**
     * This event is triggered whenever the currently displayed map region is about to changing
     * with an animation.
     * <p
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_WILL_CHANGE_ANIMATED = 1;

    /**
     * This event is triggered whenever the currently displayed map region is changing.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_IS_CHANGING = 2;

    /**
     * This event is triggered whenever the currently displayed map region finished changing
     * without an animation.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_DID_CHANGE = 3;

    /**
     * This event is triggered whenever the currently displayed map region finished changing
     * with an animation.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_DID_CHANGE_ANIMATED = 4;

    /**
     * This event is triggered when the map is about to start loading a new map style.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int WILL_START_LOADING_MAP = 5;

    /**
     * This  is triggered when the map has successfully loaded a new map style.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_LOADING_MAP = 6;

    /**
     * This event is triggered when the map has failed to load a new map style.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FAIL_LOADING_MAP = 7;

    /**
     * This event is triggered when the map will start rendering a frame.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int WILL_START_RENDERING_FRAME = 8;

    /**
     * This event is triggered when the map finished rendering a frame.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_FRAME = 9;

    /**
     * This event is triggered when the map finished rendeirng the frame fully.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_FRAME_FULLY_RENDERED = 10;

    /**
     * This event is triggered when the map will start rendering the map.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int WILL_START_RENDERING_MAP = 11;

    /**
     * This event is triggered when the map finished rendering the map.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_MAP = 12;

    /**
     * This event is triggered when the map is fully rendered.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_MAP_FULLY_RENDERED = 13;


    /**
     * This {@link MapChange} is triggered when a style has finished loading.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_LOADING_STYLE = 14;


    /**
     * This {@link MapChange} is triggered when a source attribution changes.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapChange
     * @see MapView.OnMapChangedListener
     */
    public static final int SOURCE_DID_CHANGE = 15;

    /**
     * Interface definition for a callback to be invoked when the displayed map view changes.
     * <p>
     * Register to {@link MapChange} events with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}.
     * </p>
     *
     * @see MapView#addOnMapChangedListener(OnMapChangedListener)
     * @see MapView.MapChange
     */
    public interface OnMapChangedListener {
        /**
         * Called when the displayed map view changes.
         *
         * @param change Type of map change event, one of {@link #REGION_WILL_CHANGE},
         *               {@link #REGION_WILL_CHANGE_ANIMATED},
         *               {@link #REGION_IS_CHANGING},
         *               {@link #REGION_DID_CHANGE},
         *               {@link #REGION_DID_CHANGE_ANIMATED},
         *               {@link #WILL_START_LOADING_MAP},
         *               {@link #DID_FAIL_LOADING_MAP},
         *               {@link #DID_FINISH_LOADING_MAP},
         *               {@link #WILL_START_RENDERING_FRAME},
         *               {@link #DID_FINISH_RENDERING_FRAME},
         *               {@link #DID_FINISH_RENDERING_FRAME_FULLY_RENDERED},
         *               {@link #WILL_START_RENDERING_MAP},
         *               {@link #DID_FINISH_RENDERING_MAP},
         *               {@link #DID_FINISH_RENDERING_MAP_FULLY_RENDERED}.
         */
        void onMapChanged(@MapChange int change);
    }

    // This class handles input events from the zoom control buttons
    // Zoom controls allow single touch only devices to zoom in and out
    private class OnZoomListener implements ZoomButtonsController.OnZoomListener {

        private UiSettings uiSettings;
        private Transform transform;

        OnZoomListener(MapboxMap mapboxMap) {
            this.uiSettings = mapboxMap.getUiSettings();
            this.transform = mapboxMap.getTransform();
        }

        // Not used
        @Override
        public void onVisibilityChanged(boolean visible) {
            // Ignore
        }

        // Called when user pushes a zoom button
        @Override
        public void onZoom(boolean zoomIn) {
            if (!uiSettings.isZoomGesturesEnabled()) {
                return;
            }
            transform.zoom(zoomIn);
        }
    }

    private class FocalPointInvalidator implements FocalPointChangeListener {

        @Override
        public void onFocalPointChanged(PointF pointF) {
            mapGestureDetector.setFocalPoint(pointF);
        }
    }

    private class RegisterTouchListener implements MapboxMap.OnRegisterTouchListener {

        @Override
        public void onRegisterMapClickListener(MapboxMap.OnMapClickListener listener) {
            mapGestureDetector.setOnMapClickListener(listener);
        }

        @Override
        public void onRegisterMapLongClickListener(MapboxMap.OnMapLongClickListener listener) {
            mapGestureDetector.setOnMapLongClickListener(listener);
        }

        @Override
        public void onRegisterScrollListener(MapboxMap.OnScrollListener listener) {
            mapGestureDetector.setOnScrollListener(listener);
        }

        @Override
        public void onRegisterFlingListener(MapboxMap.OnFlingListener listener) {
            mapGestureDetector.setOnFlingListener(listener);
        }
    }

    NativeMapView getNativeMapView() {
        return nativeMapView;
    }
}
