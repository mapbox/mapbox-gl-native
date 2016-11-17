package com.mapbox.mapboxsdk.maps;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Dialog;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.FloatRange;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.ShoveGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.mapboxsdk.MapboxAccountManager;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;
import com.mapbox.mapboxsdk.telemetry.MapboxEvent;
import com.mapbox.mapboxsdk.telemetry.MapboxEventManager;
import com.mapbox.mapboxsdk.utils.ColorUtils;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import static com.mapbox.mapboxsdk.utils.MathUtils.convertNativeBearing;

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
    private IconManager iconManager;
    private AnnotationManager annotationManager;

    private boolean initialLoad;
    private boolean destroyed;

    private NativeMapView nativeMapView;
    private boolean hasSurface = false;

    private ViewGroup markerViewContainer;
    private CompassView compassView;
    private ImageView logoView;
    private ImageView attributionsView;
    private MyLocationView myLocationView;
    private LocationListener myLocationListener;

    private Projection projection;

    private CopyOnWriteArrayList<OnMapChangedListener> onMapChangedListener;
    private ZoomButtonsController zoomButtonsController;
    private ConnectivityReceiver connectivityReceiver;
    private float screenDensity = 1.0f;

    private TrackballLongPressTimeOut currentTrackballLongPressTimeOut;
    private GestureDetectorCompat gestureDetector;
    private ScaleGestureDetector scaleGestureDetector;
    private RotateGestureDetector rotateGestureDetector;
    private ShoveGestureDetector shoveGestureDetector;
    private boolean twoTap = false;
    private boolean zoomStarted = false;
    private boolean dragStarted = false;
    private boolean quickZoom = false;
    private boolean scrollInProgress = false;

    private int contentPaddingLeft;
    private int contentPaddingTop;
    private int contentPaddingRight;
    private int contentPaddingBottom;

    private PointF focalPoint;

    private String styleUrl = Style.MAPBOX_STREETS;
    private boolean styleWasSet = false;

    private List<OnMapReadyCallback> onMapReadyCallbackList;
    private MapboxMap.CancelableCallback cameraCancelableCallback;
    private SnapshotRequest snapshotRequest;

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
    public MapView(@NonNull Context context, @Nullable MapboxMapOptions options) {
        super(context);
        initialize(context, options);
    }

    private void initialize(@NonNull Context context, @NonNull MapboxMapOptions options) {
        if (isInEditMode()) {
            // if we are in an editor mode we show an image of a map
            LayoutInflater.from(context).inflate(R.layout.mapbox_mapview_preview, this);
            return;
        }

        initialLoad = true;
        onMapReadyCallbackList = new ArrayList<>();
        onMapChangedListener = new CopyOnWriteArrayList<>();

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
        iconManager = new IconManager(nativeMapView);
        mapboxMap = new MapboxMap(this, iconManager);
        annotationManager = mapboxMap.getAnnotationManager();
        projection = mapboxMap.getProjection();

        // Ensure this view is interactable
        setClickable(true);
        setLongClickable(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();

        // Touch gesture detectors
        gestureDetector = new GestureDetectorCompat(context, new GestureListener());
        gestureDetector.setIsLongpressEnabled(true);
        scaleGestureDetector = new ScaleGestureDetector(context, new ScaleGestureListener());
        ScaleGestureDetectorCompat.setQuickScaleEnabled(scaleGestureDetector, true);
        rotateGestureDetector = new RotateGestureDetector(context, new RotateGestureListener());
        shoveGestureDetector = new ShoveGestureDetector(context, new ShoveGestureListener());

        zoomButtonsController = new ZoomButtonsController(this);
        zoomButtonsController.setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
        zoomButtonsController.setOnZoomListener(new OnZoomListener());

        // Connectivity
        onConnectivityChanged(isConnected());

        markerViewContainer = (ViewGroup) view.findViewById(R.id.markerViewContainer);

        myLocationView = (MyLocationView) view.findViewById(R.id.userLocationView);
        myLocationView.setMapboxMap(mapboxMap);

        compassView = (CompassView) view.findViewById(R.id.compassView);
        compassView.setMapboxMap(mapboxMap);

        logoView = (ImageView) view.findViewById(R.id.logoView);

        // Setup Attributions control
        attributionsView = (ImageView) view.findViewById(R.id.attributionView);
        attributionsView.setOnClickListener(new AttributionOnClickListener(this));

        screenDensity = context.getResources().getDisplayMetrics().density;

        setInitialState(options);

        // Shows the zoom controls
        if (!context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
            mapboxMap.getUiSettings().setZoomControlsEnabled(true);
        }
    }

    private void setInitialState(MapboxMapOptions options) {
        mapboxMap.setDebugActive(options.getDebugActive());

        CameraPosition position = options.getCamera();
        if (!position.equals(CameraPosition.DEFAULT)) {
            mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(position));
            myLocationView.setTilt(position.tilt);
        }

        // api base url
        String apiBaseUrl = options.getApiBaseUrl();
        if (!TextUtils.isEmpty(apiBaseUrl)) {
            setApiBaseUrl(apiBaseUrl);
        }

        // access token
        String accessToken = options.getAccessToken();
        if (!TextUtils.isEmpty(accessToken)) {
            mapboxMap.setAccessToken(accessToken);
        }

        // style url
        String style = options.getStyle();
        if (!TextUtils.isEmpty(style)) {
            styleUrl = style;
        }

        // MyLocationView
        MyLocationViewSettings myLocationViewSettings = mapboxMap.getMyLocationViewSettings();
        myLocationViewSettings.setForegroundDrawable(
                options.getMyLocationForegroundDrawable(), options.getMyLocationForegroundBearingDrawable());
        myLocationViewSettings.setForegroundTintColor(options.getMyLocationForegroundTintColor());
        myLocationViewSettings.setBackgroundDrawable(
                options.getMyLocationBackgroundDrawable(), options.getMyLocationBackgroundPadding());
        myLocationViewSettings.setBackgroundTintColor(options.getMyLocationBackgroundTintColor());
        myLocationViewSettings.setAccuracyAlpha(options.getMyLocationAccuracyAlpha());
        myLocationViewSettings.setAccuracyTintColor(options.getMyLocationAccuracyTintColor());
        mapboxMap.setMyLocationEnabled(options.getLocationEnabled());

        // Enable gestures
        UiSettings uiSettings = mapboxMap.getUiSettings();
        uiSettings.setZoomGesturesEnabled(options.getZoomGesturesEnabled());
        uiSettings.setZoomGestureChangeAllowed(options.getZoomGesturesEnabled());
        uiSettings.setScrollGesturesEnabled(options.getScrollGesturesEnabled());
        uiSettings.setScrollGestureChangeAllowed(options.getScrollGesturesEnabled());
        uiSettings.setRotateGesturesEnabled(options.getRotateGesturesEnabled());
        uiSettings.setRotateGestureChangeAllowed(options.getRotateGesturesEnabled());
        uiSettings.setTiltGesturesEnabled(options.getTiltGesturesEnabled());
        uiSettings.setTiltGestureChangeAllowed(options.getTiltGesturesEnabled());

        // Ui Controls
        uiSettings.setZoomControlsEnabled(options.getZoomControlsEnabled());

        // Zoom
        mapboxMap.setMaxZoom(options.getMaxZoom());
        mapboxMap.setMinZoom(options.getMinZoom());

        // Compass
        uiSettings.setCompassEnabled(options.getCompassEnabled());
        uiSettings.setCompassGravity(options.getCompassGravity());
        int[] compassMargins = options.getCompassMargins();
        if (compassMargins != null) {
            uiSettings.setCompassMargins(compassMargins[0], compassMargins[1], compassMargins[2], compassMargins[3]);
        } else {
            int tenDp = (int) getResources().getDimension(R.dimen.mapbox_ten_dp);
            uiSettings.setCompassMargins(tenDp, tenDp, tenDp, tenDp);
        }
        uiSettings.setCompassFadeFacingNorth(options.getCompassFadeFacingNorth());

        // Logo
        uiSettings.setLogoEnabled(options.getLogoEnabled());
        uiSettings.setLogoGravity(options.getLogoGravity());
        int[] logoMargins = options.getLogoMargins();
        if (logoMargins != null) {
            uiSettings.setLogoMargins(logoMargins[0], logoMargins[1], logoMargins[2], logoMargins[3]);
        } else {
            int sixteenDp = (int) getResources().getDimension(R.dimen.mapbox_sixteen_dp);
            uiSettings.setLogoMargins(sixteenDp, sixteenDp, sixteenDp, sixteenDp);
        }

        // Attribution
        uiSettings.setAttributionEnabled(options.getAttributionEnabled());
        uiSettings.setAttributionGravity(options.getAttributionGravity());
        int[] attributionMargins = options.getAttributionMargins();
        if (attributionMargins != null) {
            uiSettings.setAttributionMargins(attributionMargins[0], attributionMargins[1], attributionMargins[2], attributionMargins[3]);
        } else {
            Resources resources = getResources();
            int sevenDp = (int) resources.getDimension(R.dimen.mapbox_seven_dp);
            int seventySixDp = (int) resources.getDimension(R.dimen.mapbox_seventy_six_dp);
            uiSettings.setAttributionMargins(seventySixDp, sevenDp, sevenDp, sevenDp);
        }

        int attributionTintColor = options.getAttributionTintColor();
        uiSettings.setAttributionTintColor(attributionTintColor != -1
                ? attributionTintColor : ColorUtils.getPrimaryColor(getContext()));
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
        String accessToken = mapboxMap.getAccessToken();
        if (TextUtils.isEmpty(accessToken)) {
            accessToken = MapboxAccountManager.getInstance().getAccessToken();
            mapboxMap.setAccessToken(accessToken);
        } else {
            // user provided access token through xml attributes, need to start MapboxAccountManager
            MapboxAccountManager.start(getContext(), accessToken);
        }

        // Force a check for an access token
        MapboxAccountManager.validateAccessToken(accessToken);

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
            styleUrl = savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL);

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
            Log.i(MapView.class.getCanonicalName(), "MapView start Telemetry...");
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
                if (change == DID_FINISH_LOADING_STYLE && initialLoad) {
                    initialLoad = false;
                    iconManager.reloadIcons();
                    annotationManager.reloadMarkers();
                    annotationManager.adjustTopOffsetPixels(mapboxMap);

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
                    invalidateCameraPosition();

                } else if (change == REGION_IS_CHANGING || change == REGION_DID_CHANGE || change == DID_FINISH_LOADING_MAP) {
                    mapboxMap.getMarkerViewManager().scheduleViewMarkerInvalidation();

                    compassView.update(getDirection());
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
        outState.putString(MapboxConstants.STATE_STYLE_URL, styleUrl);
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

        // In case that no style was set or was loaded through MapboxMapOptions
        if (!styleWasSet) {
            setStyleUrl(styleUrl);
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

    void setFocalPoint(PointF focalPoint) {
        if (focalPoint == null) {
            // resetting focal point,
            UiSettings uiSettings = mapboxMap.getUiSettings();
            // need to validate if we need to reset focal point with user provided one
            if (uiSettings.getFocalPoint() != null) {
                focalPoint = uiSettings.getFocalPoint();
            }
        }
        this.focalPoint = focalPoint;
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

    //
    // LatLng / CenterCoordinate
    //

    LatLng getLatLng() {
        return nativeMapView.getLatLng();
    }

    //
    // Pitch / Tilt
    //

    double getTilt() {
        return nativeMapView.getPitch();
    }

    void setTilt(Double pitch) {
        mapboxMap.getMarkerViewManager().setTilt(pitch.floatValue());
        myLocationView.setTilt(pitch);
        nativeMapView.setPitch(pitch, 0);
    }

    //
    // Center coordinate
    //

    LatLng getCenterCoordinate(){
        return nativeMapView.getLatLng();
    }

    void setCenterCoordinate(LatLng centerCoordinate){
        nativeMapView.setLatLng(centerCoordinate);
    }

    //
    // Direction
    //

    double getDirection() {
        if (destroyed) {
            return 0;
        }

        return convertNativeBearing(nativeMapView.getBearing());
    }

    void setDirection(@FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION) double direction) {
        if (destroyed) {
            return;
        }
        setDirection(direction, false);
    }

    void setDirection(@FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION) double direction, boolean animated) {
        if (destroyed) {
            return;
        }
        long duration = animated ? MapboxConstants.ANIMATION_DURATION : 0;
        cancelTransitions();
        // Out of range directions are normalised in setBearing
        nativeMapView.setBearing(-direction, duration);
    }

    void resetNorth() {
        if (destroyed) {
            return;
        }
        myLocationView.setBearing(0);
        cancelTransitions();
        nativeMapView.resetNorth();
    }

    //
    // Content padding
    //

    int getContentPaddingLeft() {
        return contentPaddingLeft;
    }

    int getContentPaddingTop() {
        return contentPaddingTop;
    }

    int getContentPaddingRight() {
        return contentPaddingRight;
    }

    int getContentPaddingBottom() {
        return contentPaddingBottom;
    }

    //
    // Zoom
    //

    double getZoom() {
        if (destroyed) {
            return 0;
        }
        return nativeMapView.getZoom();
    }

    void setMinZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
        if (destroyed) {
            return;
        }
        nativeMapView.setMinZoom(minZoom);
    }

    double getMinZoom() {
        if (destroyed) {
            return 0;
        }
        return nativeMapView.getMinZoom();
    }

    void setMaxZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double maxZoom) {
        if (destroyed) {
            return;
        }
        nativeMapView.setMaxZoom(maxZoom);
    }

    double getMaxZoom() {
        if (destroyed) {
            return 0;
        }
        return nativeMapView.getMaxZoom();
    }

    // Zoom in or out
    private void zoom(boolean zoomIn) {
        zoom(zoomIn, -1.0f, -1.0f);
    }

    private void zoom(boolean zoomIn, float x, float y) {
        // Cancel any animation
        cancelTransitions();

        if (zoomIn) {
            nativeMapView.scaleBy(2.0, x / screenDensity, y / screenDensity, MapboxConstants.ANIMATION_DURATION);
        } else {
            nativeMapView.scaleBy(0.5, x / screenDensity, y / screenDensity, MapboxConstants.ANIMATION_DURATION);
        }

        // work around to invalidate camera position
        postDelayed(new ZoomInvalidator(mapboxMap), MapboxConstants.ANIMATION_DURATION);
    }

    //
    // Debug
    //

    boolean isDebugActive() {
        return !destroyed && nativeMapView.getDebug();
    }

    void setDebugActive(boolean debugActive) {
        if (destroyed) {
            return;
        }
        nativeMapView.setDebug(debugActive);
    }

    void cycleDebugOptions() {
        if (destroyed) {
            return;
        }
        nativeMapView.cycleDebugOptions();
    }

    //
    // Styling
    //

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

        styleUrl = url;
        nativeMapView.setStyleUrl(url);
        styleWasSet = true;
    }

    /**
     * <p>
     * Loads a new map style from the specified bundled style.
     * </p>
     * <p>
     * This method is asynchronous and will return immediately before the style finishes loading.
     * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
     * </p>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
     *
     * @param style The bundled style. Accepts one of the values from {@link Style}.
     * @see Style
     */
    @UiThread
    public void setStyle(@Style.StyleUrl String style) {
        setStyleUrl(style);
    }

    /**
     * <p>
     * Returns the map style currently displayed in the map view.
     * </p>
     * If the default style is currently displayed, a URL will be returned instead of null.
     *
     * @return The URL of the map style.
     */
    @UiThread
    @NonNull
    public String getStyleUrl() {
        return styleUrl;
    }

    //
    // API Base URL
    //

    @UiThread
    void setApiBaseUrl(@NonNull String baseUrl) {
        nativeMapView.setApiBaseUrl(baseUrl);
    }

    //
    // Access token
    //

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
    // Projection
    //

    /*
     * Internal use only, use Projection#fromScreenLocation instead
     */
    LatLng fromNativeScreenLocation(@NonNull PointF point) {
        if (destroyed) {
            return new LatLng();
        }
        return nativeMapView.latLngForPixel(point);
    }

    /*
     * Internal use only, use Projection#toScreenLocation instead.
     */
    PointF toNativeScreenLocation(@NonNull LatLng location) {
        if (destroyed || location == null) {
            return new PointF();
        }
        return nativeMapView.pixelForLatLng(location);
    }

    /**
     * @return the ViewGroup containing the marker views
     */
    public ViewGroup getMarkerViewContainer() {
        return markerViewContainer;
    }

    void setContentPadding(int left, int top, int right, int bottom) {
        if (destroyed) {
            return;
        }

//        if (left == contentPaddingLeft && top == contentPaddingTop && right == contentPaddingRight && bottom == contentPaddingBottom) {
//            return;
//        }

        contentPaddingLeft = left;
        contentPaddingTop = top;
        contentPaddingRight = right;
        contentPaddingBottom = bottom;

        int[] userLocationViewPadding = mapboxMap.getMyLocationViewSettings().getPadding();
        left += userLocationViewPadding[0];
        top += userLocationViewPadding[1];
        right += userLocationViewPadding[2];
        bottom += userLocationViewPadding[3];

        nativeMapView.setContentPadding(top / screenDensity, left / screenDensity, bottom / screenDensity, right / screenDensity);
    }

    public void invalidateContentPadding() {
        setContentPadding(contentPaddingLeft, contentPaddingTop, contentPaddingRight, contentPaddingBottom);

        if (!mapboxMap.getTrackingSettings().isLocationTrackingDisabled()) {
            setFocalPoint(new PointF(myLocationView.getCenterX(), myLocationView.getCenterY()));
        } else {
            setFocalPoint(null);
        }
    }

    double getMetersPerPixelAtLatitude(@FloatRange(from = -180, to = 180) double latitude) {
        if (destroyed) {
            return 0;
        }

        return nativeMapView.getMetersPerPixelAtLatitude(latitude, getZoom()) / screenDensity;
    }

    //
    // Mapbox Core GL Camera
    //

    private void cancelTransitions(){
        if (cameraCancelableCallback != null) {
            cameraCancelableCallback.onCancel();
            cameraCancelableCallback = null;
        }
        nativeMapView.cancelTransitions();
    }

    void jumpTo(double bearing, LatLng center, double pitch, double zoom) {
        if (destroyed) {
            return;
        }
        cancelTransitions();
        nativeMapView.jumpTo(bearing, center, pitch, zoom);
    }

    void easeTo(double bearing, LatLng center, long duration, double pitch, double zoom, boolean easingInterpolator, @Nullable final MapboxMap.CancelableCallback cancelableCallback) {
        if (destroyed) {
            return;
        }
        cancelTransitions();

        // Register callbacks early enough
        if (cancelableCallback != null) {
            cameraCancelableCallback = cancelableCallback;
            addOnMapChangedListener(new OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
                        cameraCancelableCallback.onFinish();
                        cameraCancelableCallback = null;
                        // Clean up after self
                        removeOnMapChangedListener(this);
                    }
                }
            });
        }

        nativeMapView.easeTo(bearing, center, duration, pitch, zoom, easingInterpolator);
    }

    void flyTo(double bearing, LatLng center, long duration, double pitch, double zoom, @Nullable final MapboxMap.CancelableCallback cancelableCallback) {
        if (destroyed) {
            return;
        }
        cancelTransitions();

        // Register callbacks early enough
        if (cancelableCallback != null) {
            cameraCancelableCallback = cancelableCallback;
            addOnMapChangedListener(new OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED && cameraCancelableCallback != null) {
                        cancelableCallback.onFinish();
                        cameraCancelableCallback = null;
                        // Clean up after self
                        removeOnMapChangedListener(this);
                    }
                }
            });
        }

        nativeMapView.flyTo(bearing, center, duration, pitch, zoom);
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
            nativeMapView.resizeView((int) (width / screenDensity), (int) (height / screenDensity));
        }
    }

    double getScale() {
        if (destroyed) {
            return 0;
        }

        return nativeMapView.getScale();
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
            compassView.update(getDirection());
            myLocationView.update();
            mapboxMap.getMarkerViewManager().update();

            for (InfoWindow infoWindow : mapboxMap.getInfoWindows()) {
                infoWindow.update();
            }
        }
    }

    CameraPosition invalidateCameraPosition() {
        if (destroyed) {
            return new CameraPosition.Builder().build();
        }
        CameraPosition position = new CameraPosition.Builder(nativeMapView.getCameraValues()).build();
        myLocationView.setCameraPosition(position);
        mapboxMap.getMarkerViewManager().setTilt((float) position.tilt);
        return position;
    }

    double getBearing() {
        if (destroyed) {
            return 0;
        }

        double direction = -nativeMapView.getBearing();

        while (direction > 360) {
            direction -= 360;
        }
        while (direction < 0) {
            direction += 360;
        }

        return direction;
    }

    void setBearing(double bearing) {
        if (destroyed) {
            return;
        }
        myLocationView.setBearing(bearing);
        nativeMapView.setBearing(bearing);
    }

    void setBearing(double bearing, long duration) {
        if (destroyed) {
            return;
        }
        myLocationView.setBearing(bearing);
        nativeMapView.setBearing(bearing, duration);
    }

    void setBearing(double bearing, float focalX, float focalY) {
        if (destroyed) {
            return;
        }
        myLocationView.setBearing(bearing);
        nativeMapView.setBearing(bearing, focalX, focalY);
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

        // Required by ZoomButtonController (from Android SDK documentation)
        if (visibility == View.VISIBLE) {
            if (mapboxMap != null && mapboxMap.getUiSettings().isZoomControlsEnabled()) {
                zoomButtonsController.setVisible(true);
            }
        } else {
            if (mapboxMap != null && mapboxMap.getUiSettings().isZoomControlsEnabled()) {
                zoomButtonsController.setVisible(false);
            }
        }
    }

    //
    // Touch events
    //

    /**
     * Helper method for tracking gesture events
     *
     * @param gestureId   Type of Gesture See {@see MapboxEvent#GESTURE_SINGLETAP MapboxEvent#GESTURE_DOUBLETAP MapboxEvent#GESTURE_TWO_FINGER_SINGLETAP MapboxEvent#GESTURE_QUICK_ZOOM MapboxEvent#GESTURE_PAN_START MapboxEvent#GESTURE_PINCH_START MapboxEvent#GESTURE_ROTATION_START MapboxEvent#GESTURE_PITCH_START}
     * @param xCoordinate Original x screen coordinate at start of gesture
     * @param yCoordinate Original y screen cooridnate at start of gesture
     */
    private void trackGestureEvent(@NonNull String gestureId, float xCoordinate, float yCoordinate) {
        LatLng tapLatLng = projection.fromScreenLocation(new PointF(xCoordinate, yCoordinate));

        // NaN and Infinite checks to prevent JSON errors at send to server time
        if (Double.isNaN(tapLatLng.getLatitude()) || Double.isNaN(tapLatLng.getLongitude())) {
            Log.d(MapView.class.getSimpleName(), "trackGestureEvent() has a NaN lat or lon.  Returning.");
            return;
        }

        if (Double.isInfinite(tapLatLng.getLatitude()) || Double.isInfinite(tapLatLng.getLongitude())) {
            Log.d(MapView.class.getSimpleName(), "trackGestureEvent() has an Infinite lat or lon.  Returning.");
            return;
        }

        Hashtable<String, Object> evt = new Hashtable<>();
        evt.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_MAP_CLICK);
        evt.put(MapboxEvent.ATTRIBUTE_CREATED, MapboxEventManager.generateCreateDate());
        evt.put(MapboxEvent.KEY_GESTURE_ID, gestureId);
        evt.put(MapboxEvent.KEY_LATITUDE, tapLatLng.getLatitude());
        evt.put(MapboxEvent.KEY_LONGITUDE, tapLatLng.getLongitude());
        evt.put(MapboxEvent.KEY_ZOOM, mapboxMap.getCameraPosition().zoom);

        MapboxEventManager.getMapboxEventManager().pushEvent(evt);
    }

    /**
     * Helper method for tracking DragEnd gesture event
     * See {@see MapboxEvent#TYPE_MAP_DRAGEND}
     *
     * @param xCoordinate Original x screen coordinate at end of drag
     * @param yCoordinate Orginal y screen coordinate at end of drag
     */
    private void trackGestureDragEndEvent(float xCoordinate, float yCoordinate) {
        LatLng tapLatLng = projection.fromScreenLocation(new PointF(xCoordinate, yCoordinate));

        // NaN and Infinite checks to prevent JSON errors at send to server time
        if (Double.isNaN(tapLatLng.getLatitude()) || Double.isNaN(tapLatLng.getLongitude())) {
            Log.d(MapView.class.getSimpleName(), "trackGestureDragEndEvent() has a NaN lat or lon.  Returning.");
            return;
        }

        if (Double.isInfinite(tapLatLng.getLatitude()) || Double.isInfinite(tapLatLng.getLongitude())) {
            Log.d(MapView.class.getSimpleName(), "trackGestureDragEndEvent() has an Infinite lat or lon.  Returning.");
            return;
        }

        Hashtable<String, Object> evt = new Hashtable<>();
        evt.put(MapboxEvent.ATTRIBUTE_EVENT, MapboxEvent.TYPE_MAP_DRAGEND);
        evt.put(MapboxEvent.ATTRIBUTE_CREATED, MapboxEventManager.generateCreateDate());
        evt.put(MapboxEvent.KEY_LATITUDE, tapLatLng.getLatitude());
        evt.put(MapboxEvent.KEY_LONGITUDE, tapLatLng.getLongitude());
        evt.put(MapboxEvent.KEY_ZOOM, mapboxMap.getCameraPosition().zoom);

        MapboxEventManager.getMapboxEventManager().pushEvent(evt);
    }

    // Called when user touches the screen, all positions are absolute
    @Override
    public boolean onTouchEvent(@NonNull MotionEvent event) {
        // Check and ignore non touch or left clicks
        if (destroyed) {
            return super.onTouchEvent(event);
        }

        if ((event.getButtonState() != 0) && (event.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
            return false;
        }

        // Check two finger gestures first
        rotateGestureDetector.onTouchEvent(event);
        scaleGestureDetector.onTouchEvent(event);
        shoveGestureDetector.onTouchEvent(event);

        // Handle two finger tap
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                // First pointer down
                nativeMapView.setGestureInProgress(true);
                break;

            case MotionEvent.ACTION_POINTER_DOWN:
                // Second pointer down
                twoTap = event.getPointerCount() == 2
                        && mapboxMap.getUiSettings().isZoomGesturesEnabled();
                if (twoTap) {
                    // Confirmed 2nd Finger Down
                    trackGestureEvent(MapboxEvent.GESTURE_TWO_FINGER_SINGLETAP, event.getX(), event.getY());
                }
                break;

            case MotionEvent.ACTION_POINTER_UP:
                // Second pointer up
                break;

            case MotionEvent.ACTION_UP:
                // First pointer up
                long tapInterval = event.getEventTime() - event.getDownTime();
                boolean isTap = tapInterval <= ViewConfiguration.getTapTimeout();
                boolean inProgress = rotateGestureDetector.isInProgress()
                        || scaleGestureDetector.isInProgress()
                        || shoveGestureDetector.isInProgress();

                if (twoTap && isTap && !inProgress) {
                    if (focalPoint != null) {
                        zoom(false, focalPoint.x, focalPoint.y);
                    } else {
                        PointF focalPoint = TwoFingerGestureDetector.determineFocalPoint(event);
                        zoom(false, focalPoint.x, focalPoint.y);
                    }
                    twoTap = false;
                    return true;
                }

                // Scroll / Pan Has Stopped
                if (scrollInProgress) {
                    trackGestureDragEndEvent(event.getX(), event.getY());
                    scrollInProgress = false;
                }

                twoTap = false;
                nativeMapView.setGestureInProgress(false);
                break;

            case MotionEvent.ACTION_CANCEL:
                twoTap = false;
                nativeMapView.setGestureInProgress(false);
                break;
        }

        boolean retVal = gestureDetector.onTouchEvent(event);
        return retVal || super.onTouchEvent(event);
    }

    // This class handles one finger gestures
    private class GestureListener extends GestureDetector.SimpleOnGestureListener {

        // Must always return true otherwise all events are ignored
        @Override
        @SuppressLint("ResourceType")
        public boolean onDown(MotionEvent event) {
            // Show the zoom controls
            if (mapboxMap.getUiSettings().isZoomControlsEnabled()) {
                zoomButtonsController.setVisible(true);
            }
            return true;
        }

        // Called for double taps
        @Override
        public boolean onDoubleTapEvent(MotionEvent e) {
            if (destroyed || !mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                return false;
            }

            switch (e.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    break;
                case MotionEvent.ACTION_MOVE:
                    break;
                case MotionEvent.ACTION_UP:
                    if (quickZoom) {
                        // insert here?
                        quickZoom = false;
                        break;
                    }

                    // Single finger double tap
                    if (focalPoint != null) {
                        // User provided focal point
                        zoom(true, focalPoint.x, focalPoint.y);
                    } else {
                        // Zoom in on gesture
                        zoom(true, e.getX(), e.getY());
                    }
                    break;
            }

            trackGestureEvent(MapboxEvent.GESTURE_DOUBLETAP, e.getX(), e.getY());

            return true;
        }

        @Override
        public boolean onSingleTapUp(MotionEvent motionEvent) {
            if (destroyed) {
                return false;
            }
            // Cancel any animation
            cancelTransitions();
            return true;
        }

        @Override
        public boolean onSingleTapConfirmed(MotionEvent motionEvent) {
            List<Marker> selectedMarkers = mapboxMap.getSelectedMarkers();

            PointF tapPoint = new PointF(motionEvent.getX(), motionEvent.getY());
            float toleranceSides = 4 * screenDensity;
            float toleranceTopBottom = 10 * screenDensity;

            RectF tapRect = new RectF(tapPoint.x - iconManager.getAverageIconWidth() / 2 - toleranceSides,
                    tapPoint.y - iconManager.getAverageIconHeight() / 2 - toleranceTopBottom,
                    tapPoint.x + iconManager.getAverageIconWidth() / 2 + toleranceSides,
                    tapPoint.y + iconManager.getAverageIconHeight() / 2 + toleranceTopBottom);

            List<Marker> nearbyMarkers = annotationManager.getMarkersInRect(tapRect);
            long newSelectedMarkerId = -1;

            if (nearbyMarkers != null && nearbyMarkers.size() > 0) {
                Collections.sort(nearbyMarkers);
                for (Marker nearbyMarker : nearbyMarkers) {
                    boolean found = false;
                    for (Marker selectedMarker : selectedMarkers) {
                        if (selectedMarker.equals(nearbyMarker)) {
                            found = true;
                        }
                    }
                    if (!found) {
                        newSelectedMarkerId = nearbyMarker.getId();
                        break;
                    }
                }
            }

            if (newSelectedMarkerId >= 0) {
                List<Annotation> annotations = mapboxMap.getAnnotations();
                int count = annotations.size();
                for (int i = 0; i < count; i++) {
                    Annotation annotation = annotations.get(i);
                    if (annotation instanceof Marker) {
                        if (annotation.getId() == newSelectedMarkerId) {
                            if (selectedMarkers.isEmpty() || !selectedMarkers.contains(annotation)) {
                                if (!(annotation instanceof MarkerView)) {
                                    mapboxMap.selectMarker((Marker) annotation);
                                } else {
                                    mapboxMap.getMarkerViewManager().onClickMarkerView((MarkerView) annotation);
                                }
                            }
                            break;
                        }
                    }
                }
            } else {
                if (mapboxMap.getUiSettings().isDeselectMarkersOnTap()) {
                    // deselect any selected marker
                    mapboxMap.deselectMarkers();
                }

                // notify app of map click
                MapboxMap.OnMapClickListener listener = mapboxMap.getOnMapClickListener();
                if (listener != null) {
                    LatLng point = projection.fromScreenLocation(tapPoint);
                    listener.onMapClick(point);
                }
            }

            trackGestureEvent(MapboxEvent.GESTURE_SINGLETAP, motionEvent.getX(), motionEvent.getY());
            return true;
        }

        // Called for a long press
        @Override
        public void onLongPress(MotionEvent motionEvent) {
            MapboxMap.OnMapLongClickListener listener = mapboxMap.getOnMapLongClickListener();
            if (listener != null && !quickZoom) {
                LatLng point = projection.fromScreenLocation(new PointF(motionEvent.getX(), motionEvent.getY()));
                listener.onMapLongClick(point);
            }
        }

        // Called for flings
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            if (destroyed || !mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                return false;
            }

            resetTrackingModesIfRequired(true, false);

            double decelerationRate = 1;

            // Cancel any animation
            cancelTransitions();

            double offsetX = velocityX * decelerationRate / 4 / screenDensity;
            double offsetY = velocityY * decelerationRate / 4 / screenDensity;

            nativeMapView.setGestureInProgress(true);
            nativeMapView.moveBy(offsetX, offsetY, (long) (decelerationRate * 1000.0f));
            nativeMapView.setGestureInProgress(false);

            MapboxMap.OnFlingListener listener = mapboxMap.getOnFlingListener();
            if (listener != null) {
                listener.onFling();
            }

            trackGestureEvent(MapboxEvent.GESTURE_PAN_START, e1.getX(), e1.getY());
            return true;
        }

        // Called for drags
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (!scrollInProgress) {
                scrollInProgress = true;
            }
            if (destroyed || !mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                return false;
            }

            if (dragStarted) {
                return false;
            }

            requestDisallowInterceptTouchEvent(true);

            // reset tracking if needed
            resetTrackingModesIfRequired(true, false);
            // Cancel any animation
            cancelTransitions();

            // Scroll the map
            nativeMapView.moveBy(-distanceX / screenDensity, -distanceY / screenDensity);

            MapboxMap.OnScrollListener listener = mapboxMap.getOnScrollListener();
            if (listener != null) {
                listener.onScroll();
            }
            return true;
        }
    }

    // This class handles two finger gestures and double-tap drag gestures
    private class ScaleGestureListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {

        long beginTime = 0;
        float scaleFactor = 1.0f;

        // Called when two fingers first touch the screen
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            if (destroyed || !mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                return false;
            }

            beginTime = detector.getEventTime();
            trackGestureEvent(MapboxEvent.GESTURE_PINCH_START, detector.getFocusX(), detector.getFocusY());
            return true;
        }

        // Called when fingers leave screen
        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
            beginTime = 0;
            scaleFactor = 1.0f;
            zoomStarted = false;
        }

        // Called each time a finger moves
        // Called for pinch zooms and quickzooms/quickscales
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            UiSettings uiSettings = mapboxMap.getUiSettings();
            if (destroyed || !uiSettings.isZoomGesturesEnabled()) {
                return super.onScale(detector);
            }

            // If scale is large enough ignore a tap
            scaleFactor *= detector.getScaleFactor();
            if ((scaleFactor > 1.05f) || (scaleFactor < 0.95f)) {
                zoomStarted = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small scales
            long time = detector.getEventTime();
            long interval = time - beginTime;
            if (!zoomStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            if (!zoomStarted) {
                return false;
            }

            if (dragStarted) {
                return false;
            }

            // Cancel any animation
            cancelTransitions();

            // Gesture is a quickzoom if there aren't two fingers
            quickZoom = !twoTap;

            // make an assumption here; if the zoom center is specified by the gesture, it's NOT going
            // to be in the center of the map. Therefore the zoom will translate the map center, so tracking
            // should be disabled.

            resetTrackingModesIfRequired(!quickZoom, false);
            // Scale the map
            if (focalPoint != null) {
                // arround user provided focal point
                nativeMapView.scaleBy(detector.getScaleFactor(), focalPoint.x / screenDensity, focalPoint.y / screenDensity);
            } else if (quickZoom) {
                // around center map
                nativeMapView.scaleBy(detector.getScaleFactor(), (getWidth() / 2) / screenDensity, (getHeight() / 2) / screenDensity);
            } else {
                // around gesture
                nativeMapView.scaleBy(detector.getScaleFactor(), detector.getFocusX() / screenDensity, detector.getFocusY() / screenDensity);
            }

            return true;
        }
    }

    // This class handles two finger rotate gestures
    private class RotateGestureListener extends RotateGestureDetector.SimpleOnRotateGestureListener {

        long beginTime = 0;
        float totalAngle = 0.0f;
        boolean started = false;

        // Called when two fingers first touch the screen
        @Override
        public boolean onRotateBegin(RotateGestureDetector detector) {
            if (destroyed || !mapboxMap.getTrackingSettings().isRotateGestureCurrentlyEnabled()) {
                return false;
            }

            beginTime = detector.getEventTime();
            trackGestureEvent(MapboxEvent.GESTURE_ROTATION_START, detector.getFocusX(), detector.getFocusY());
            return true;
        }

        // Called when the fingers leave the screen
        @Override
        public void onRotateEnd(RotateGestureDetector detector) {
            beginTime = 0;
            totalAngle = 0.0f;
            started = false;
        }

        // Called each time one of the two fingers moves
        // Called for rotation
        @Override
        public boolean onRotate(RotateGestureDetector detector) {
            if (destroyed || !mapboxMap.getTrackingSettings().isRotateGestureCurrentlyEnabled() || dragStarted) {
                return false;
            }

            // If rotate is large enough ignore a tap
            // Also is zoom already started, don't rotate
            totalAngle += detector.getRotationDegreesDelta();
            if (!zoomStarted && ((totalAngle > 20.0f) || (totalAngle < -20.0f))) {
                started = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small rotate
            long time = detector.getEventTime();
            long interval = time - beginTime;
            if (!started && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            if (!started) {
                return false;
            }

            // Cancel any animation
            cancelTransitions();

            // rotation constitutes translation of anything except the center of
            // rotation, so cancel both location and bearing tracking if required

            resetTrackingModesIfRequired(true, true);

            // Get rotate value
            double bearing = nativeMapView.getBearing();
            bearing += detector.getRotationDegreesDelta();

            // Rotate the map
            if (focalPoint != null) {
                // User provided focal point
                setBearing(bearing, focalPoint.x / screenDensity, focalPoint.y / screenDensity);
            } else {
                // around gesture
                setBearing(bearing, detector.getFocusX() / screenDensity, detector.getFocusY() / screenDensity);
            }
            return true;
        }
    }

    // This class handles a vertical two-finger shove. (If you place two fingers on screen with
    // less than a 20 degree angle between them, this will detect movement on the Y-axis.)
    private class ShoveGestureListener implements ShoveGestureDetector.OnShoveGestureListener {

        long beginTime = 0;
        float totalDelta = 0.0f;
        boolean started = false;

        @Override
        public boolean onShoveBegin(ShoveGestureDetector detector) {
            if (!mapboxMap.getUiSettings().isTiltGesturesEnabled()) {
                return false;
            }

            beginTime = detector.getEventTime();
            trackGestureEvent(MapboxEvent.GESTURE_PITCH_START, detector.getFocusX(), detector.getFocusY());
            return true;
        }

        @Override
        public void onShoveEnd(ShoveGestureDetector detector) {
            beginTime = 0;
            totalDelta = 0.0f;
            started = false;
            dragStarted = false;
        }

        @Override
        public boolean onShove(ShoveGestureDetector detector) {
            if (destroyed || !mapboxMap.getUiSettings().isTiltGesturesEnabled()) {
                return false;
            }

            // If tilt is large enough ignore a tap
            // Also if zoom already started, don't tilt
            totalDelta += detector.getShovePixelsDelta();
            if (!zoomStarted && ((totalDelta > 10.0f) || (totalDelta < -10.0f))) {
                started = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small tilt
            long time = detector.getEventTime();
            long interval = time - beginTime;
            if (!started && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            if (!started) {
                return false;
            }

            // Cancel any animation
            cancelTransitions();

            // Get tilt value (scale and clamp)
            double pitch = getTilt();
            pitch -= 0.1 * detector.getShovePixelsDelta();
            pitch = Math.max(MapboxConstants.MINIMUM_TILT, Math.min(MapboxConstants.MAXIMUM_TILT, pitch));

            // Tilt the map
            mapboxMap.setTilt(pitch);

            dragStarted = true;

            return true;
        }
    }

    // This class handles input events from the zoom control buttons
    // Zoom controls allow single touch only devices to zoom in and out
    private class OnZoomListener implements ZoomButtonsController.OnZoomListener {

        // Not used
        @Override
        public void onVisibilityChanged(boolean visible) {
            // Ignore
        }

        // Called when user pushes a zoom button
        @Override
        public void onZoom(boolean zoomIn) {
            if (!mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                return;
            }
            zoom(zoomIn);
        }
    }

    //
    // Input events
    //

    // Called when the user presses a key, also called for repeating keys held
    // down
    @Override
    public boolean onKeyDown(int keyCode, @NonNull KeyEvent event) {
        if (destroyed) {
            return super.onKeyDown(keyCode, event);
        }

        // If the user has held the scroll key down for a while then accelerate
        // the scroll speed
        double scrollDist = event.getRepeatCount() >= 5 ? 50.0 : 10.0;

        // Check which key was pressed via hardware/real key code
        switch (keyCode) {
            // Tell the system to track these keys for long presses on
            // onKeyLongPress is fired
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_DPAD_CENTER:
                event.startTracking();
                return true;

            case KeyEvent.KEYCODE_DPAD_LEFT:
                if (!mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                    return false;
                }

                // Cancel any animation
                cancelTransitions();

                // Move left
                nativeMapView.moveBy(scrollDist / screenDensity, 0.0 / screenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_RIGHT:
                if (!mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                    return false;
                }

                // Cancel any animation
                cancelTransitions();

                // Move right
                nativeMapView.moveBy(-scrollDist / screenDensity, 0.0 / screenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_UP:
                if (!mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                    return false;
                }

                // Cancel any animation
                cancelTransitions();

                // Move up
                nativeMapView.moveBy(0.0 / screenDensity, scrollDist / screenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_DOWN:
                if (!mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                    return false;
                }

                // Cancel any animation
                cancelTransitions();

                // Move down
                nativeMapView.moveBy(0.0 / screenDensity, -scrollDist / screenDensity);
                return true;

            default:
                // We are not interested in this key
                return super.onKeyUp(keyCode, event);
        }
    }

    // Called when the user long presses a key that is being tracked
    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event) {
        // Check which key was pressed via hardware/real key code
        switch (keyCode) {
            // Tell the system to track these keys for long presses on
            // onKeyLongPress is fired
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_DPAD_CENTER:
                if (!mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                    return false;
                }

                // Zoom out
                zoom(false);
                return true;

            default:
                // We are not interested in this key
                return super.onKeyUp(keyCode, event);
        }
    }

    // Called when the user releases a key
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        // Check if the key action was canceled (used for virtual keyboards)
        if (event.isCanceled()) {
            return super.onKeyUp(keyCode, event);
        }

        // Check which key was pressed via hardware/real key code
        // Note if keyboard does not have physical key (ie primary non-shifted
        // key) then it will not appear here
        // Must use the key character map as physical to character is not
        // fixed/guaranteed
        switch (keyCode) {
            case KeyEvent.KEYCODE_ENTER:
            case KeyEvent.KEYCODE_DPAD_CENTER:
                if (!mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                    return false;
                }

                // Zoom in
                zoom(true);
                return true;
        }

        // We are not interested in this key
        return super.onKeyUp(keyCode, event);
    }

    // Called for trackball events, all motions are relative in device specific
    // units
    @Override
    public boolean onTrackballEvent(MotionEvent event) {
        if (destroyed) {
            return false;
        }
        // Choose the action
        switch (event.getActionMasked()) {
            // The trackball was rotated
            case MotionEvent.ACTION_MOVE:
                if (!mapboxMap.getTrackingSettings().isScrollGestureCurrentlyEnabled()) {
                    return false;
                }

                // Cancel any animation
                cancelTransitions();

                // Scroll the map
                nativeMapView.moveBy(-10.0 * event.getX() / screenDensity, -10.0 * event.getY() / screenDensity);
                return true;

            // Trackball was pushed in so start tracking and tell system we are
            // interested
            // We will then get the up action
            case MotionEvent.ACTION_DOWN:
                // Set up a delayed callback to check if trackball is still
                // After waiting the system long press time out
                if (currentTrackballLongPressTimeOut != null) {
                    currentTrackballLongPressTimeOut.cancel();
                    currentTrackballLongPressTimeOut = null;
                }
                currentTrackballLongPressTimeOut = new TrackballLongPressTimeOut();
                postDelayed(currentTrackballLongPressTimeOut,
                        ViewConfiguration.getLongPressTimeout());
                return true;

            // Trackball was released
            case MotionEvent.ACTION_UP:
                if (!mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                    return false;
                }

                // Only handle if we have not already long pressed
                if (currentTrackballLongPressTimeOut != null) {
                    // Zoom in
                    zoom(true);
                }
                return true;

            // Trackball was cancelled
            case MotionEvent.ACTION_CANCEL:
                if (currentTrackballLongPressTimeOut != null) {
                    currentTrackballLongPressTimeOut.cancel();
                    currentTrackballLongPressTimeOut = null;
                }
                return true;

            default:
                // We are not interested in this event
                return super.onTrackballEvent(event);
        }
    }

    // This class implements the trackball long press time out callback
    private class TrackballLongPressTimeOut implements Runnable {

        // Track if we have been cancelled
        private boolean cancelled;

        TrackballLongPressTimeOut() {
            cancelled = false;
        }

        // Cancel the timeout
        public void cancel() {
            cancelled = true;
        }

        // Called when long press time out expires
        @Override
        public void run() {
            // Check if the trackball is still pressed
            if (!cancelled) {
                // Zoom out
                zoom(false);

                // Ensure the up action is not run
                currentTrackballLongPressTimeOut = null;
            }
        }
    }

    // Called for events that don't fit the other handlers
    // such as mouse scroll events, mouse moves, joystick, trackpad
    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        if (destroyed) {
            return false;
        }
        // Mouse events
        //if (event.isFromSource(InputDevice.SOURCE_CLASS_POINTER)) { // this is not available before API 18
        if ((event.getSource() & InputDevice.SOURCE_CLASS_POINTER) == InputDevice.SOURCE_CLASS_POINTER) {
            // Choose the action
            switch (event.getActionMasked()) {
                // Mouse scrolls
                case MotionEvent.ACTION_SCROLL:
                    if (!mapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                        return false;
                    }

                    // Cancel any animation
                    cancelTransitions();

                    // Get the vertical scroll amount, one click = 1
                    float scrollDist = event.getAxisValue(MotionEvent.AXIS_VSCROLL);

                    // Scale the map by the appropriate power of two factor
                    nativeMapView.scaleBy(Math.pow(2.0, scrollDist), event.getX() / screenDensity, event.getY() / screenDensity);

                    return true;

                default:
                    // We are not interested in this event
                    return super.onGenericMotionEvent(event);
            }
        }

        // We are not interested in this event
        return super.onGenericMotionEvent(event);
    }

    // Called when the mouse pointer enters or exits the view
    // or when it fades in or out due to movement
    @Override
    public boolean onHoverEvent(@NonNull MotionEvent event) {
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
                return super.onHoverEvent(event);
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
            onMapChangedListener.add(listener);
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
            onMapChangedListener.remove(listener);
        }
    }

    // Called when the map view transformation has changed
    // Called via JNI from NativeMapView
    // Forward to any listeners
    protected void onMapChanged(int mapChange) {
        if (onMapChangedListener != null) {
            OnMapChangedListener listener;
            final Iterator<OnMapChangedListener> iterator = onMapChangedListener.iterator();
            while (iterator.hasNext()) {
                listener = iterator.next();
                listener.onMapChanged(mapChange);
            }
        }
    }

    //
    // User location
    //

    void setMyLocationEnabled(boolean enabled) {
        myLocationView.setEnabled(enabled);
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
            LocationServices.getLocationServices(getContext()).addLocationListener(myLocationListener);
        } else {
            LocationServices.getLocationServices(getContext()).removeLocationListener(myLocationListener);
            myLocationListener = null;
        }
    }

    void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && !mapboxMap.isMyLocationEnabled()) {
            mapboxMap.setMyLocationEnabled(true);
        }
        myLocationView.setMyLocationTrackingMode(myLocationTrackingMode);

        if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
            setFocalPoint(new PointF(myLocationView.getCenterX(), myLocationView.getCenterY()));
        } else {
            setFocalPoint(null);
        }

        MapboxMap.OnMyLocationTrackingModeChangeListener listener = mapboxMap.getOnMyLocationTrackingModeChangeListener();
        if (listener != null) {
            listener.onMyLocationTrackingModeChange(myLocationTrackingMode);
        }
    }

    void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        if (myBearingTrackingMode != MyBearingTracking.NONE && !mapboxMap.isMyLocationEnabled()) {
            mapboxMap.setMyLocationEnabled(true);
        }
        myLocationView.setMyBearingTrackingMode(myBearingTrackingMode);
        MapboxMap.OnMyBearingTrackingModeChangeListener listener = mapboxMap.getOnMyBearingTrackingModeChangeListener();
        if (listener != null) {
            listener.onMyBearingTrackingModeChange(myBearingTrackingMode);
        }
    }

    boolean isPermissionsAccepted() {
        return (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED) ||
                ContextCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    void resetTrackingModesIfRequired(boolean translate, boolean rotate) {
        TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();

        // if tracking is on, and we should dismiss tracking with gestures, and this is a scroll action, turn tracking off
        if (translate && !trackingSettings.isLocationTrackingDisabled() && trackingSettings.isDismissLocationTrackingOnGesture()) {
            resetLocationTrackingMode();
        }

        // reset bearing tracking only on rotate
        if (rotate && !trackingSettings.isBearingTrackingDisabled() && trackingSettings.isDismissBearingTrackingOnGesture()) {
            resetBearingTrackingMode();
        }
    }

    void resetTrackingModesIfRequired(CameraPosition cameraPosition) {
        resetTrackingModesIfRequired(cameraPosition.target != null, cameraPosition.bearing != -1);
    }

    private void resetLocationTrackingMode() {
        try {
            TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
            trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
        } catch (SecurityException ignore) {
            // User did not accept location permissions
        }
    }

    private void resetBearingTrackingMode() {
        try {
            TrackingSettings trackingSettings = mapboxMap.getTrackingSettings();
            trackingSettings.setMyBearingTrackingMode(MyBearingTracking.NONE);
        } catch (SecurityException ignore) {
            // User did not accept location permissions
        }
    }

    //
    // Compass
    //

    void setCompassEnabled(boolean compassEnabled) {
        compassView.setEnabled(compassEnabled);
    }

    void setCompassGravity(int gravity) {
        setWidgetGravity(compassView, gravity);
    }

    void setCompassMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(compassView, left, top, right, bottom);
    }

    void setCompassFadeFacingNorth(boolean compassFadeFacingNorth) {
        compassView.fadeCompassViewFacingNorth(compassFadeFacingNorth);
    }

    //
    // Logo
    //

    void setLogoGravity(int gravity) {
        setWidgetGravity(logoView, gravity);
    }

    void setLogoMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(logoView, left, top, right, bottom);
    }

    void setLogoEnabled(boolean visible) {
        logoView.setVisibility(visible ? View.VISIBLE : View.GONE);
    }

    //
    // Attribution
    //

    void setAttributionGravity(int gravity) {
        setWidgetGravity(attributionsView, gravity);
    }

    void setAttributionMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(attributionsView, left, top, right, bottom);
    }

    void setAttributionEnabled(int visibility) {
        attributionsView.setVisibility(visibility);
    }

    void setAtttibutionTintColor(int tintColor) {
        // Check that the tint color being passed in isn't transparent.
        if (Color.alpha(tintColor) == 0) {
            ColorUtils.setTintList(attributionsView, ContextCompat.getColor(getContext(), R.color.mapbox_blue));
        } else {
            ColorUtils.setTintList(attributionsView, tintColor);
        }
    }

    int getAttributionTintColor() {
        return mapboxMap.getUiSettings().getAttributionTintColor();
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

    MyLocationView getUserLocationView() {
        return myLocationView;
    }

    NativeMapView getNativeMapView() {
        return nativeMapView;
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

    //
    // View utility methods
    //

    private void setWidgetGravity(@NonNull final View view, int gravity) {
        LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
        layoutParams.gravity = gravity;
        view.setLayoutParams(layoutParams);
    }

    private void setWidgetMargins(@NonNull final View view, int left, int top, int right, int bottom) {
        LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
        left += contentPaddingLeft;
        top += contentPaddingTop;
        right += contentPaddingRight;
        bottom += contentPaddingBottom;
        layoutParams.setMargins(left, top, right, bottom);
        view.setLayoutParams(layoutParams);
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

                AlertDialog telemDialog = builder.show();
                telemDialog.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(mapView.getAttributionTintColor());
                telemDialog.getButton(AlertDialog.BUTTON_NEGATIVE).setTextColor(mapView.getAttributionTintColor());
                telemDialog.getButton(AlertDialog.BUTTON_NEUTRAL).setTextColor(mapView.getAttributionTintColor());
                return;
            }
            String url = context.getResources().getStringArray(R.array.mapbox_attribution_links)[which];
            if (which == ATTRIBUTION_INDEX_IMPROVE_THIS_MAP) {
                LatLng latLng = mapView.getMapboxMap().getCameraPosition().target;
                url = String.format(url, latLng.getLongitude(), latLng.getLatitude(), (int) mapView.getZoom());
            }
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(url));
            context.startActivity(intent);
        }
    }

    private static class ZoomInvalidator implements Runnable {

        private MapboxMap mapboxMap;

        ZoomInvalidator(MapboxMap mapboxMap) {
            this.mapboxMap = mapboxMap;
        }

        @Override
        public void run() {
            // invalidate camera position
            mapboxMap.getCameraPosition();
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
            DID_FINISH_LOADING_STYLE
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

}
