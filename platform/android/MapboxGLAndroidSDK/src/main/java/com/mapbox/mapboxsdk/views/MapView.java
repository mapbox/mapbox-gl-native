package com.mapbox.mapboxsdk.views;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.Dialog;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.FloatRange;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.ShoveGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MathConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.exceptions.IconBitmapChangedException;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.layers.CustomLayer;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.utils.MathUtils;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.TimeUnit;

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
 *
 * @see MapView#setAccessToken(String)
 */
public final class MapView extends FrameLayout {

    //
    // Static members
    //

    // Used for logging
    private static final String TAG = "MapView";

    // Used for animation
    public static final long ANIMATION_DURATION = 300;

    // Used for saving instance state
    private static final String STATE_CENTER_COORDINATE = "centerCoordinate";
    private static final String STATE_CENTER_DIRECTION = "centerDirection";
    private static final String STATE_ZOOM_LEVEL = "zoomLevel";
    private static final String STATE_TILT = "tilt";
    private static final String STATE_ZOOM_ENABLED = "zoomEnabled";
    private static final String STATE_SCROLL_ENABLED = "scrollEnabled";
    private static final String STATE_ROTATE_ENABLED = "rotateEnabled";
    private static final String STATE_TILT_ENABLED = "tiltEnabled";
    private static final String STATE_ZOOM_CONTROLS_ENABLED = "zoomControlsEnabled";
    private static final String STATE_DEBUG_ACTIVE = "debugActive";
    private static final String STATE_STYLE_URL = "styleUrl";
    private static final String STATE_ACCESS_TOKEN = "accessToken";
    private static final String STATE_STYLE_CLASSES = "styleClasses";
    private static final String STATE_DEFAULT_TRANSITION_DURATION = "defaultTransitionDuration";
    private static final String STATE_MY_LOCATION_ENABLED = "myLocationEnabled";
    private static final String STATE_MY_LOCATION_TRACKING_MODE = "myLocationTracking";
    private static final String STATE_COMPASS_ENABLED = "compassEnabled";
    private static final String STATE_COMPASS_GRAVITY = "compassGravity";
    private static final String STATE_COMPASS_MARGIN_LEFT = "compassMarginLeft";
    private static final String STATE_COMPASS_MARGIN_TOP = "compassMarginTop";
    private static final String STATE_COMPASS_MARGIN_RIGHT = "compassMarginRight";
    private static final String STATE_COMPASS_MARGIN_BOTTOM = "compassMarginBottom";
    private static final String STATE_LOGO_GRAVITY = "logoGravity";
    private static final String STATE_LOGO_MARGIN_LEFT = "logoMarginLeft";
    private static final String STATE_LOGO_MARGIN_TOP = "logoMarginTop";
    private static final String STATE_LOGO_MARGIN_RIGHT = "logoMarginRight";
    private static final String STATE_LOGO_MARGIN_BOTTOM = "logoMarginBottom";
    private static final String STATE_LOGO_VISIBILITY = "logoVisibility";
    private static final String STATE_ATTRIBUTION_GRAVITY = "attrGravity";
    private static final String STATE_ATTRIBUTION_MARGIN_LEFT = "attrMarginLeft";
    private static final String STATE_ATTRIBUTION_MARGIN_TOP = "attrMarginTop";
    private static final String STATE_ATTRIBUTION_MARGIN_RIGHT = "attrMarginRight";
    private static final String STATE_ATTRIBUTION_MARGIN_BOTTOM = "atrrMarginBottom";
    private static final String STATE_ATTRIBUTION_VISIBILITY = "atrrVisibility";

    // Used for positioning views
    private static final float DIMENSION_SEVEN_DP = 7f;
    private static final float DIMENSION_TEN_DP = 10f;
    private static final float DIMENSION_SIXTEEN_DP = 16f;
    private static final float DIMENSION_SEVENTYSIX_DP = 76f;

    // Used to select "Improve this map" link in the attribution dialog
    // Index into R.arrays.attribution_links
    private static final int ATTRIBUTION_INDEX_IMPROVE_THIS_MAP = 2;

    /**
     * The currently supported maximum zoom level.
     *
     * @see MapView#setZoomLevel(double)
     */
    public static final double MAXIMUM_ZOOM_LEVEL = 18.0;

    /**
     * The currently supported maximum and minimum tilt values.
     *
     * @see MapView#setTilt(Double, Long)
     */
    protected static final double MINIMUM_TILT = 0;
    protected static final double MAXIMUM_TILT = 60;

    //
    // Instance members
    //

    // Used to call JNI NativeMapView
    private NativeMapView mNativeMapView;

    // Used to track rendering
    private TextureView mTextureView;

    // Used to handle DPI scaling
    private float mScreenDensity = 1.0f;

    // Touch gesture detectors
    private GestureDetectorCompat mGestureDetector;
    private ScaleGestureDetector mScaleGestureDetector;
    private RotateGestureDetector mRotateGestureDetector;
    private ShoveGestureDetector mShoveGestureDetector;
    private boolean mTwoTap = false;
    private boolean mZoomStarted = false;
    private boolean mQuickZoom = false;

    // Shows zoom buttons
    private ZoomButtonsController mZoomButtonsController;
    private boolean mZoomControlsEnabled = false;

    // Used to track trackball long presses
    private TrackballLongPressTimeOut mCurrentTrackballLongPressTimeOut;

    // Receives changes to network connectivity
    private ConnectivityReceiver mConnectivityReceiver;

    // Used for user location
    private UserLocationView mUserLocationView;

    // Used for the compass
    private CompassView mCompassView;

    // Used for displaying annotations
    // Every annotation that has been added to the map
    private final List<Annotation> mAnnotations = new ArrayList<>();
    private List<Marker> mMarkersNearLastTap = new ArrayList<>();
    private List<Marker> mSelectedMarkers = new ArrayList<>();
    private List<InfoWindow> mInfoWindows = new ArrayList<>();
    private MapboxMap.InfoWindowAdapter mInfoWindowAdapter;
    private List<Icon> mIcons = new ArrayList<>();

    // Used for the Mapbox Logo
    private ImageView mLogoView;

    // Used for attributions control
    private ImageView mAttributionsView;

    // Used to manage MapChange event listeners
    private List<MapboxMap.OnMapChangedListener> mOnMapChangedListener = new ArrayList<>();

    // Used to manage map click event listeners
    private MapboxMap.OnMapClickListener mOnMapClickListener;
    private MapboxMap.OnMapLongClickListener mOnMapLongClickListener;

    // Used to manage fling and scroll event listeners
    private MapboxMap.OnFlingListener mOnFlingListener;
    private MapboxMap.OnScrollListener mOnScrollListener;

    // Used to manage marker click event listeners
    private MapboxMap.OnMarkerClickListener mOnMarkerClickListener;
    private MapboxMap.OnInfoWindowClickListener mOnInfoWindowClickListener;

    // Used to manage FPS change event listeners
    private MapboxMap.OnFpsChangedListener mOnFpsChangedListener;

    // Used to manage tracking mode changes
    private MapboxMap.OnMyLocationTrackingModeChangeListener mOnMyLocationTrackingModeChangeListener;
    private MapboxMap.OnMyBearingTrackingModeChangeListener mOnMyBearingTrackingModeChangeListener;

    //
    // Properties
    //

    // These are properties with setters/getters, saved in onSaveInstanceState and XML attributes
    private boolean mZoomEnabled = true;
    private boolean mScrollEnabled = true;
    private boolean mRotateEnabled = true;
    private boolean mTiltEnabled = true;
    private boolean mAllowConcurrentMultipleOpenInfoWindows = false;
    private String mStyleUrl;

    //
    // Constructors
    //

    /**
     * Simple constructor to use when creating a {@link MapView} from code using the default map style.
     *
     * @param context     The {@link Context} of the {@link android.app.Activity}
     *                    or {@link android.app.Fragment} the {@link MapView} is running in.
     * @param accessToken Your public Mapbox access token. Used to load map styles and tiles.
     */
    @UiThread
    public MapView(@NonNull Context context, @NonNull String accessToken) {
        super(context);
        if (accessToken == null) {
            Log.w(TAG, "accessToken was null, so just returning");
            return;
        }
        initialize(context, null);
        setAccessToken(accessToken);
        setStyleUrl(null);
    }

    /**
     * Simple constructor to use when creating a {@link MapView} from code using the provided map style URL.
     *
     * @param context     The {@link Context} of the {@link android.app.Activity}
     *                    or {@link android.app.Fragment} the {@link MapView} is running in.
     * @param accessToken Your public Mapbox access token. Used to load map styles and tiles.
     * @param styleUrl    A URL to the map style initially displayed. See {@link MapView#setStyleUrl(String)} for possible values.
     * @see MapView#setStyleUrl(String)
     */
    @UiThread
    public MapView(@NonNull Context context, @NonNull String accessToken, @NonNull String styleUrl) {
        super(context);
        if (accessToken == null) {
            Log.w(TAG, "accessToken was null, so just returning");
            return;
        }
        if (styleUrl == null) {
            Log.w(TAG, "styleUrl was null, so just returning");
            return;
        }
        initialize(context, null);
        setAccessToken(accessToken);
        setStyleUrl(styleUrl);
    }

    // Constructor that is called when inflating a view from XML.

    /**
     * Do not call from code.
     */
    @UiThread
    public MapView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initialize(context, attrs);
    }

    // Constructor that is called when inflating a view from XML.

    /**
     * Do not call from code.
     */
    @UiThread
    public MapView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context, attrs);
    }

    //
    // Initialization
    //

    // Common initialization code goes here
    private void initialize(Context context, AttributeSet attrs) {
        if (context == null) {
            Log.w(TAG, "context was null, so just returning");
            return;
        }

        // Inflate content
        View view = LayoutInflater.from(context).inflate(R.layout.mapview_internal, this);

        if (!isInEditMode()) {
            setWillNotDraw(false);
        }

        // Reference the TextureView
        mTextureView = (TextureView) view.findViewById(R.id.textureView);
        mTextureView.setSurfaceTextureListener(new SurfaceTextureListener());

        // Check if we are in Android Studio UI editor to avoid error in layout preview
        if (isInEditMode()) {
            return;
        }

        // Get the screen's density
        mScreenDensity = context.getResources().getDisplayMetrics().density;

        // Get the cache path
        String cachePath = context.getCacheDir().getAbsolutePath();
        String dataPath = context.getFilesDir().getAbsolutePath();
        String apkPath = context.getPackageCodePath();

        // Create the NativeMapView
        int availableProcessors = Runtime.getRuntime().availableProcessors();
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        ActivityManager activityManager = (ActivityManager) context
                .getSystemService(Context.ACTIVITY_SERVICE);
        activityManager.getMemoryInfo(memoryInfo);
        long maxMemory = memoryInfo.availMem;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            maxMemory = memoryInfo.totalMem;
        }
        mNativeMapView = new
                NativeMapView(this, cachePath, dataPath, apkPath, mScreenDensity, availableProcessors, maxMemory);

        // Ensure this view is interactable
        setClickable(true);
        setLongClickable(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();

        // Touch gesture detectors
        mGestureDetector = new GestureDetectorCompat(context, new GestureListener());
        mGestureDetector.setIsLongpressEnabled(true);
        mScaleGestureDetector = new ScaleGestureDetector(context, new ScaleGestureListener());
        ScaleGestureDetectorCompat.setQuickScaleEnabled(mScaleGestureDetector, true);
        mRotateGestureDetector = new RotateGestureDetector(context, new RotateGestureListener());
        mShoveGestureDetector = new ShoveGestureDetector(context, new ShoveGestureListener());

        // Shows the zoom controls
        if (!context.getPackageManager()
                .hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
            mZoomControlsEnabled = true;
        }
        mZoomButtonsController = new ZoomButtonsController(this);
        mZoomButtonsController.setZoomSpeed(ANIMATION_DURATION);
        mZoomButtonsController.setOnZoomListener(new OnZoomListener());

        // Check current connection status
        ConnectivityManager connectivityManager = (ConnectivityManager) context
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
        boolean isConnected = (activeNetwork != null) && activeNetwork.isConnectedOrConnecting();
        onConnectivityChanged(isConnected);

        // Setup user location UI
        mUserLocationView = (UserLocationView) view.findViewById(R.id.userLocationView);
        mUserLocationView.setMapView(this);

        // Setup compass
        mCompassView = (CompassView) view.findViewById(R.id.compassView);
        mCompassView.setOnClickListener(new CompassView.CompassClickListener(this));

        // Setup Mapbox logo
        mLogoView = (ImageView) view.findViewById(R.id.logoView);

        // Setup Attributions control
        mAttributionsView = (ImageView) view.findViewById(R.id.attributionView);
        mAttributionsView.setOnClickListener(new AttributionOnClickListener(this));

        // Load the attributes
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.MapView, 0, 0);
        try {
            double centerLatitude = typedArray.getFloat(R.styleable.MapView_center_latitude, 0.0f);
            double centerLongitude = typedArray.getFloat(R.styleable.MapView_center_longitude, 0.0f);
            LatLng centerCoordinate = new LatLng(centerLatitude, centerLongitude);
            setCenterCoordinate(centerCoordinate, false);
            // need to set zoom level first because of limitation on rotating when zoomed out
            setZoomLevel(typedArray.getFloat(R.styleable.MapView_zoom_level, 0.0f));
            setDirection(typedArray.getFloat(R.styleable.MapView_direction, 0.0f));
            setZoomEnabled(typedArray.getBoolean(R.styleable.MapView_zoom_enabled, true));
            setScrollEnabled(typedArray.getBoolean(R.styleable.MapView_scroll_enabled, true));
            setRotateEnabled(typedArray.getBoolean(R.styleable.MapView_rotate_enabled, true));
            setTiltEnabled(typedArray.getBoolean(R.styleable.MapView_tilt_enabled, true));
            setZoomControlsEnabled(typedArray.getBoolean(R.styleable.MapView_zoom_controls_enabled, isZoomControlsEnabled()));
            setDebugActive(typedArray.getBoolean(R.styleable.MapView_debug_active, false));
            if (typedArray.getString(R.styleable.MapView_style_url) != null) {
                setStyleUrl(typedArray.getString(R.styleable.MapView_style_url));
            }
            if (typedArray.getString(R.styleable.MapView_access_token) != null) {
                setAccessToken(typedArray.getString(R.styleable.MapView_access_token));
            }
            if (typedArray.getString(R.styleable.MapView_style_classes) != null) {
                List<String> styleClasses = Arrays.asList(typedArray
                        .getString(R.styleable.MapView_style_classes).split("\\s*,\\s*"));
                for (String styleClass : styleClasses) {
                    if (styleClass.length() == 0) {
                        styleClasses.remove(styleClass);
                    }
                }
                setStyleClasses(styleClasses,0);
            }

            // Compass
            setCompassEnabled(typedArray.getBoolean(R.styleable.MapView_compass_enabled, true));
            setCompassGravity(typedArray.getInt(R.styleable.MapView_compass_gravity, Gravity.TOP | Gravity.END));
            setWidgetMargins(mCompassView, typedArray.getDimension(R.styleable.MapView_compass_margin_left, DIMENSION_TEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_compass_margin_top, DIMENSION_TEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_compass_margin_right, DIMENSION_TEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_compass_margin_bottom, DIMENSION_TEN_DP));

            // Logo
            setLogoVisibility(typedArray.getInt(R.styleable.MapView_logo_visibility, View.VISIBLE));
            setLogoGravity(typedArray.getInt(R.styleable.MapView_logo_gravity, Gravity.BOTTOM | Gravity.START));
            setWidgetMargins(mLogoView, typedArray.getDimension(R.styleable.MapView_logo_margin_left, DIMENSION_SIXTEEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_logo_margin_top, DIMENSION_SIXTEEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_logo_margin_right, DIMENSION_SIXTEEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_logo_margin_bottom, DIMENSION_SIXTEEN_DP));

            // Attribution
            setAttributionVisibility(typedArray.getInt(R.styleable.MapView_attribution_visibility, View.VISIBLE));
            setAttributionGravity(typedArray.getInt(R.styleable.MapView_attribution_gravity, Gravity.BOTTOM));
            setWidgetMargins(mAttributionsView, typedArray.getDimension(R.styleable.MapView_attribution_margin_left, DIMENSION_SEVENTYSIX_DP)
                    , typedArray.getDimension(R.styleable.MapView_attribution_margin_top, DIMENSION_SEVEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_attribution_margin_right, DIMENSION_SEVEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_attribution_margin_bottom, DIMENSION_SEVEN_DP));

            // User location
            try {
                setMyLocationEnabled(typedArray.getBoolean(R.styleable.MapView_my_location_enabled, false));
            } catch (SecurityException ignore) {
                // User did not accept location permissions
            }

        } finally {
            typedArray.recycle();
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
        if (savedInstanceState != null) {
            setCenterCoordinate((LatLng) savedInstanceState.getParcelable(STATE_CENTER_COORDINATE), false);
            // need to set zoom level first because of limitation on rotating when zoomed out
            setZoomLevel(savedInstanceState.getDouble(STATE_ZOOM_LEVEL));
            setDirection(savedInstanceState.getDouble(STATE_CENTER_DIRECTION));
            setTilt(savedInstanceState.getDouble(STATE_TILT), null);
            setZoomEnabled(savedInstanceState.getBoolean(STATE_ZOOM_ENABLED));
            setScrollEnabled(savedInstanceState.getBoolean(STATE_SCROLL_ENABLED));
            setRotateEnabled(savedInstanceState.getBoolean(STATE_ROTATE_ENABLED));
            setTiltEnabled(savedInstanceState.getBoolean(STATE_TILT_ENABLED));
            setZoomControlsEnabled(savedInstanceState.getBoolean(STATE_ZOOM_CONTROLS_ENABLED));
            setDebugActive(savedInstanceState.getBoolean(STATE_DEBUG_ACTIVE));
            setStyleUrl(savedInstanceState.getString(STATE_STYLE_URL));
            setAccessToken(savedInstanceState.getString(STATE_ACCESS_TOKEN));
            List<String> appliedStyleClasses = savedInstanceState.getStringArrayList(STATE_STYLE_CLASSES);
            if (!appliedStyleClasses.isEmpty()) {
                setStyleClasses(appliedStyleClasses,0);
            }
            mNativeMapView.setDefaultTransitionDuration(
                    savedInstanceState.getLong(STATE_DEFAULT_TRANSITION_DURATION));

            // User location
            try {
                setMyLocationEnabled(savedInstanceState.getBoolean(STATE_MY_LOCATION_ENABLED));
            } catch (SecurityException ignore) {
                // User did not accept location permissions
            }

            // Compass
            setCompassEnabled(savedInstanceState.getBoolean(STATE_COMPASS_ENABLED));
            setCompassGravity(savedInstanceState.getInt(STATE_COMPASS_GRAVITY));
            setCompassMargins(savedInstanceState.getInt(STATE_COMPASS_MARGIN_LEFT)
                    , savedInstanceState.getInt(STATE_COMPASS_MARGIN_TOP)
                    , savedInstanceState.getInt(STATE_COMPASS_MARGIN_RIGHT)
                    , savedInstanceState.getInt(STATE_COMPASS_MARGIN_BOTTOM));

            // Logo
            setLogoVisibility(savedInstanceState.getInt(STATE_LOGO_VISIBILITY));
            setLogoGravity(savedInstanceState.getInt(STATE_LOGO_GRAVITY));
            setLogoMargins(savedInstanceState.getInt(STATE_LOGO_MARGIN_LEFT)
                    , savedInstanceState.getInt(STATE_LOGO_MARGIN_TOP)
                    , savedInstanceState.getInt(STATE_LOGO_MARGIN_RIGHT)
                    , savedInstanceState.getInt(STATE_LOGO_MARGIN_BOTTOM));

            // Attribution
            setAttributionVisibility(savedInstanceState.getInt(STATE_ATTRIBUTION_VISIBILITY));
            setAttributionGravity(savedInstanceState.getInt(STATE_ATTRIBUTION_GRAVITY));
            setAttributionMargins(savedInstanceState.getInt(STATE_ATTRIBUTION_MARGIN_LEFT)
                    , savedInstanceState.getInt(STATE_ATTRIBUTION_MARGIN_TOP)
                    , savedInstanceState.getInt(STATE_ATTRIBUTION_MARGIN_RIGHT)
                    , savedInstanceState.getInt(STATE_ATTRIBUTION_MARGIN_BOTTOM));

            //noinspection ResourceType
            setMyLocationTrackingMode(savedInstanceState.getInt(STATE_MY_LOCATION_TRACKING_MODE, MyLocationTracking.TRACKING_NONE));
        }

        // Force a check for an access token
        validateAccessToken(getAccessToken());

        // Initialize EGL
        mNativeMapView.initializeDisplay();
        mNativeMapView.initializeContext();

        // Add annotation deselection listener
        addOnMapChangedListener(new MapboxMap.OnMapChangedListener() {
            @Override
            public void onMapChanged(@MapboxMap.MapChange int change) {
                if (change == MapboxMap.DID_FINISH_LOADING_MAP) {
                    reloadIcons();
                    reloadMarkers();
                    adjustTopOffsetPixels();
                }
            }
        });
    }

    /**
     * You must call this method from the parent's {@link android.app.Activity#onSaveInstanceState(Bundle)}
     * or {@link android.app.Fragment#onSaveInstanceState(Bundle)}.
     *
     * @param outState Pass in the parent's outState.
     */
    @UiThread
    public void onSaveInstanceState(@NonNull Bundle outState) {
        if (outState == null) {
            Log.w(TAG, "outState was null, so just returning");
            return;
        }

        outState.putParcelable(STATE_CENTER_COORDINATE, getCenterCoordinate());
        // need to set zoom level first because of limitation on rotating when zoomed out
        outState.putDouble(STATE_ZOOM_LEVEL, getZoomLevel());
        outState.putDouble(STATE_CENTER_DIRECTION, getDirection());
        outState.putDouble(STATE_TILT, getTilt());
        outState.putBoolean(STATE_ZOOM_ENABLED, isZoomEnabled());
        outState.putBoolean(STATE_SCROLL_ENABLED, isScrollEnabled());
        outState.putBoolean(STATE_ROTATE_ENABLED, isRotateEnabled());
        outState.putBoolean(STATE_TILT_ENABLED, isTiltEnabled());
        outState.putBoolean(STATE_ZOOM_CONTROLS_ENABLED, isZoomControlsEnabled());
        outState.putBoolean(STATE_DEBUG_ACTIVE, isDebugActive());
        outState.putString(STATE_STYLE_URL, getStyleUrl());
        outState.putString(STATE_ACCESS_TOKEN, getAccessToken());
        outState.putStringArrayList(STATE_STYLE_CLASSES, new ArrayList<>(getStyleClasses()));
        outState.putLong(STATE_DEFAULT_TRANSITION_DURATION, mNativeMapView.getDefaultTransitionDuration());
        outState.putBoolean(STATE_MY_LOCATION_ENABLED, isMyLocationEnabled());
        outState.putInt(STATE_MY_LOCATION_TRACKING_MODE, mUserLocationView.getMyLocationTrackingMode());

        // Compass
        LayoutParams compassParams = (LayoutParams) mCompassView.getLayoutParams();
        outState.putBoolean(STATE_COMPASS_ENABLED, isCompassEnabled());
        outState.putInt(STATE_COMPASS_GRAVITY, compassParams.gravity);
        outState.putInt(STATE_COMPASS_MARGIN_LEFT, compassParams.leftMargin);
        outState.putInt(STATE_COMPASS_MARGIN_TOP, compassParams.topMargin);
        outState.putInt(STATE_COMPASS_MARGIN_BOTTOM, compassParams.bottomMargin);
        outState.putInt(STATE_COMPASS_MARGIN_RIGHT, compassParams.rightMargin);

        // Logo
        LayoutParams logoParams = (LayoutParams) mLogoView.getLayoutParams();
        outState.putInt(STATE_LOGO_GRAVITY, logoParams.gravity);
        outState.putInt(STATE_LOGO_MARGIN_LEFT, logoParams.leftMargin);
        outState.putInt(STATE_LOGO_MARGIN_TOP, logoParams.topMargin);
        outState.putInt(STATE_LOGO_MARGIN_RIGHT, logoParams.rightMargin);
        outState.putInt(STATE_LOGO_MARGIN_BOTTOM, logoParams.bottomMargin);
        outState.putInt(STATE_LOGO_VISIBILITY, mLogoView.getVisibility());

        // Attribution
        LayoutParams attrParams = (LayoutParams) mAttributionsView.getLayoutParams();
        outState.putInt(STATE_ATTRIBUTION_GRAVITY, attrParams.gravity);
        outState.putInt(STATE_ATTRIBUTION_MARGIN_LEFT, attrParams.leftMargin);
        outState.putInt(STATE_ATTRIBUTION_MARGIN_TOP, attrParams.topMargin);
        outState.putInt(STATE_ATTRIBUTION_MARGIN_RIGHT, attrParams.rightMargin);
        outState.putInt(STATE_ATTRIBUTION_MARGIN_BOTTOM, attrParams.bottomMargin);
        outState.putInt(STATE_ATTRIBUTION_VISIBILITY, mAttributionsView.getVisibility());
    }

    /**
     * You must call this method from the parent's {@link Activity#onDestroy()} or {@link Fragment#onDestroy()}.
     */
    @UiThread
    public void onDestroy() {
        mNativeMapView.terminateContext();
        mNativeMapView.terminateDisplay();
        mNativeMapView.destroySurface();
        mNativeMapView.destroy();
        mNativeMapView = null;
    }

    /**
     * You must call this method from the parent's {@link Activity#onStart()} or {@link Fragment#onStart()}.
     */
    @UiThread
    public void onStart() {
        mUserLocationView.onStart();
    }

    /**
     * You must call this method from the parent's {@link Activity#onStop()} or {@link Fragment#onStop()}
     */
    @UiThread
    public void onStop() {
        mUserLocationView.onStop();
    }

    /**
     * You must call this method from the parent's {@link Activity#onPause()} or {@link Fragment#onPause()}.
     */
    @UiThread
    public void onPause() {
        // Register for connectivity changes
        getContext().unregisterReceiver(mConnectivityReceiver);
        mConnectivityReceiver = null;

        mUserLocationView.pause();
        mNativeMapView.pause();
    }

    /**
     * You must call this method from the parent's {@link Activity#onResume()} or {@link Fragment#onResume()}.
     */
    @UiThread
    public void onResume() {
        // Register for connectivity changes
        mConnectivityReceiver = new ConnectivityReceiver();
        getContext().registerReceiver(mConnectivityReceiver, new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));

        mNativeMapView.resume();
        mNativeMapView.update();
        mUserLocationView.resume();
    }

    /**
     * You must call this method from the parent's {@link Activity#onLowMemory()} or {@link Fragment#onLowMemory()}.
     */
    @UiThread
    public void onLowMemory() {
        mNativeMapView.onLowMemory();
    }

    //
    // Position
    //

    @UiThread
    @NonNull
    LatLng getCenterCoordinate() {
        return mNativeMapView.getLatLng();
    }

    @UiThread
    void setCenterCoordinate(@NonNull LatLng centerCoordinate, boolean animated) {
        if (centerCoordinate == null) {
            Log.w(TAG, "centerCoordinate was null, so just returning");
            return;
        }

        if (animated) {
            CameraPosition cameraPosition = new CameraPosition.Builder(getCameraPosition())
                    .target(centerCoordinate)
                    .build();
            animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition),
                    (int) ANIMATION_DURATION, null);
        } else {
            jumpTo(mNativeMapView.getBearing(), centerCoordinate, mNativeMapView.getPitch(), mNativeMapView.getZoom());
        }
    }

    @UiThread
    void resetPosition() {
        mNativeMapView.cancelTransitions();
        mNativeMapView.resetPosition();
    }

    @UiThread
    void setCenterCoordinate(@NonNull LatLngZoom centerCoordinate, boolean animated) {
        if (centerCoordinate == null) {
            Log.w(TAG, "centerCoordinate was null, so just returning");
            return;
        }
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        mNativeMapView.setLatLngZoom(centerCoordinate, duration);
    }

    @UiThread
    boolean isScrollEnabled() {
        return mScrollEnabled;
    }

    @UiThread
    void setScrollEnabled(boolean scrollEnabled) {
        this.mScrollEnabled = scrollEnabled;
    }

    //
    // Pitch / Tilt
    //

    double getTilt() {
        return mNativeMapView.getPitch();
    }

    @FloatRange(from = MINIMUM_TILT, to = MAXIMUM_TILT)
    void setTilt(Double pitch, @Nullable Long duration) {
        long actualDuration = 0;
        if (duration != null) {
            actualDuration = duration;
        }
        mNativeMapView.setPitch(pitch, actualDuration);
    }

    //
    // Rotation
    //

    @UiThread
    @FloatRange(from = 0, to = 360)
    double getDirection() {
        double direction = -mNativeMapView.getBearing();

        while (direction > 360) {
            direction -= 360;
        }
        while (direction < 0) {
            direction += 360;
        }

        return direction;
    }

    @UiThread
    void setDirection(@FloatRange(from = 0, to = 360) double direction) {
        setDirection(direction, false);
    }

    @UiThread
    void setDirection(@FloatRange(from = 0, to = 360) double direction, boolean animated) {
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        // Out of range direactions are normallised in setBearing
        mNativeMapView.setBearing(-direction, duration);
    }

    @UiThread
    void resetNorth() {
        mNativeMapView.cancelTransitions();
        mNativeMapView.resetNorth();
    }

    @UiThread
    boolean isRotateEnabled() {
        return mRotateEnabled;
    }

    @UiThread
    void setRotateEnabled(boolean rotateEnabled) {
        this.mRotateEnabled = rotateEnabled;
    }

    //
    // Scale
    //

    @UiThread
    @FloatRange(from = 0.0, to = MAXIMUM_ZOOM_LEVEL)
    double getZoomLevel() {
        return mNativeMapView.getZoom();
    }

    @UiThread
    void setZoomLevel(@FloatRange(from = 0.0, to = MAXIMUM_ZOOM_LEVEL) double zoomLevel) {
        setZoomLevel(zoomLevel, false);
    }

    @UiThread
    void setZoomLevel(@FloatRange(from = 0.0, to = MAXIMUM_ZOOM_LEVEL) double zoomLevel, boolean animated) {
        if ((zoomLevel < 0.0) || (zoomLevel > MAXIMUM_ZOOM_LEVEL)) {
            throw new IllegalArgumentException("zoomLevel is < 0 or > MapView.MAXIMUM_ZOOM_LEVEL");
        }
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        mNativeMapView.setZoom(zoomLevel, duration);
    }

    @UiThread
    boolean isZoomEnabled() {
        return mZoomEnabled;
    }

    @UiThread
    void setZoomEnabled(boolean zoomEnabled) {
        this.mZoomEnabled = zoomEnabled;

        if (mZoomControlsEnabled && (getVisibility() == View.VISIBLE) && mZoomEnabled) {
            mZoomButtonsController.setVisible(true);
        } else {
            mZoomButtonsController.setVisible(false);
        }
    }

    boolean isZoomControlsEnabled() {
        return mZoomControlsEnabled;
    }

    void setZoomControlsEnabled(boolean enabled) {
        mZoomControlsEnabled = enabled;

        if (mZoomControlsEnabled && (getVisibility() == View.VISIBLE) && mZoomEnabled) {
            mZoomButtonsController.setVisible(true);
        } else {
            mZoomButtonsController.setVisible(false);
        }
    }

    // Zoom in or out
    private void zoom(boolean zoomIn) {
        zoom(zoomIn, -1.0f, -1.0f);
    }

    private void zoom(boolean zoomIn, float x, float y) {
        // Cancel any animation
        mNativeMapView.cancelTransitions();

        if (zoomIn) {
            mNativeMapView.scaleBy(2.0, x / mScreenDensity, y / mScreenDensity, ANIMATION_DURATION);
        } else {
            mNativeMapView.scaleBy(0.5, x / mScreenDensity, y / mScreenDensity, ANIMATION_DURATION);
        }
    }

    //
    // Tilt
    //

    @UiThread
    boolean isTiltEnabled() {
        return mTiltEnabled;
    }

    @UiThread
    void setTiltEnabled(boolean tiltEnabled) {
        this.mTiltEnabled = tiltEnabled;
    }


    //
    // Camera API
    //

    final CameraPosition getCameraPosition() {
        return new CameraPosition(getCenterCoordinate(), (float) getZoomLevel(), (float) getTilt(), (float) getBearing());
    }

    @UiThread
    final void animateCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
        if (update.getTarget() == null) {
            Log.w(TAG, "animateCamera with null target coordinate passed in.  Will immediately return without animating camera.");
            return;
        }

        mNativeMapView.cancelTransitions();

        // Register callbacks early enough
        if (callback != null) {
            final MapView view = this;
            addOnMapChangedListener(new MapboxMap.OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapboxMap.MapChange int change) {
                    if (change == MapboxMap.REGION_DID_CHANGE_ANIMATED) {
                        callback.onFinish();

                        // Clean up after self
                        removeOnMapChangedListener(this);
                    }
                }
            });
        }

        // Convert Degrees To Radians
        double angle = -1;
        if (update.getBearing() >= 0) {
            angle = (-update.getBearing()) * MathConstants.DEG2RAD;
        }
        double pitch = -1;
        if (update.getTilt() >= 0) {
            double dp = MathUtils.clamp(update.getTilt(), MINIMUM_TILT, MAXIMUM_TILT);
            pitch = dp * MathConstants.DEG2RAD;
        }
        double zoom = -1;
        if (update.getZoom() >= 0) {
            zoom = update.getZoom();
        }

        long durationNano = 0;
        if (durationMs > 0) {
            durationNano = TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS);
        }

        flyTo(angle, update.getTarget(), durationNano, pitch, zoom);
    }

    @UiThread
    final void easeCamera(CameraUpdate update, int durationMs, final MapboxMap.CancelableCallback callback) {
        if (update.getTarget() == null) {
            Log.w(TAG, "easeCamera with null target coordinate passed in.  Will immediately return without easing camera.");
            return;
        }

        mNativeMapView.cancelTransitions();

        // Register callbacks early enough
        if (callback != null) {
            final MapView view = this;
            addOnMapChangedListener(new MapboxMap.OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapboxMap.MapChange int change) {
                    if (change == MapboxMap.REGION_DID_CHANGE_ANIMATED) {
                        callback.onFinish();

                        // Clean up after self
                        removeOnMapChangedListener(this);
                    }
                }
            });
        }

        // Convert Degrees To Radians
        double angle = -1;
        if (update.getBearing() >= 0) {
            angle = (-update.getBearing()) * MathConstants.DEG2RAD;
        }
        double pitch = -1;
        if (update.getTilt() >= 0) {
            double dp = MathUtils.clamp(update.getTilt(), MINIMUM_TILT, MAXIMUM_TILT);
            pitch = dp * MathConstants.DEG2RAD;
        }
        double zoom = -1;
        if (update.getZoom() >= 0) {
            zoom = update.getZoom();
        }

        long durationNano = 0;
        if (durationMs > 0) {
            durationNano = TimeUnit.NANOSECONDS.convert(durationMs, TimeUnit.MILLISECONDS);
        }

        easeTo(angle, update.getTarget(), durationNano, pitch, zoom);
    }

    @UiThread
    final void moveCamera(CameraUpdate update) {
        if (update.getTarget() == null) {
            Log.w(TAG, "moveCamera with null target coordinate passed in.  Will immediately return without moving camera.");
            return;
        }

        mNativeMapView.cancelTransitions();

        // Convert Degrees To Radians
        double angle = -1;
        if (update.getBearing() >= 0) {
            angle = (-update.getBearing()) * MathConstants.DEG2RAD;
        }
        double pitch = -1;
        if (update.getTilt() >= 0) {
            double dp = MathUtils.clamp(update.getTilt(), MINIMUM_TILT, MAXIMUM_TILT);
            pitch = dp * MathConstants.DEG2RAD;
        }
        double zoom = -1;
        if (update.getZoom() >= 0) {
            zoom = update.getZoom();
        }

        jumpTo(angle, update.getTarget(), pitch, zoom);
    }

    //
    // InfoWindows
    //

    @UiThread
    void setAllowConcurrentMultipleOpenInfoWindows(boolean allow) {
        this.mAllowConcurrentMultipleOpenInfoWindows = allow;
    }

    @UiThread
    boolean isAllowConcurrentMultipleOpenInfoWindows() {
        return this.mAllowConcurrentMultipleOpenInfoWindows;
    }

    //
    // Debug
    //

    @UiThread
    boolean isDebugActive() {
        return mNativeMapView.getDebug();
    }

    @UiThread
    void setDebugActive(boolean debugActive) {
        mNativeMapView.setDebug(debugActive);
    }

    @UiThread
    void cycleDebugOptions() {
        mNativeMapView.cycleDebugOptions();
    }

    // True if map has finished loading the view
    private boolean isFullyLoaded() {
        return mNativeMapView.isFullyLoaded();
    }

    //
    // Styling
    //

    @UiThread
    void setStyleUrl(@Nullable String url) {
        if (url == null) {
            url = Style.MAPBOX_STREETS;
        }
        mStyleUrl = url;
        mNativeMapView.setStyleUrl(url);
    }

    @UiThread
    @NonNull
    String getStyleUrl() {
        return mStyleUrl;
    }

    @UiThread
    @NonNull
    List<String> getStyleClasses() {
        return Collections.unmodifiableList(mNativeMapView.getClasses());
    }

    @UiThread
    void setStyleClasses(@NonNull List<String> styleClasses, @IntRange(from = 0) long transitionDuration) {
        if (styleClasses == null) {
            Log.w(TAG, "styleClasses was null, so just returning");
            return;
        }
        if (transitionDuration < 0) {
            throw new IllegalArgumentException("transitionDuration is < 0");
        }
        // TODO non negative check and annotation (go back and check other functions too)
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        mNativeMapView.setClasses(styleClasses);
    }

    @UiThread
    void addStyleClass(@NonNull String styleClass) {
        if (styleClass == null) {
            Log.w(TAG, "styleClass was null, so just returning");
            return;
        }
        mNativeMapView.addClass(styleClass);
    }

    @UiThread
    void removeStyleClass(@NonNull String styleClass) {
        if (styleClass == null) {
            Log.w(TAG, "styleClass was null, so just returning");
            return;
        }
        mNativeMapView.removeClass(styleClass);
    }

    @UiThread
    boolean hasStyleClass(@NonNull String styleClass) {
        if (styleClass == null) {
            Log.w(TAG, "centerCoordinate was null, so just returning false");
            return false;
        }
        return mNativeMapView.hasClass(styleClass);
    }

    @UiThread
    void removeAllStyleClasses() {
        removeAllStyleClasses(0);
    }

    @UiThread
    void removeAllStyleClasses(@IntRange(from = 0) long transitionDuration) {
        if (transitionDuration < 0) {
            throw new IllegalArgumentException("transitionDuration is < 0");
        }
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        ArrayList<String> styleClasses = new ArrayList<>(0);
        setStyleClasses(styleClasses, 0);
    }

    //
    // Access token
    //

    // Checks if the given token is valid
    private void validateAccessToken(String accessToken) {
        if (TextUtils.isEmpty(accessToken) || (!accessToken.startsWith("pk.") && !accessToken.startsWith("sk."))) {
            throw new InvalidAccessTokenException();
        }
    }

    /**
     * <p>
     * Sets the current Mapbox access token used to load map styles and tiles.
     * </p>
     * <p>
     * You must set a valid access token before you call {@link MapView#onCreate(Bundle)}
     * or an exception will be thrown.
     * </p>
     * You can use {@link ApiAccess#getToken(Context)} to load an access token from your
     * application's manifest.
     *
     * @param accessToken Your public Mapbox access token.
     * @see MapView#onCreate(Bundle)
     * @see ApiAccess#getToken(Context)
     */
    @UiThread
    public void setAccessToken(@NonNull String accessToken) {
        // validateAccessToken does the null check
        if (!TextUtils.isEmpty(accessToken)) {
            accessToken = accessToken.trim();
        }
        validateAccessToken(accessToken);
        mNativeMapView.setAccessToken(accessToken);
    }

    /**
     * Returns the current Mapbox access token used to load map styles and tiles.
     *
     * @return The current Mapbox access token.
     */
    @UiThread
    @Nullable
    String getAccessToken() {
        return mNativeMapView.getAccessToken();
    }

    //
    // Projection
    //

    @UiThread
    @NonNull
    LatLng fromScreenLocation(@NonNull PointF point) {
        if (point == null) {
            Log.w(TAG, "point was null, so just returning (0, 0)");
            return new LatLng();
        }

        float x = point.x;
        float y = point.y;

        // flip y direction vertically to match core GL
        y = getHeight() - y;

        return mNativeMapView.latLngForPixel(new PointF(x / mScreenDensity, y / mScreenDensity));
    }

    @UiThread
    @NonNull
    public PointF toScreenLocation(@NonNull LatLng location) {
        if (location == null) {
            Log.w(TAG, "location was null, so just returning (0, 0)");
            return new PointF();
        }

        PointF point = mNativeMapView.pixelForLatLng(location);

        float x = point.x * mScreenDensity;
        float y = point.y * mScreenDensity;

        // flip y direction vertically to match core GL
        y = getHeight() - y;

        return new PointF(x, y);
    }

    //
    // Annotations
    //

    IconFactory getIconFactory() {
        return IconFactory.getInstance(getContext());
    }

    void loadIcon(Icon icon) {
        Bitmap bitmap = icon.getBitmap();
        String id = icon.getId();
        if (bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
            bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, false);
        }
        ByteBuffer buffer = ByteBuffer.allocate(bitmap.getRowBytes() * bitmap.getHeight());
        bitmap.copyPixelsToBuffer(buffer);

        float density = bitmap.getDensity();
        if (density == Bitmap.DENSITY_NONE) {
            density = DisplayMetrics.DENSITY_DEFAULT;
        }
        float scale = density / DisplayMetrics.DENSITY_DEFAULT;

        mNativeMapView.addAnnotationIcon(
                id,
                (int) (bitmap.getWidth() / scale),
                (int) (bitmap.getHeight() / scale),
                scale, buffer.array());
    }

    void reloadIcons() {
        int count = mIcons.size();
        for (int i = 0; i < count; i++) {
            Icon icon = mIcons.get(i);
            loadIcon(icon);
        }
    }

    Marker prepareMarker(MarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = marker.getIcon();
        if (icon == null) {
            icon = getIconFactory().defaultMarker();
            marker.setIcon(icon);
        }
        if (!mIcons.contains(icon)) {
            mIcons.add(icon);
            loadIcon(icon);
        } else {
            Icon oldIcon = mIcons.get(mIcons.indexOf(icon));
            if (!oldIcon.getBitmap().sameAs(icon.getBitmap())) {
                throw new IconBitmapChangedException();
            }
        }
        marker.setTopOffsetPixels(getTopOffsetPixelsForIcon(icon));
        return marker;
    }

    @UiThread
    @NonNull
    Marker addMarker(@NonNull MarkerOptions markerOptions) {
        if (markerOptions == null) {
            Log.w(TAG, "markerOptions was null, so just returning null");
            return null;
        }

        Marker marker = prepareMarker(markerOptions);
        long id = mNativeMapView.addMarker(marker);
        marker.setId(id);        // the annotation needs to know its id
        marker.setMapView(this); // the annotation needs to know which map view it is in
        mAnnotations.add(marker);
        return marker;
    }

      @UiThread
    @NonNull
    List<Marker> addMarkers(@NonNull List<MarkerOptions> markerOptionsList) {
        if (markerOptionsList == null) {
            Log.w(TAG, "markerOptionsList was null, so just returning null");
            return null;
        }

        int count = markerOptionsList.size();
        List<Marker> markers = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            MarkerOptions markerOptions = markerOptionsList.get(i);
            Marker marker = prepareMarker(markerOptions);
            markers.add(marker);
        }

        long[] ids = mNativeMapView.addMarkers(markers);

        Marker m;
        for (int i = 0; i < count; i++) {
            m = markers.get(i);
            m.setId(ids[i]);
            m.setMapView(this);
            mAnnotations.add(m);
        }

        return new ArrayList<>(markers);
    }

    @UiThread
    @NonNull
    Polyline addPolyline(@NonNull PolylineOptions polylineOptions) {
        if (polylineOptions == null) {
            Log.w(TAG, "polylineOptions was null, so just returning null");
            return null;
        }

        Polyline polyline = polylineOptions.getPolyline();
        long id = mNativeMapView.addPolyline(polyline);
        polyline.setId(id);
        polyline.setMapView(this);
        mAnnotations.add(polyline);
        return polyline;
    }

    @UiThread
    @NonNull
    List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList) {
        if (polylineOptionsList == null) {
            Log.w(TAG, "polylineOptionsList was null, so just returning null");
            return null;
        }

        int count = polylineOptionsList.size();
        List<Polyline> polylines = new ArrayList<>(count);
        for (PolylineOptions options : polylineOptionsList) {
            polylines.add(options.getPolyline());
        }

        long[] ids = mNativeMapView.addPolylines(polylines);

        Polyline p;
        for (int i = 0; i < count; i++) {
            p = polylines.get(i);
            p.setId(ids[i]);
            p.setMapView(this);
            mAnnotations.add(p);
        }

        return new ArrayList<>(polylines);
    }

    @UiThread
    @NonNull
    Polygon addPolygon(@NonNull PolygonOptions polygonOptions) {
        if (polygonOptions == null) {
            Log.w(TAG, "polygonOptions was null, so just returning null");
            return null;
        }

        Polygon polygon = polygonOptions.getPolygon();
        long id = mNativeMapView.addPolygon(polygon);
        polygon.setId(id);
        polygon.setMapView(this);
        mAnnotations.add(polygon);
        return polygon;
    }


    @UiThread
    @NonNull
    List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList) {
        if (polygonOptionsList == null) {
            Log.w(TAG, "polygonOptionsList was null, so just returning null");
            return null;
        }

        int count = polygonOptionsList.size();
        List<Polygon> polygons = new ArrayList<>(count);
        for (PolygonOptions polygonOptions : polygonOptionsList) {
            polygons.add(polygonOptions.getPolygon());
        }

        long[] ids = mNativeMapView.addPolygons(polygons);

        Polygon p;
        for (int i = 0; i < count; i++) {
            p = polygons.get(i);
            p.setId(ids[i]);
            p.setMapView(this);
            mAnnotations.add(p);
        }

        return new ArrayList<>(polygons);
    }

    @UiThread
    void removeMarker(@NonNull Marker marker) {
        removeAnnotation(marker);
    }

    @UiThread
    public void removeAnnotation(@NonNull Annotation annotation) {
        if (annotation == null) {
            Log.w(TAG, "annotation was null, so just returning");
            return;
        }

        if (annotation instanceof Marker) {
            ((Marker) annotation).hideInfoWindow();
        }
        long id = annotation.getId();
        mNativeMapView.removeAnnotation(id);
        mAnnotations.remove(annotation);
    }

    @UiThread
    void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
        if (annotationList == null) {
            Log.w(TAG, "annotationList was null, so just returning");
            return;
        }

        int count = annotationList.size();
        long[] ids = new long[count];
        for (int i = 0; i < count; i++) {
            ids[i] = annotationList.get(i).getId();
        }
        mNativeMapView.removeAnnotations(ids);
    }

    @UiThread
    void removeAllAnnotations() {
        int count = mAnnotations.size();
        long[] ids = new long[mAnnotations.size()];

        for (int i = 0; i < count; i++) {
            Annotation annotation = mAnnotations.get(i);
            long id = annotation.getId();
            ids[i] = id;
            if (annotation instanceof Marker) {
                ((Marker) annotation).hideInfoWindow();
            }
        }

        mNativeMapView.removeAnnotations(ids);
        mAnnotations.clear();
    }

    @NonNull
    List<Annotation> getAllAnnotations() {
        return new ArrayList<>(mAnnotations);
    }

    private List<Marker> getMarkersInBounds(@NonNull BoundingBox bbox) {
        if (bbox == null) {
            Log.w(TAG, "bbox was null, so just returning null");
            return null;
        }

        // TODO: filter in JNI using C++ parameter to getAnnotationsInBounds
        long[] ids = mNativeMapView.getAnnotationsInBounds(bbox);

        List<Long> idsList = new ArrayList<>(ids.length);
        for (int i = 0; i < ids.length; i++) {
            idsList.add(ids[i]);
        }

        List<Marker> annotations = new ArrayList<>(ids.length);
        int count = mAnnotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = mAnnotations.get(i);
            if (annotation instanceof Marker && idsList.contains(annotation.getId())) {
                annotations.add((Marker) annotation);
            }
        }

        return new ArrayList<>(annotations);
    }

    private int getTopOffsetPixelsForIcon(Icon icon) {
        // This method will dead lock if map paused. Causes a freeze if you add a marker in an
        // activity's onCreate()
        if (mNativeMapView.isPaused()) {
            return 0;
        }

        return (int) (mNativeMapView.getTopOffsetPixelsForAnnotationSymbol(icon.getId())
                * mScreenDensity);
    }

    @UiThread
    double getMetersPerPixelAtLatitude(@FloatRange(from = -180, to = 180) double latitude) {
        return mNativeMapView.getMetersPerPixelAtLatitude(latitude, getZoomLevel()) / mScreenDensity;
    }

    @UiThread
    void selectMarker(@NonNull Marker marker) {
        if (marker == null) {
            Log.w(TAG, "marker was null, so just returning");
            return;
        }

        if (mSelectedMarkers.contains(marker)) {
            return;
        }

        // Need to deselect any currently selected annotation first
        if (!isAllowConcurrentMultipleOpenInfoWindows()) {
            deselectMarkers();
        }

        boolean handledDefaultClick = false;
        if (mOnMarkerClickListener != null) {
            // end developer has provided a custom click listener
            handledDefaultClick = mOnMarkerClickListener.onMarkerClick(marker);
        }

        if (!handledDefaultClick) {
            // default behaviour show InfoWindow
            mInfoWindows.add(marker.showInfoWindow());
        }

        mSelectedMarkers.add(marker);
    }

    @UiThread
    void deselectMarkers() {
        if (mSelectedMarkers.isEmpty()) {
            return;
        }

        for (Marker marker : mSelectedMarkers) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
            }
        }

        // Removes all selected markers from the list
        mSelectedMarkers.clear();
    }

    @UiThread
    public void deselectMarker(@NonNull Marker marker) {
        if (!mSelectedMarkers.contains(marker)) {
            return;
        }

        if (marker.isInfoWindowShown()) {
            marker.hideInfoWindow();
        }

        mSelectedMarkers.remove(marker);
    }

    /**
     * Gets the currently selected marker.
     *
     * @return The currently selected marker.
     */
    @UiThread
    @Nullable
    List<Marker> getSelectedMarkers() {
        return mSelectedMarkers;
    }

    //
    // Mapbox Core GL Camera
    //

    @UiThread
    private void jumpTo(double bearing, LatLng center, double pitch, double zoom) {
        mNativeMapView.jumpTo(bearing, center, pitch, zoom);
    }

    @UiThread
    private void easeTo(double bearing, LatLng center, long duration, double pitch, double zoom) {
        mNativeMapView.easeTo(bearing, center, duration, pitch, zoom);
    }

    @UiThread
    private void flyTo(double bearing, LatLng center, long duration, double pitch, double zoom) {
        mNativeMapView.flyTo(bearing, center, duration, pitch, zoom);
    }

    void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
        mNativeMapView.setVisibleCoordinateBounds(coordinates, new RectF(padding.left / mScreenDensity,
                        padding.top / mScreenDensity, padding.right / mScreenDensity, padding.bottom / mScreenDensity),
                direction, duration);
    }

    private void adjustTopOffsetPixels() {
        int count = mAnnotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = mAnnotations.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.setTopOffsetPixels(
                        getTopOffsetPixelsForIcon(marker.getIcon()));
            }
        }

        for (Marker marker : mSelectedMarkers) {
            if (marker.isInfoWindowShown()) {
                Marker temp = marker;
                temp.hideInfoWindow();
                temp.showInfoWindow();
                marker = temp;
            }
        }
    }

    private void reloadMarkers() {
        int count = mAnnotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = mAnnotations.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                mNativeMapView.removeAnnotation(annotation.getId());
                long newId = mNativeMapView.addMarker(marker);
                marker.setId(newId);
            }
        }
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

        if (!mNativeMapView.isPaused()) {
            mNativeMapView.renderSync();
        }
    }

    @Override
    protected void onSizeChanged(int width, int height, int oldw, int oldh) {
        if (!isInEditMode()) {
            mNativeMapView.resizeView((int) (width / mScreenDensity), (int) (height / mScreenDensity));
        }
    }

    // This class handles TextureView callbacks
    private class SurfaceTextureListener implements TextureView.SurfaceTextureListener {

        // Called when the native surface texture has been created
        // Must do all EGL/GL ES initialization here
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            mNativeMapView.createSurface(new Surface(surface));
            mNativeMapView.resizeFramebuffer(width, height);
        }

        // Called when the native surface texture has been destroyed
        // Must do all EGL/GL ES destruction here
        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            if (mNativeMapView != null) {
                mNativeMapView.destroySurface();
            }
            return true;
        }

        // Called when the format or size of the native surface texture has been changed
        // Must handle window resizing here.
        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            mNativeMapView.resizeFramebuffer(width, height);
        }

        // Called when the SurfaceTexure frame is drawn to screen
        // Must sync with UI here
        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {
            mCompassView.update(getDirection());
            mUserLocationView.update();
            for (InfoWindow infoWindow : mInfoWindows) {
                infoWindow.update();
            }
        }
    }

    // Used by UserLocationView
    void update() {
        if (mNativeMapView != null) {
            mNativeMapView.update();
        }
    }

    //
    // Bearing
    //

    /**
     * Get Bearing in degrees
     *
     * @return Bearing in degrees
     */
    double getBearing() {
        return mNativeMapView.getBearing();
    }

    /**
     * Set Bearing in degrees
     *
     * @param bearing Bearing in degrees
     */
    void setBearing(float bearing) {
        mNativeMapView.setBearing(bearing);
    }

    /**
     * Sets Bearing in degrees
     * <p>
     * NOTE: Used by UserLocationView
     *
     * @param bearing  Bearing in degrees
     * @param duration Length of time to rotate
     */
    void setBearing(float bearing, long duration) {
        mNativeMapView.setBearing(bearing, duration);
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
        if (mZoomControlsEnabled) {
            mZoomButtonsController.setVisible(false);
        }
    }

    // Called when view is hidden and shown
    @Override
    protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
        // Required by ZoomButtonController (from Android SDK documentation)
        if (mZoomControlsEnabled && (visibility != View.VISIBLE)) {
            mZoomButtonsController.setVisible(false);
        }
        if (mZoomControlsEnabled && (visibility == View.VISIBLE)
                && mZoomEnabled) {
            mZoomButtonsController.setVisible(true);
        }
    }

    //
    // Touch events
    //

    void setAllGesturesEnabled(boolean enabled) {
        setZoomEnabled(enabled);
        setScrollEnabled(enabled);
        setRotateEnabled(enabled);
        setTiltEnabled(enabled);
    }

    // Called when user touches the screen, all positions are absolute
    @Override
    public boolean onTouchEvent(@NonNull MotionEvent event) {
        // Check and ignore non touch or left clicks

        if ((event.getButtonState() != 0) && (event.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
            return false;
        }

        // Check two finger gestures first
        mRotateGestureDetector.onTouchEvent(event);
        mScaleGestureDetector.onTouchEvent(event);
        mShoveGestureDetector.onTouchEvent(event);

        // Handle two finger tap
        switch (event.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                // First pointer down
                mNativeMapView.setGestureInProgress(true);
                break;

            case MotionEvent.ACTION_POINTER_DOWN:
                // Second pointer down
                mTwoTap = event.getPointerCount() == 2;
                break;

            case MotionEvent.ACTION_POINTER_UP:
                // Second pointer up
                break;

            case MotionEvent.ACTION_UP:
                // First pointer up
                long tapInterval = event.getEventTime() - event.getDownTime();
                boolean isTap = tapInterval <= ViewConfiguration.getTapTimeout();
                boolean inProgress = mRotateGestureDetector.isInProgress()
                        || mScaleGestureDetector.isInProgress()
                        || mShoveGestureDetector.isInProgress();

                if (mTwoTap && isTap && !inProgress) {
                    PointF focalPoint = TwoFingerGestureDetector.determineFocalPoint(event);
                    zoom(false, focalPoint.x, focalPoint.y);
                    mTwoTap = false;
                    return true;
                }

                mTwoTap = false;
                mNativeMapView.setGestureInProgress(false);
                break;

            case MotionEvent.ACTION_CANCEL:
                mTwoTap = false;
                mNativeMapView.setGestureInProgress(false);
                break;
        }

        boolean retVal = mGestureDetector.onTouchEvent(event);
        return retVal || super.onTouchEvent(event);
    }

    // This class handles one finger gestures
    private class GestureListener extends
            GestureDetector.SimpleOnGestureListener {

        // Must always return true otherwise all events are ignored
        @Override
        public boolean onDown(MotionEvent event) {
            // Show the zoom controls
            if (mZoomControlsEnabled && mZoomEnabled) {
                mZoomButtonsController.setVisible(true);
            }

            // Disable tracking mode if a gesture occurs
            try {
                setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
                setMyBearingTrackingMode(MyBearingTracking.NONE);
            } catch (SecurityException ignore) {
                // User did not accept location permissions
            }

            return true;
        }

        // Called for double taps
        @Override
        public boolean onDoubleTapEvent(MotionEvent e) {
            if (!mZoomEnabled) {
                return false;
            }

            switch (e.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    break;
                case MotionEvent.ACTION_MOVE:
                    break;
                case MotionEvent.ACTION_UP:
                    if (mQuickZoom) {
                        mQuickZoom = false;
                        break;
                    }

                    // Single finger double tap
                    if (mUserLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE) {
                        // Zoom in on gesture
                        zoom(true, e.getX(), e.getY());
                    } else {
                        // Zoom in on center map
                        zoom(true, getWidth() / 2, getHeight() / 2);
                    }
                    break;
            }

            return true;
        }

        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            // Cancel any animation
            mNativeMapView.cancelTransitions();
            return true;
        }

        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            // Open / Close InfoWindow
            PointF tapPoint = new PointF(e.getX(), e.getY());

            final float toleranceSides = 30 * mScreenDensity;
            final float toleranceTop = 40 * mScreenDensity;
            final float toleranceBottom = 10 * mScreenDensity;

            RectF tapRect = new RectF(tapPoint.x - toleranceSides, tapPoint.y + toleranceTop,
                    tapPoint.x + toleranceSides, tapPoint.y - toleranceBottom);

            List<LatLng> corners = Arrays.asList(
                    fromScreenLocation(new PointF(tapRect.left, tapRect.bottom)),
                    fromScreenLocation(new PointF(tapRect.left, tapRect.top)),
                    fromScreenLocation(new PointF(tapRect.right, tapRect.top)),
                    fromScreenLocation(new PointF(tapRect.right, tapRect.bottom))
            );

            BoundingBox tapBounds = BoundingBox.fromLatLngs(corners);

            List<Marker> nearbyMarkers = getMarkersInBounds(tapBounds);

            long newSelectedMarkerId;

            if (nearbyMarkers.size() > 0) {

                // there is at least one nearby marker; select one
                //
                // first, sort for comparison and iteration
                Collections.sort(nearbyMarkers);

                if (nearbyMarkers == mMarkersNearLastTap) {

                    // TODO: We still need to adapt this logic to the new mSelectedMarkers list,
                    // though the basic functionality is there.

                    // the selection candidates haven't changed; cycle through them
//                    if (mSelectedMarker != null
//                            && (mSelectedMarker.getId() == mMarkersNearLastTap.get(mMarkersNearLastTap.size() - 1).getId())) {
//                        // the selected marker is the last in the set; cycle back to the first
//                        // note: this could be the selected marker if only one in set
//                        newSelectedMarkerId = mMarkersNearLastTap.get(0).getId();
//                    } else if (mSelectedMarker != null) {
//                        // otherwise increment the selection through the candidates
//                        long result = mMarkersNearLastTap.indexOf(mSelectedMarker);
//                        newSelectedMarkerId = mMarkersNearLastTap.get((int) result + 1).getId();
//                    } else {
                    // no current selection; select the first one
                    newSelectedMarkerId = mMarkersNearLastTap.get(0).getId();
//                    }
                } else {
                    // start tracking a new set of nearby markers
                    mMarkersNearLastTap = nearbyMarkers;

                    // select the first one
                    newSelectedMarkerId = mMarkersNearLastTap.get(0).getId();
                }

            } else {
                // there are no nearby markers; deselect if necessary
                newSelectedMarkerId = -1;
            }

            if (newSelectedMarkerId >= 0) {

                int count = mAnnotations.size();
                for (int i = 0; i < count; i++) {
                    Annotation annotation = mAnnotations.get(i);
                    if (annotation instanceof Marker) {
                        if (annotation.getId() == newSelectedMarkerId) {
                            if (mSelectedMarkers.isEmpty() || !mSelectedMarkers.contains(annotation)) {
                                selectMarker((Marker) annotation);
                            }
                            break;
                        }
                    }
                }

            } else {
                // deselect any selected marker
                deselectMarkers();

                // notify app of map click
                if (mOnMapClickListener != null) {
                    LatLng point = fromScreenLocation(tapPoint);
                    mOnMapClickListener.onMapClick(point);
                }
            }

            return true;
        }

        // Called for a long press
        @Override
        public void onLongPress(MotionEvent e) {
            if (mOnMapLongClickListener != null && !mQuickZoom) {
                LatLng point = fromScreenLocation(new PointF(e.getX(), e.getY()));
                mOnMapLongClickListener.onMapLongClick(point);
            }
        }

        // Called for flings
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                               float velocityY) {
            if (!mScrollEnabled) {
                return false;
            }

            // Fling the map
            float ease = 0.25f;

            velocityX = velocityX * ease;
            velocityY = velocityY * ease;

            double speed = Math.sqrt(velocityX * velocityX + velocityY * velocityY);
            double deceleration = 2500;
            double duration = speed / (deceleration * ease);

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            mNativeMapView.moveBy(velocityX * duration / 2.0 / mScreenDensity, velocityY * duration / 2.0 / mScreenDensity, (long) (duration * 1000.0f));

            if (mOnFlingListener != null) {
                mOnFlingListener.onFling();
            }

            return true;
        }

        // Called for drags
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (!mScrollEnabled) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Scroll the map
            mNativeMapView.moveBy(-distanceX / mScreenDensity, -distanceY / mScreenDensity);

            if (mOnScrollListener != null) {
                mOnScrollListener.onScroll();
            }

            return true;
        }
    }

    // This class handles two finger gestures and double-tap drag gestures
    private class ScaleGestureListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {

        long mBeginTime = 0;
        float mScaleFactor = 1.0f;

        // Called when two fingers first touch the screen
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            if (!mZoomEnabled) {
                return false;
            }

            mBeginTime = detector.getEventTime();
            return true;
        }

        // Called when fingers leave screen
        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
            mBeginTime = 0;
            mScaleFactor = 1.0f;
            mZoomStarted = false;
        }

        // Called each time a finger moves
        // Called for pinch zooms and quickzooms/quickscales
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            if (!mZoomEnabled) {
                return false;
            }

            // If scale is large enough ignore a tap
            mScaleFactor *= detector.getScaleFactor();
            if ((mScaleFactor > 1.05f) || (mScaleFactor < 0.95f)) {
                mZoomStarted = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small scales
            long time = detector.getEventTime();
            long interval = time - mBeginTime;
            if (!mZoomStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            if (!mZoomStarted) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Gesture is a quickzoom if there aren't two fingers
            mQuickZoom = !mTwoTap;

            // Scale the map
            if (mScrollEnabled && !mQuickZoom && mUserLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE) {
                // around gesture
                mNativeMapView.scaleBy(detector.getScaleFactor(), detector.getFocusX() / mScreenDensity, detector.getFocusY() / mScreenDensity);
            } else {
                // around center map
                mNativeMapView.scaleBy(detector.getScaleFactor(), (getWidth() / 2) / mScreenDensity, (getHeight() / 2) / mScreenDensity);
            }
            return true;
        }
    }

    // This class handles two finger rotate gestures
    private class RotateGestureListener extends RotateGestureDetector.SimpleOnRotateGestureListener {

        long mBeginTime = 0;
        float mTotalAngle = 0.0f;
        boolean mStarted = false;

        // Called when two fingers first touch the screen
        @Override
        public boolean onRotateBegin(RotateGestureDetector detector) {
            if (!mRotateEnabled) {
                return false;
            }

            mBeginTime = detector.getEventTime();
            return true;
        }

        // Called when the fingers leave the screen
        @Override
        public void onRotateEnd(RotateGestureDetector detector) {
            mBeginTime = 0;
            mTotalAngle = 0.0f;
            mStarted = false;
        }

        // Called each time one of the two fingers moves
        // Called for rotation
        @Override
        public boolean onRotate(RotateGestureDetector detector) {
            if (!mRotateEnabled) {
                return false;
            }

            // If rotate is large enough ignore a tap
            // Also is zoom already started, don't rotate
            mTotalAngle += detector.getRotationDegreesDelta();
            if (!mZoomStarted && ((mTotalAngle > 10.0f) || (mTotalAngle < -10.0f))) {
                mStarted = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small rotate
            long time = detector.getEventTime();
            long interval = time - mBeginTime;
            if (!mStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            if (!mStarted) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Get rotate value
            double bearing = mNativeMapView.getBearing();
            bearing += detector.getRotationDegreesDelta();

            // Rotate the map
            if (mUserLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE) {
                // around gesture
                mNativeMapView.setBearing(bearing,
                        detector.getFocusX() / mScreenDensity,
                        (getHeight() - detector.getFocusY()) / mScreenDensity);
            } else {
                // around center map
                mNativeMapView.setBearing(bearing,
                        (getWidth() / 2) / mScreenDensity,
                        (getHeight() / 2) / mScreenDensity);
            }
            return true;
        }
    }

    // This class handles a vertical two-finger shove. (If you place two fingers on screen with
    // less than a 20 degree angle between them, this will detect movement on the Y-axis.)
    private class ShoveGestureListener implements ShoveGestureDetector.OnShoveGestureListener {

        long mBeginTime = 0;
        float mTotalDelta = 0.0f;
        boolean mStarted = false;

        @Override
        public boolean onShoveBegin(ShoveGestureDetector detector) {
            if (!mTiltEnabled) {
                return false;
            }

            mBeginTime = detector.getEventTime();
            return true;
        }

        @Override
        public void onShoveEnd(ShoveGestureDetector detector) {
            mBeginTime = 0;
            mTotalDelta = 0.0f;
            mStarted = false;
        }

        @Override
        public boolean onShove(ShoveGestureDetector detector) {
            if (!mTiltEnabled) {
                return false;
            }

            // If tilt is large enough ignore a tap
            // Also if zoom already started, don't tilt
            mTotalDelta += detector.getShovePixelsDelta();
            if (!mZoomStarted && ((mTotalDelta > 10.0f) || (mTotalDelta < -10.0f))) {
                mStarted = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small tilt
            long time = detector.getEventTime();
            long interval = time - mBeginTime;
            if (!mStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            if (!mStarted) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Get tilt value (scale and clamp)
            double pitch = getTilt();
            pitch -= 0.1 * detector.getShovePixelsDelta();
            pitch = Math.max(MINIMUM_TILT, Math.min(MAXIMUM_TILT, pitch));

            // Tilt the map
            setTilt(pitch, null);

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
            if (!mZoomEnabled) {
                return;
            }

            // Zoom in or out
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
                if (!mScrollEnabled) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move left
                mNativeMapView.moveBy(scrollDist / mScreenDensity, 0.0 / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_RIGHT:
                if (!mScrollEnabled) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move right
                mNativeMapView.moveBy(-scrollDist / mScreenDensity, 0.0 / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_UP:
                if (!mScrollEnabled) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move up
                mNativeMapView.moveBy(0.0 / mScreenDensity, scrollDist / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_DOWN:
                if (!mScrollEnabled) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move down
                mNativeMapView.moveBy(0.0 / mScreenDensity, -scrollDist / mScreenDensity);
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
                if (!mZoomEnabled) {
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
                if (!mZoomEnabled) {
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
        // Choose the action
        switch (event.getActionMasked()) {
            // The trackball was rotated
            case MotionEvent.ACTION_MOVE:
                if (!mScrollEnabled) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Scroll the map
                mNativeMapView.moveBy(-10.0 * event.getX() / mScreenDensity, -10.0 * event.getY() / mScreenDensity);
                return true;

            // Trackball was pushed in so start tracking and tell system we are
            // interested
            // We will then get the up action
            case MotionEvent.ACTION_DOWN:
                // Set up a delayed callback to check if trackball is still
                // After waiting the system long press time out
                if (mCurrentTrackballLongPressTimeOut != null) {
                    mCurrentTrackballLongPressTimeOut.cancel();
                    mCurrentTrackballLongPressTimeOut = null;
                }
                mCurrentTrackballLongPressTimeOut = new TrackballLongPressTimeOut();
                postDelayed(mCurrentTrackballLongPressTimeOut,
                        ViewConfiguration.getLongPressTimeout());
                return true;

            // Trackball was released
            case MotionEvent.ACTION_UP:
                if (!mZoomEnabled) {
                    return false;
                }

                // Only handle if we have not already long pressed
                if (mCurrentTrackballLongPressTimeOut != null) {
                    // Zoom in
                    zoom(true);
                }
                return true;

            // Trackball was cancelled
            case MotionEvent.ACTION_CANCEL:
                if (mCurrentTrackballLongPressTimeOut != null) {
                    mCurrentTrackballLongPressTimeOut.cancel();
                    mCurrentTrackballLongPressTimeOut = null;
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

        public TrackballLongPressTimeOut() {
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
                mCurrentTrackballLongPressTimeOut = null;
            }
        }
    }

    // Called for events that don't fit the other handlers
    // such as mouse scroll events, mouse moves, joystick, trackpad
    @Override
    public boolean onGenericMotionEvent(MotionEvent event) {
        // Mouse events
        //if (event.isFromSource(InputDevice.SOURCE_CLASS_POINTER)) { // this is not available before API 18
        if ((event.getSource() & InputDevice.SOURCE_CLASS_POINTER) == InputDevice.SOURCE_CLASS_POINTER) {
            // Choose the action
            switch (event.getActionMasked()) {
                // Mouse scrolls
                case MotionEvent.ACTION_SCROLL:
                    if (!mZoomEnabled) {
                        return false;
                    }

                    // Cancel any animation
                    mNativeMapView.cancelTransitions();

                    // Get the vertical scroll amount, one click = 1
                    float scrollDist = event.getAxisValue(MotionEvent.AXIS_VSCROLL);

                    // Scale the map by the appropriate power of two factor
                    mNativeMapView.scaleBy(Math.pow(2.0, scrollDist), event.getX() / mScreenDensity, event.getY() / mScreenDensity);

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
                if (mZoomControlsEnabled && mZoomEnabled) {
                    mZoomButtonsController.setVisible(true);
                }
                return true;

            case MotionEvent.ACTION_HOVER_EXIT:
                // Hide the zoom controls
                if (mZoomControlsEnabled) {
                    mZoomButtonsController.setVisible(false);
                }

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
            if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
                boolean noConnectivity = intent.getBooleanExtra(ConnectivityManager.EXTRA_NO_CONNECTIVITY, false);
                onConnectivityChanged(!noConnectivity);
            }
        }
    }

    // Called when our Internet connectivity has changed
    private void onConnectivityChanged(boolean isConnected) {
        mNativeMapView.setReachability(isConnected);
    }

    //
    // Map events
    //

    @UiThread
    void addOnMapChangedListener(@Nullable MapboxMap.OnMapChangedListener listener) {
        if (listener != null) {
            mOnMapChangedListener.add(listener);
        }
    }

    @UiThread
    void removeOnMapChangedListener(@Nullable MapboxMap.OnMapChangedListener listener) {
        if (listener != null) {
            mOnMapChangedListener.remove(listener);
        }
    }

    // Called when the map view transformation has changed
    // Called via JNI from NativeMapView
    // Forward to any listeners
    protected void onMapChanged(int mapChange) {
        if (mOnMapChangedListener != null) {
            int count = mOnMapChangedListener.size();
            for (int i = 0; i < count; i++) {
                mOnMapChangedListener.get(i).onMapChanged(mapChange);
            }
        }
    }

    @UiThread
    void setInfoWindowAdapter(@Nullable MapboxMap.InfoWindowAdapter infoWindowAdapter) {
        mInfoWindowAdapter = infoWindowAdapter;
    }

    @UiThread
    @Nullable
    public MapboxMap.InfoWindowAdapter getInfoWindowAdapter() {
        return mInfoWindowAdapter;
    }

    @UiThread
    void setOnFpsChangedListener(@Nullable MapboxMap.OnFpsChangedListener listener) {
        mOnFpsChangedListener = listener;
    }

    // Called when debug mode is enabled to update a FPS counter
    // Called via JNI from NativeMapView
    // Forward to any listener
    protected void onFpsChanged(final double fps) {
        post(new Runnable() {
            @Override
            public void run() {
                if (mOnFpsChangedListener != null) {
                    mOnFpsChangedListener.onFpsChanged(fps);
                }
            }
        });
    }

    @UiThread
    void setOnScrollListener(@Nullable MapboxMap.OnScrollListener listener) {
        mOnScrollListener = listener;
    }

    @UiThread
    void setOnFlingListener(@Nullable MapboxMap.OnFlingListener listener) {
        mOnFlingListener = listener;
    }

    @UiThread
    void setOnMapClickListener(@Nullable MapboxMap.OnMapClickListener listener) {
        mOnMapClickListener = listener;
    }

    @UiThread
    void setOnMapLongClickListener(@Nullable MapboxMap.OnMapLongClickListener listener) {
        mOnMapLongClickListener = listener;
    }

    @UiThread
    void setOnMarkerClickListener(@Nullable MapboxMap.OnMarkerClickListener listener) {
        mOnMarkerClickListener = listener;
    }

    @UiThread
    @Nullable
    public MapboxMap.OnInfoWindowClickListener getOnInfoWindowClickListener() {
        return mOnInfoWindowClickListener;
    }

    @UiThread
    void setOnInfoWindowClickListener(@Nullable MapboxMap.OnInfoWindowClickListener listener) {
        mOnInfoWindowClickListener = listener;
    }

    //
    // User location
    //

    @UiThread
    boolean isMyLocationEnabled() {
        return mUserLocationView.isEnabled();
    }

    @UiThread
    void setMyLocationEnabled(boolean enabled) {
        mUserLocationView.setEnabled(enabled);
    }

    @UiThread
    @Nullable
    Location getMyLocation() {
        return mUserLocationView.getLocation();
    }

    @UiThread
    void setOnMyLocationChangeListener(@Nullable MapboxMap.OnMyLocationChangeListener listener) {
        mUserLocationView.setOnMyLocationChangeListener(listener);
    }

    @UiThread
    void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && !isMyLocationEnabled()) {
            setMyLocationEnabled(true);
        }

        mUserLocationView.setMyLocationTrackingMode(myLocationTrackingMode);

        if (mOnMyLocationTrackingModeChangeListener != null) {
            mOnMyLocationTrackingModeChangeListener.onMyLocationTrackingModeChange(myLocationTrackingMode);
        }
    }

    @UiThread
    @MyLocationTracking.Mode
    int getMyLocationTrackingMode() {
        return mUserLocationView.getMyLocationTrackingMode();
    }

    @UiThread
    void setOnMyLocationTrackingModeChangeListener(@Nullable MapboxMap.OnMyLocationTrackingModeChangeListener listener) {
        mOnMyLocationTrackingModeChangeListener = listener;
    }

    @UiThread
    void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        if (!isMyLocationEnabled()) {
            setMyLocationEnabled(true);
        }

        mUserLocationView.setMyBearingTrackingMode(myBearingTrackingMode);

        if (mOnMyBearingTrackingModeChangeListener != null) {
            mOnMyBearingTrackingModeChangeListener.onMyBearingTrackingModeChange(myBearingTrackingMode);
        }
    }

    @UiThread
    @MyLocationTracking.Mode
    int getMyBearingTrackingMode() {
        //noinspection ResourceType
        return mUserLocationView.getMyBearingTrackingMode();
    }

    @UiThread
    void setOnMyBearingTrackingModeChangeListener(@Nullable MapboxMap.OnMyBearingTrackingModeChangeListener listener) {
        mOnMyBearingTrackingModeChangeListener = listener;
    }

    //
    // Compass
    //

    /**
     * Returns whether the compass is enabled.
     *
     * @return True if the compass is enabled; false if the compass is disabled.
     */
    @UiThread
    boolean isCompassEnabled() {
        return mCompassView.isEnabled();
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
    @UiThread
    void setCompassEnabled(boolean compassEnabled) {
        mCompassView.setEnabled(compassEnabled);
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
    void setCompassGravity(int gravity) {
        setWidgetGravity(mCompassView, gravity);
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
    void setCompassMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mCompassView, left, top, right, bottom);
    }

    //
    // Logo
    //

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
    @UiThread
    void setLogoGravity(int gravity) {
        setWidgetGravity(mLogoView, gravity);
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
    @UiThread
    void setLogoMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mLogoView, left, top, right, bottom);
    }

    /**
     * <p>
     * Enables or disables the Mapbox logo.
     * </p>
     * By default, the compass is enabled.
     *
     * @param visibility True to enable the logo; false to disable the logo.
     */
    @UiThread
    void setLogoVisibility(int visibility) {
        mLogoView.setVisibility(visibility);
    }

    //
    // Attribution
    //

    /**
     * <p>
     * Sets the gravity of the attribution button view. Use this to change the corner of the map
     * view that the attribution button is displayed in.
     * </p>
     * By default, the attribution button is in the bottom left corner.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
    @UiThread
    void setAttributionGravity(int gravity) {
        setWidgetGravity(mAttributionsView, gravity);
    }

    /**
     * Sets the margins of the attribution button view. Use this to change the distance of the
     * attribution button from the map view edge.
     *
     * @param left   The left margin in pixels.
     * @param top    The top margin in pixels.
     * @param right  The right margin in pixels.
     * @param bottom The bottom margin in pixels.
     */
    @UiThread
    void setAttributionMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mAttributionsView, left, top, right, bottom);
    }

    /**
     * <p>
     * Enables or disables the attribution button. The attribution is a button with an "i" than when
     * clicked shows a menu with copyright and legal notices. The menu also inlcudes the "Improve
     * this map" link which user can report map errors with.
     * </p>
     * By default, the attribution button is enabled.
     *
     * @param visibility True to enable the attribution button; false to disable the attribution button.
     */
    @UiThread
    void setAttributionVisibility(int visibility) {
        mAttributionsView.setVisibility(visibility);
    }

    @UiThread
    void addCustomLayer(CustomLayer customLayer, String before) {
        mNativeMapView.addCustomLayer(customLayer, before);
    }

    @UiThread
    void removeCustomLayer(String id) {
        mNativeMapView.removeCustomLayer(id);
    }

    @UiThread
    void invalidateCustomLayers() {
        mNativeMapView.update();
    }

    private void setWidgetGravity(@NonNull final View view, int gravity) {
        LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
        layoutParams.gravity = gravity;
        view.setLayoutParams(layoutParams);
    }

    private void setWidgetMargins(@NonNull final View view, int left, int top, int right, int bottom) {
        LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
        layoutParams.setMargins(left, top, right, bottom);
        view.setLayoutParams(layoutParams);
    }

    private void setWidgetMargins(@NonNull final View view, float leftDp, float topDp, float rightDp, float bottomDp) {
        LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
        layoutParams.setMargins((int) (leftDp * mScreenDensity), (int) (topDp * mScreenDensity), (int) (rightDp * mScreenDensity), (int) (bottomDp * mScreenDensity));
        view.setLayoutParams(layoutParams);
    }

    private static class AttributionOnClickListener implements View.OnClickListener, DialogInterface.OnClickListener {

        private MapView mMapView;

        public AttributionOnClickListener(MapView mapView) {
            mMapView = mapView;
        }

        // Called when someone presses the attribution icon
        @Override
        public void onClick(View v) {
            Context context = v.getContext();
            String[] items = context.getResources().getStringArray(R.array.attribution_names);
            AlertDialog.Builder builder = new AlertDialog.Builder(context, R.style.AttributionAlertDialogStyle);
            builder.setTitle(R.string.attributionsDialogTitle);
            builder.setAdapter(new ArrayAdapter<>(context, R.layout.attribution_list_item, items), this);
            builder.show();
        }

        // Called when someone selects an attribution, 'Improve this map' adds location data to the url
        @Override
        public void onClick(DialogInterface dialog, int which) {
            Context context = ((Dialog) dialog).getContext();
            String url = context.getResources().getStringArray(R.array.attribution_links)[which];
            if (which == ATTRIBUTION_INDEX_IMPROVE_THIS_MAP) {
                LatLng latLng = mMapView.getCenterCoordinate();
                url = String.format(url, latLng.getLongitude(), latLng.getLatitude(), (int) mMapView.getZoomLevel());
            }
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(url));
            context.startActivity(intent);
        }
    }

    public void getMapAsync(OnMapReadyCallback callback) {
        callback.onMapReady(new MapboxMap(this));
    }

}
