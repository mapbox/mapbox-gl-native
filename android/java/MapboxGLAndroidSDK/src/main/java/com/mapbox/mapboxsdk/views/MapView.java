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
import android.support.annotation.IntDef;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.annotations.Sprite;
import com.mapbox.mapboxsdk.annotations.SpriteFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.exceptions.SpriteBitmapChangedException;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.utils.ApiAccess;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/**
 * A {@code MapView} provides an embeddable map interface, similar to the one provided by the Google Maps API.
 * You use this class to display map information and to manipulate the map contents from your application.
 * You can center the map on a given coordinate, specify the size of the area you want to display,
 * and style the features of the map to fit your application's use case.
 * <p/>
 * Use of {@code MapView} requires a Mapbox API access token.
 * Obtain an access token on the <a href="https://www.mapbox.com/account/apps/">Mapbox account page</a>.
 * <p/>
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
    private static final long ANIMATION_DURATION = 300;

    // Used for saving instance state
    private static final String STATE_CENTER_COORDINATE = "centerCoordinate";
    private static final String STATE_CENTER_DIRECTION = "centerDirection";
    private static final String STATE_ZOOM_LEVEL = "zoomLevel";
    private static final String STATE_DIRECTION = "direction";
    private static final String STATE_ZOOM_ENABLED = "zoomEnabled";
    private static final String STATE_SCROLL_ENABLED = "scrollEnabled";
    private static final String STATE_ROTATE_ENABLED = "rotateEnabled";
    private static final String STATE_DEBUG_ACTIVE = "debugActive";
    private static final String STATE_STYLE_URL = "styleUrl";
    private static final String STATE_ACCESS_TOKEN = "accessToken";
    private static final String STATE_STYLE_CLASSES = "styleClasses";
    private static final String STATE_DEFAULT_TRANSITION_DURATION = "defaultTransitionDuration";
    private static final String STATE_MY_LOCATION_ENABLED = "myLocationEnabled";
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
    private boolean mTwoTap = false;

    // Shows zoom buttons
    private ZoomButtonsController mZoomButtonsController;

    // Used to track trackball long presses
    private TrackballLongPressTimeOut mCurrentTrackballLongPressTimeOut;

    // Receives changes to network connectivity
    private ConnectivityReceiver mConnectivityReceiver;

    // Holds the context
    private Context mContext;

    // Used for user location
    private UserLocationView mUserLocationView;

    // Used for the compass
    private CompassView mCompassView;

    // Used for displaying annotations
    // Every annotation that has been added to the map
    private final List<Annotation> mAnnotations = new ArrayList<>();
    private List<Marker> mMarkersNearLastTap = new ArrayList<>();
    private Marker mSelectedMarker;
    private InfoWindowAdapter mInfoWindowAdapter;
    private SpriteFactory mSpriteFactory;
    private ArrayList<Sprite> mSprites = new ArrayList<>();

    // Used for the Mapbox Logo
    private ImageView mLogoView;

    // Used for attributions control
    private ImageView mAttributionsView;

    // Used to manage MapChange event listeners
    private ArrayList<OnMapChangedListener> mOnMapChangedListener = new ArrayList<>();

    // Used to manage map click event listeners
    private OnMapClickListener mOnMapClickListener;
    private OnMapLongClickListener mOnMapLongClickListener;

    // Used to manage fling and scroll event listeners
    private OnFlingListener mOnFlingListener;
    private OnScrollListener mOnScrollListener;

    // Used to manage marker click event listeners
    private OnMarkerClickListener mOnMarkerClickListener;
    private OnInfoWindowClickListener mOnInfoWindowClickListener;

    // Used to manage FPS change event listeners
    private OnFpsChangedListener mOnFpsChangedListener;

    //
    // Properties
    //

    // These are properties with setters/getters, saved in onSaveInstanceState and XML attributes
    private boolean mZoomEnabled = true;
    private boolean mScrollEnabled = true;
    private boolean mRotateEnabled = true;
    private String mStyleUrl;

    //
    // Inner classes
    //

    //
    // Enums
    //

    /**
     * Map change event types.
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
            DID_FINISH_RENDERING_MAP_FULLY_RENDERED
    })
    @Retention(RetentionPolicy.SOURCE)
    public @interface MapChange {
    }

    /**
     * This event is triggered whenever the currently displayed map region is about to changing
     * without an animation.
     * <p/>
     * This event is followed by a series of {@link MapView#REGION_IS_CHANGING} and ends
     * with {@link MapView#REGION_DID_CHANGE}.
     */
    public static final int REGION_WILL_CHANGE = 0;

    /**
     * This event is triggered whenever the currently displayed map region is about to changing
     * with an animation.
     * <p/>
     * This event is followed by a series of {@link MapView#REGION_IS_CHANGING} and ends
     * with {@link MapView#REGION_DID_CHANGE_ANIMATED}.
     */
    public static final int REGION_WILL_CHANGE_ANIMATED = 1;

    /**
     * This event is triggered whenever the currently displayed map region is changing.
     */
    public static final int REGION_IS_CHANGING = 2;

    /**
     * This event is triggered whenever the currently displayed map region finished changing
     * without an animation.
     */
    public static final int REGION_DID_CHANGE = 3;

    /**
     * This event is triggered whenever the currently displayed map region finished changing
     * with an animation.
     */
    public static final int REGION_DID_CHANGE_ANIMATED = 4;

    /**
     * This event is triggered when the map is about to start loading a new map style.
     * <p/>
     * This event is followed by {@link MapView#DID_FINISH_LOADING_MAP} or
     * {@link MapView#DID_FAIL_LOADING_MAP}.
     */
    public static final int WILL_START_LOADING_MAP = 5;

    /**
     * This event is triggered when the map has successfully loaded a new map style.
     */
    public static final int DID_FINISH_LOADING_MAP = 6;

    /**
     * Currently not implemented.
     * <p/>
     * This event is triggered when the map has failed to load a new map style.
     */
    public static final int DID_FAIL_LOADING_MAP = 7;

    /**
     * Currently not implemented.
     */
    public static final int WILL_START_RENDERING_FRAME = 8;

    /**
     * Currently not implemented.
     */
    public static final int DID_FINISH_RENDERING_FRAME = 9;

    /**
     * Currently not implemented.
     */
    public static final int DID_FINISH_RENDERING_FRAME_FULLY_RENDERED = 10;

    /**
     * Currently not implemented.
     */
    public static final int WILL_START_RENDERING_MAP = 11;

    /**
     * Currently not implemented.
     */
    public static final int DID_FINISH_RENDERING_MAP = 12;

    /**
     * Currently not implemented.
     */
    public static final int DID_FINISH_RENDERING_MAP_FULLY_RENDERED = 13;

    //
    // Interfaces
    //

    /**
     * Interface definition for a callback to be invoked when the map is flinged.
     *
     * @see MapView#setOnFlingListener(OnFlingListener)
     */
    public interface OnFlingListener {
        /**
         * Called when the map is flinged.
         */
        void onFling();
    }

    /**
     * Interface definition for a callback to be invoked when the map is scrolled.
     *
     * @see MapView#setOnScrollListener(OnScrollListener)
     */
    public interface OnScrollListener {
        /**
         * Called when the map is scrolled.
         */
        void onScroll();
    }

    /**
     * Interface definition for a callback to be invoked on every frame rendered to the map view.
     *
     * @see MapView#setOnFpsChangedListener(OnFpsChangedListener)
     */
    public interface OnFpsChangedListener {
        /**
         * Called for every frame rendered to the map view.
         *
         * @param fps The average number of frames rendered over the last second.
         */
        void onFpsChanged(double fps);
    }

    /**
     * Interface definition for a callback to be invoked when the user clicks on the map view.
     *
     * @see MapView#setOnMapClickListener(OnMapClickListener)
     */
    public interface OnMapClickListener {
        /**
         * Called when the user clicks on the map view.
         *
         * @param point The projected map coordinate the user clicked on.
         */
        void onMapClick(@NonNull LatLng point);
    }

    /**
     * Interface definition for a callback to be invoked when the user long clicks on the map view.
     *
     * @see MapView#setOnMapLongClickListener(OnMapLongClickListener)
     */
    public interface OnMapLongClickListener {
        /**
         * Called when the user long clicks on the map view.
         *
         * @param point The projected map coordinate the user long clicked on.
         */
        void onMapLongClick(@NonNull LatLng point);
    }

    /**
     * Interface definition for a callback to be invoked when the user clicks on a marker.
     *
     * @see MapView#setOnMarkerClickListener(OnMarkerClickListener)
     */
    public interface OnMarkerClickListener {
        /**
         * Called when the user clicks on a marker.
         *
         * @param marker The marker the user clicked on.
         * @return If true the listener has consumed the event and the info window will not be shown.
         */
        boolean onMarkerClick(@NonNull Marker marker);
    }

    /**
     * Interface definition for a callback to be invoked when the user clicks on an info window.
     *
     * @see MapView#setOnInfoWindowClickListener(OnInfoWindowClickListener)
     */
    public interface OnInfoWindowClickListener {
        /**
         * Called when the user clicks on an info window.
         *
         * @param marker The marker of the info window the user clicked on.
         * @return If true the listener has consumed the event and the info window will not be closed.
         */
        boolean onMarkerClick(@NonNull Marker marker);
    }

    /**
     * Interface definition for a callback to be invoked when the displayed map view changes.
     *
     * @see MapView#addOnMapChangedListener(OnMapChangedListener)
     * @see MapView.MapChange
     */
    public interface OnMapChangedListener {
        /**
         * Called when the displayed map view changes.
         *
         * @param change The type of map change event.
         */
        void onMapChanged(@MapChange int change);
    }

    /**
     * Interface definition for a callback to be invoked when an info window will be shown.
     *
     * @see MapView#setInfoWindowAdapter(InfoWindowAdapter)
     */
    public interface InfoWindowAdapter {
        /**
         * Called when an info window will be shown as a result of a marker click.
         *
         * @param marker The marker the user clicked on.
         * @return View to be shown as a info window. If null is returned the default
         * info window will be shown.
         */
        @Nullable
        View getInfoWindow(@NonNull Marker marker);
    }

    /**
     * Interface definition for a callback to be invoked when the the My Location dot
     * (which signifies the user's location) changes location.
     *
     * @see MapView#setOnMyLocationChangeListener(OnMyLocationChangeListener)
     */
    public interface OnMyLocationChangeListener {
        /**
         * Called when the location of the My Location dot has changed
         * (be it latitude/longitude, bearing or accuracy).
         *
         * @param location The current location of the My Location dot The type of map change event.
         */
        void  onMyLocationChange(@Nullable Location location);
    }

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
            throw new NullPointerException("accessToken is null");
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
            throw new NullPointerException("accessToken is null");
        }
        if (styleUrl == null) {
            throw new NullPointerException("styleUrl is null");
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
            throw new NullPointerException("context is null");
        }

        // Save the context
        mContext = context;

        setWillNotDraw(false);

        // Create the TextureView
        mTextureView = new TextureView(mContext);
        addView(mTextureView);
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
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
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

        // Shows the zoom controls
        // But not when in Eclipse UI editor
        if (!isInEditMode()) {
            if (!context.getPackageManager()
                    .hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
                mZoomButtonsController = new ZoomButtonsController(this);
                mZoomButtonsController.setZoomSpeed(ANIMATION_DURATION);
                mZoomButtonsController.setOnZoomListener(new OnZoomListener());
            }

            // Check current connection status
            ConnectivityManager connectivityManager = (ConnectivityManager) context
                    .getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
            boolean isConnected = (activeNetwork != null) && activeNetwork.isConnectedOrConnecting();
            onConnectivityChanged(isConnected);
        }

        // Setup user location UI
        mUserLocationView = new UserLocationView(this, getContext());
        addView(mUserLocationView);

        // Setup compass
        mCompassView = new CompassView(mContext);
        mCompassView.setOnClickListener(new CompassView.CompassClickListener(this));
        addView(mCompassView);

        // Setup Mapbox logo
        mLogoView = new ImageView(mContext);
        mLogoView.setImageDrawable(ContextCompat.getDrawable(mContext, R.drawable.ic_logo_mapbox));
        mLogoView.setContentDescription(getResources().getString(R.string.mapboxIconContentDescription));
        ViewGroup.LayoutParams logoParams = new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        mLogoView.setLayoutParams(logoParams);
        addView(mLogoView);

        // Setup Attributions control
        mAttributionsView = new ImageView(mContext);
        mAttributionsView.setClickable(true);
        mAttributionsView.setImageResource(R.drawable.ic_info_selector);
        int attrPadding = (int) (DIMENSION_SEVEN_DP * mScreenDensity);
        mAttributionsView.setPadding(attrPadding, attrPadding, attrPadding, attrPadding);
        mAttributionsView.setAdjustViewBounds(true);
        mAttributionsView.setContentDescription(getResources()
                .getString(R.string.attributionsIconContentDescription));
        LayoutParams attrParams = new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        mAttributionsView.setLayoutParams(attrParams);
        addView(mAttributionsView);
        mAttributionsView.setOnClickListener(new AttributionOnClickListener(this));

        // Load the attributes
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.MapView, 0, 0);
        try {
            double centerLatitude = typedArray.getFloat(R.styleable.MapView_center_latitude, 0.0f);
            double centerLongitude = typedArray.getFloat(R.styleable.MapView_center_longitude, 0.0f);
            LatLng centerCoordinate = new LatLng(centerLatitude, centerLongitude);
            setCenterCoordinate(centerCoordinate);
            // need to set zoom level first because of limitation on rotating when zoomed out
            setZoomLevel(typedArray.getFloat(R.styleable.MapView_zoom_level, 0.0f));
            setDirection(typedArray.getFloat(R.styleable.MapView_direction, 0.0f));
            setZoomEnabled(typedArray.getBoolean(R.styleable.MapView_zoom_enabled, true));
            setScrollEnabled(typedArray.getBoolean(R.styleable.MapView_scroll_enabled, true));
            setRotateEnabled(typedArray.getBoolean(R.styleable.MapView_rotate_enabled, true));
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
                setStyleClasses(styleClasses);
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
            setMyLocationEnabled(typedArray.getBoolean(R.styleable.MapView_my_location_enabled, false));
        } finally {
            typedArray.recycle();
        }
    }

    //
    // Lifecycle events
    //

    /**
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
            setCenterCoordinate((LatLng) savedInstanceState.getParcelable(STATE_CENTER_COORDINATE));
            // need to set zoom level first because of limitation on rotating when zoomed out
            setZoomLevel(savedInstanceState.getDouble(STATE_ZOOM_LEVEL));
            setDirection(savedInstanceState.getDouble(STATE_CENTER_DIRECTION));
            setDirection(savedInstanceState.getDouble(STATE_DIRECTION));
            setZoomEnabled(savedInstanceState.getBoolean(STATE_ZOOM_ENABLED));
            setScrollEnabled(savedInstanceState.getBoolean(STATE_SCROLL_ENABLED));
            setRotateEnabled(savedInstanceState.getBoolean(STATE_ROTATE_ENABLED));
            setDebugActive(savedInstanceState.getBoolean(STATE_DEBUG_ACTIVE));
            setStyleUrl(savedInstanceState.getString(STATE_STYLE_URL));
            setAccessToken(savedInstanceState.getString(STATE_ACCESS_TOKEN));
            List<String> appliedStyleClasses = savedInstanceState.getStringArrayList(STATE_STYLE_CLASSES);
            if (!appliedStyleClasses.isEmpty()) {
                setStyleClasses(appliedStyleClasses);
            }
            mNativeMapView.setDefaultTransitionDuration(
                    savedInstanceState.getLong(STATE_DEFAULT_TRANSITION_DURATION));
            setMyLocationEnabled(savedInstanceState.getBoolean(STATE_MY_LOCATION_ENABLED));

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
        }

        // Force a check for an access token
        validateAccessToken(getAccessToken());

        // Initialize EGL
        mNativeMapView.initializeDisplay();
        mNativeMapView.initializeContext();

        // Add annotation deselection listener
        addOnMapChangedListener(new OnMapChangedListener() {
            @Override
            public void onMapChanged(@MapChange int change) {
                if ((change == REGION_WILL_CHANGE) || (change == REGION_WILL_CHANGE_ANIMATED)) {
                    deselectMarker();
                }

                if (change == DID_FINISH_LOADING_MAP) {
                    reloadSprites();
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
            throw new NullPointerException("outState is null");
        }

        outState.putParcelable(STATE_CENTER_COORDINATE, getCenterCoordinate());
        // need to set zoom level first because of limitation on rotating when zoomed out
        outState.putDouble(STATE_ZOOM_LEVEL, getZoomLevel());
        outState.putDouble(STATE_CENTER_DIRECTION, getDirection());
        outState.putBoolean(STATE_ZOOM_ENABLED, mZoomEnabled);
        outState.putBoolean(STATE_SCROLL_ENABLED, mScrollEnabled);
        outState.putBoolean(STATE_ROTATE_ENABLED, mRotateEnabled);
        outState.putBoolean(STATE_DEBUG_ACTIVE, isDebugActive());
        outState.putString(STATE_STYLE_URL, mStyleUrl);
        outState.putString(STATE_ACCESS_TOKEN, getAccessToken());
        outState.putStringArrayList(STATE_STYLE_CLASSES, new ArrayList<>(getStyleClasses()));
        outState.putLong(STATE_DEFAULT_TRANSITION_DURATION, mNativeMapView.getDefaultTransitionDuration());
        outState.putBoolean(STATE_MY_LOCATION_ENABLED, isMyLocationEnabled());

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
    }

    /**
     * You must call this method from the parent's {@link Activity#onStop()} or {@link Fragment#onStop()}
     */
    @UiThread
    public void onStop() {
        mUserLocationView.cancelAnimations();
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
        mContext.registerReceiver(mConnectivityReceiver, new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));

        mUserLocationView.resume();
        mNativeMapView.resume();
        mNativeMapView.update();
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

    /**
     * Returns the current coordinate at the center of the map view.
     *
     * @return The current coordinate.
     */
    @UiThread
    @NonNull
    public LatLng getCenterCoordinate() {
        return mNativeMapView.getLatLng();
    }

    /**
     * Centers the map on a new coordinate immediately without changing the zoom level.
     * <p/>
     * The initial coordinate is (0, 0).
     * <p/>
     * If you want to animate the change, use {@link MapView#setCenterCoordinate(LatLng, boolean)}.
     *
     * @param centerCoordinate The new coordinate.
     * @see MapView#setCenterCoordinate(LatLng, boolean)
     */
    @UiThread
    public void setCenterCoordinate(@NonNull LatLng centerCoordinate) {
        setCenterCoordinate(centerCoordinate, false);
    }

    /**
     * Centers the map on a new coordinate without changing the zoom level and optionally animates the change.
     * <p/>
     * The initial coordinate is (0, 0).
     *
     * @param centerCoordinate The new coordinate.
     * @param animated         If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    public void setCenterCoordinate(@NonNull LatLng centerCoordinate, boolean animated) {
        if (centerCoordinate == null) {
            throw new NullPointerException("centerCoordinate is null");
        }
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        mNativeMapView.setLatLng(centerCoordinate, duration);
    }


    /**
     * Centers the map on a new coordinate immediately while changing the current zoom level.
     * <p/>
     * The initial value is a center coordinate of (0, 0) and a zoom level of 0.
     * <p/>
     * If you want to animate the change, use {@link MapView#setCenterCoordinate(LatLngZoom, boolean)}.
     *
     * @param centerCoordinate The new coordinate and zoom level.
     * @see MapView#setCenterCoordinate(LatLngZoom, boolean)
     */
    @UiThread
    public void setCenterCoordinate(@NonNull LatLngZoom centerCoordinate) {
        setCenterCoordinate(centerCoordinate, false);
    }

    /**
     * Resets the map to the minimum zoom level, a center coordinate of (0, 0), a true north heading,
     * and animates the change.
     */
    @UiThread
    public void resetPosition() {
        mNativeMapView.cancelTransitions();
        mNativeMapView.resetPosition();
    }

    /**
     * Centers the map on a new coordinate while changing the zoom level and optionally animates the change.
     * <p/>
     * The initial value is a center coordinate of (0, 0) and a zoom level of 0.
     *
     * @param centerCoordinate The new coordinate and zoom level.
     * @param animated         If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    public void setCenterCoordinate(@NonNull LatLngZoom centerCoordinate,
                                    boolean animated) {
        if (centerCoordinate == null) {
            throw new NullPointerException("centerCoordinate is null");
        }
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        mNativeMapView.setLatLngZoom(centerCoordinate, duration);
    }

    /**
     * Returns whether the user may scroll around the map.
     *
     * @return If true, scrolling is enabled.
     */
    @UiThread
    public boolean isScrollEnabled() {
        return mScrollEnabled;
    }

    /**
     * Changes whether the user may scroll around the map.
     * <p/>
     * This setting controls only user interactions with the map. If you set the value to false,
     * you may still change the map location programmatically.
     * <p/>
     * The default value is true.
     *
     * @param scrollEnabled If true, scrolling is enabled.
     */
    @UiThread
    public void setScrollEnabled(boolean scrollEnabled) {
        this.mScrollEnabled = scrollEnabled;
    }

    //
    // Rotation
    //

    /**
     * Returns the current heading of the map relative to true north.
     *
     * @return The current heading measured in degrees.
     */
    @UiThread
    @FloatRange(from = 0, to = 360)
    public double getDirection() {
        double direction = -mNativeMapView.getBearing();

        while (direction > 360) {
            direction -= 360;
        }
        while (direction < 0) {
            direction += 360;
        }

        return direction;
    }

    /**
     * Rotates the map to a new heading relative to true north immediately.
     * <p/>
     * The value 0 means that the top edge of the map view will correspond to true north.<br>
     * The value 90 means the top of the map will point due east.<br>
     * The value 180 means the top of the map will point due south.<br>
     * The value 270 means the top of the map will point due west.
     * <p/>
     * The initial heading is 0.
     * <p/>
     * If you want to animate the change, use {@link MapView#setDirection(double, boolean)}.
     *
     * @param direction The new heading measured in degrees.
     * @see MapView#setDirection(double, boolean)
     */
    @UiThread
    public void setDirection(@FloatRange(from = 0, to = 360) double direction) {
        setDirection(direction, false);
    }

    /**
     * Rotates the map to a new heading relative to true north and optionally animates the change.
     * <p/>
     * The value 0 means that the top edge of the map view will correspond to true north.<br/>
     * The value 90 means the top of the map will point due east.<br/>
     * The value 180 means the top of the map will point due south.<br/>
     * The value 270 means the top of the map will point due west
     * <p/>
     * The initial heading is 0.
     *
     * @param direction The new heading measured in degrees from true north.
     * @param animated  If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    public void setDirection(@FloatRange(from = 0, to = 360) double direction, boolean animated) {
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        // Out of range direactions are normallised in setBearing
        mNativeMapView.setBearing(-direction, duration);
    }

    /**
     * Resets the map heading to true north and animates the change.
     */
    @UiThread
    public void resetNorth() {
        mNativeMapView.cancelTransitions();
        mNativeMapView.resetNorth();
    }

    /**
     * Returns whether the user may rotate the map.
     *
     * @return If true, rotating is enabled.
     */
    @UiThread
    public boolean isRotateEnabled() {
        return mRotateEnabled;
    }

    /**
     * Changes whether the user may rotate the map.
     * <p/>
     * This setting controls only user interactions with the map. If you set the value to false,
     * you may still change the map location programmatically.
     * <p/>
     * The default value is true.
     *
     * @param rotateEnabled If true, rotating is enabled.
     */
    @UiThread
    public void setRotateEnabled(boolean rotateEnabled) {
        this.mRotateEnabled = rotateEnabled;
    }

    //
    // Scale
    //

    /**
     * Returns the current zoom level of the map view.
     *
     * @return The current zoom level.
     */
    @UiThread
    @FloatRange(from = 0.0, to = MAXIMUM_ZOOM_LEVEL)
    public double getZoomLevel() {
        return mNativeMapView.getZoom();
    }

    /**
     * Zooms the map to a new zoom level immediately without changing the center coordinate.
     * <p/>
     * At zoom level 0, tiles cover the entire world map;
     * at zoom level 1, tiles cover &frac14 of the world;
     * at zoom level 2, tiles cover 1/16 of the world, and so on.
     * <p/>
     * The initial zoom level is 0. The maximum zoom level is {@link MapView#MAXIMUM_ZOOM_LEVEL}.
     * <p/>
     * If you want to animate the change, use {@link MapView#setZoomLevel(double, boolean)}.
     *
     * @param zoomLevel The new coordinate.
     * @see MapView#setZoomLevel(double, boolean)
     * @see MapView#MAXIMUM_ZOOM_LEVEL
     */
    @UiThread
    public void setZoomLevel(@FloatRange(from = 0.0, to = MAXIMUM_ZOOM_LEVEL) double zoomLevel) {
        setZoomLevel(zoomLevel, false);
    }

    /**
     * Zooms the map to a new zoom level and optionally animates the change without changing the center coordinate.
     * <p/>
     * At zoom level 0, tiles cover the entire world map;
     * at zoom level 1, tiles cover &frac14 of the world;
     * at zoom level 2, tiles cover 1/16 of the world, and so on.
     * <p/>
     * The initial zoom level is 0. The maximum zoom level is {@link MapView#MAXIMUM_ZOOM_LEVEL}.
     *
     * @param zoomLevel The new coordinate.
     * @param animated  If true, animates the change. If false, immediately changes the map.
     * @see MapView#MAXIMUM_ZOOM_LEVEL
     */
    @UiThread
    public void setZoomLevel(@FloatRange(from = 0.0, to = MAXIMUM_ZOOM_LEVEL) double zoomLevel, boolean animated) {
        if ((zoomLevel < 0.0) || (zoomLevel > MAXIMUM_ZOOM_LEVEL)) {
            throw new IllegalArgumentException("zoomLevel is < 0 or > MapView.MAXIMUM_ZOOM_LEVEL");
        }
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        mNativeMapView.setZoom(zoomLevel, duration);
    }

    /**
     * Returns whether the user may zoom the map.
     *
     * @return If true, zooming is enabled.
     */
    @UiThread
    public boolean isZoomEnabled() {
        return mZoomEnabled;
    }

    /**
     * Changes whether the user may zoom the map.
     * <p/>
     * This setting controls only user interactions with the map. If you set the value to false,
     * you may still change the map location programmatically.
     * <p/>
     * The default value is true.
     *
     * @param zoomEnabled If true, zooming is enabled.
     */
    @UiThread
    public void setZoomEnabled(boolean zoomEnabled) {
        this.mZoomEnabled = zoomEnabled;

        if ((mZoomButtonsController != null) && (getVisibility() == View.VISIBLE) && mZoomEnabled) {
            mZoomButtonsController.setVisible(true);
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
    // Debug
    //

    /**
     * Returns whether the map debug information is currently shown.
     *
     * @return If true, map debug information is currently shown.
     */
    @UiThread
    public boolean isDebugActive() {
        return mNativeMapView.getDebug() || mNativeMapView.getCollisionDebug();
    }

    /**
     * Changes whether the map debug information is shown.
     * <p/>
     * The default value is false.
     *
     * @param debugActive If true, map debug information is shown.
     */
    @UiThread
    public void setDebugActive(boolean debugActive) {
        mNativeMapView.setDebug(debugActive);
        mNativeMapView.setCollisionDebug(debugActive);
    }

    /**
     * Toggles whether the map debug information is shown.
     * <p/>
     * The value of {@link MapView#isDebugActive()} is toggled.
     *
     * @see MapView#isDebugActive()
     */
    @UiThread
    public void toggleDebug() {
        mNativeMapView.toggleDebug();
        mNativeMapView.toggleCollisionDebug();
    }

    // True if map has finished loading the view
    private boolean isFullyLoaded() {
        return mNativeMapView.isFullyLoaded();
    }

    //
    // Styling
    //

    /**
     * Loads a new map style from the specified URL.
     * <p/>
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
     * reads the style from the APK {@code asset/} directory.
     * This is used to load a style bundled with your app.</li>
     * <li>{@code null}: loads the default {@link Style#MAPBOX_STREETS} style.</li>
     * </ul>
     * <p/>
     * This method is asynchronous and will return immediately before the style finishes loading.
     * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
     * <p/>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
     *
     * @param url The URL of the map style
     * @see Style
     */
    @UiThread
    public void setStyleUrl(@Nullable String url) {
        if (url == null) {
            url = Style.MAPBOX_STREETS;
        }
        mStyleUrl = url;
        mNativeMapView.setStyleUrl(url);
    }

    /**
     * Loads a new map style from the specified bundled style.
     * <p/>
     * This method is asynchronous and will return immediately before the style finishes loading.
     * If you wish to wait for the map to finish loading listen for the {@link MapView#DID_FINISH_LOADING_MAP} event.
     * <p/>
     * If the style fails to load or an invalid style URL is set, the map view will become blank.
     * An error message will be logged in the Android logcat and {@link MapView#DID_FAIL_LOADING_MAP} event will be sent.
     *
     * @param style The bundled style. Accepts one of the values from {@link Style}.
     */
    @UiThread
    public void setStyle(@Style.StyleUrl String style) {
        setStyleUrl(style);
    }

    /**
     * Returns the map style currently displayed in the map view.
     * <p/>
     * If the default style is currently displayed, a URL will be returned instead of null.
     *
     * @return The URL of the map style.
     */
    @UiThread
    @NonNull
    public String getStyleUrl() {
        return mStyleUrl;
    }

    /**
     * Returns the set of currently active map style classes.
     *
     * @return A list of class identifiers.
     */
    @UiThread
    @NonNull
    public List<String> getStyleClasses() {
        return Collections.unmodifiableList(mNativeMapView.getClasses());
    }

    /**
     * Changes the set of currently active map style classes immediately.
     * <p/>
     * The list of valid class identifiers is defined by the currently loaded map style.
     * <p/>
     * If you want to animate the change, use {@link MapView#setStyleClasses(List, long)}.
     *
     * @param styleClasses A list of class identifiers.
     * @see MapView#setStyleClasses(List, long)
     * @see MapView#setStyleUrl(String)
     */
    @UiThread
    public void setStyleClasses(@NonNull List<String> styleClasses) {
        setStyleClasses(styleClasses, 0);
    }

    /**
     * Changes the set of currently active map style classes with an animated transition.
     * <p/>
     * The list of valid class identifiers is defined by the currently loaded map style.
     *
     * @param styleClasses       A list of class identifiers.
     * @param transitionDuration The duration of the transition animation in milliseconds.
     * @see MapView#setStyleClasses(List, long)
     * @see MapView#setStyleUrl(String)
     */
    @UiThread
    public void setStyleClasses(@NonNull List<String> styleClasses, @IntRange(from = 0) long transitionDuration) {
        if (styleClasses == null) {
            throw new NullPointerException("styleClasses is null");
        }
        if (transitionDuration < 0) {
            throw new IllegalArgumentException("transitionDuration is < 0");
        }
        // TODO non negative check and annotation (go back and check other functions too)
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        mNativeMapView.setClasses(styleClasses);
    }

    /**
     * Activates the specified map style class.
     * <p/>
     * If you want to animate the change, use {@link MapView#setStyleClasses(List, long)}.
     *
     * @param styleClass The class identifier.
     * @see MapView#setStyleClasses(List, long)
     */
    @UiThread
    public void addStyleClass(@NonNull String styleClass) {
        if (styleClass == null) {
            throw new NullPointerException("styleClass is null");
        }
        mNativeMapView.addClass(styleClass);
    }

    /**
     * Deactivates the specified map style class.
     * <p/>
     * If you want to animate the change, use {@link MapView#setStyleClasses(List, long)}.
     *
     * @param styleClass The class identifier.
     * @see MapView#setStyleClasses(List, long)
     */
    @UiThread
    public void removeStyleClass(@NonNull String styleClass) {
        if (styleClass == null) {
            throw new NullPointerException("styleClass is null");
        }
        mNativeMapView.removeClass(styleClass);
    }

    /**
     * Returns whether the specified map style class is currently active.
     *
     * @param styleClass The class identifier.
     * @return If true, the class is currently active.
     */
    @UiThread
    public boolean hasStyleClass(@NonNull String styleClass) {
        if (styleClass == null) {
            throw new NullPointerException("styleClass is null");
        }
        return mNativeMapView.hasClass(styleClass);
    }

    /**
     * Deactivates all the currently active map style classes immediately.
     * <p/>
     * If you want to animate the change, use {@link MapView#removeAllStyleClasses(long)}.
     *
     * @see MapView#removeAllStyleClasses(long)
     */
    @UiThread
    public void removeAllStyleClasses() {
        removeAllStyleClasses(0);
    }

    /**
     * Deactivates all the currently active map style classes with an animated transition.
     *
     * @param transitionDuration The duration of the transition animation in milliseconds.
     */
    @UiThread
    public void removeAllStyleClasses(@IntRange(from = 0) long transitionDuration) {
        if (transitionDuration < 0) {
            throw new IllegalArgumentException("transitionDuration is < 0");
        }
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        ArrayList<String> styleClasses = new ArrayList<>(0);
        setStyleClasses(styleClasses);
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
     * Sets the current Mapbox access token used to load map styles and tiles.
     * <p/>
     * You must set a valid access token before you call {@link MapView#onCreate(Bundle)}
     * or an exception will be thrown.
     * <p/>
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
    public String getAccessToken() {
        return mNativeMapView.getAccessToken();
    }

    //
    // Projection
    //

    /**
     * Converts a point in this view's coordinate system to a map coordinate.
     *
     * @param point A point in this view's coordinate system.
     * @return The converted map coordinate.
     */
    @UiThread
    @NonNull
    public LatLng fromScreenLocation(@NonNull PointF point) {
        if (point == null) {
            throw new NullPointerException("point is null");
        }

        float x = point.x;
        float y = point.y;

        // flip y direction vertically to match core GL
        y = getHeight() - y;

        return mNativeMapView.latLngForPixel(new PointF(x / mScreenDensity, y / mScreenDensity));
    }

    /**
     * Converts a map coordinate to a point in this view's coordinate system.
     *
     * @param location A map coordinate.
     * @return The converted point in this view's coordinate system.
     */
    @UiThread
    @NonNull
    public PointF toScreenLocation(@NonNull LatLng location) {
        if (location == null) {
            throw new NullPointerException("location is null");
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

    public SpriteFactory getSpriteFactory() {
        if (mSpriteFactory == null) {
            mSpriteFactory = new SpriteFactory(this);
        }
        return mSpriteFactory;
    }

    private void loadSprite(Sprite sprite) {
        Bitmap bitmap = sprite.getBitmap();
        String id = sprite.getId();
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

        mNativeMapView.setSprite(
                id,
                (int) (bitmap.getWidth() / scale),
                (int) (bitmap.getHeight() / scale),
                scale, buffer.array());
    }

    private void reloadSprites() {
        int count = mSprites.size();
        for (int i = 0; i < count; i++) {
            Sprite sprite = mSprites.get(i);
            loadSprite(sprite);
        }
    }

    private Marker prepareMarker(MarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Sprite icon = marker.getIcon();
        if (icon == null) {
            icon = getSpriteFactory().defaultMarker();
            marker.setIcon(icon);
        }
        if (!mSprites.contains(icon)) {
            mSprites.add(icon);
            loadSprite(icon);
        } else {
            Sprite oldSprite = mSprites.get(mSprites.indexOf(icon));
            if (!oldSprite.getBitmap().sameAs(icon.getBitmap())) {
                throw new SpriteBitmapChangedException();
            }
        }
        marker.setTopOffsetPixels(getTopOffsetPixelsForSprite(icon));
        return marker;
    }

    /**
     * Adds a marker to this map.
     * <p/>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptions A marker options object that defines how to render the marker.
     * @return The {@code Marker} that was added to the map.
     */
    @UiThread
    @NonNull
    public Marker addMarker(@NonNull MarkerOptions markerOptions) {
        if (markerOptions == null) {
            throw new NullPointerException("markerOptions is null");
        }

        Marker marker = prepareMarker(markerOptions);
        long id = mNativeMapView.addMarker(marker);
        marker.setId(id);        // the annotation needs to know its id
        marker.setMapView(this); // the annotation needs to know which map view it is in
        mAnnotations.add(marker);
        return marker;
    }

    /**
     * Adds multiple markers to this map.
     * <p/>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptionsList A list of marker options objects that defines how to render the markers.
     * @return A list of the {@code Marker}s that were added to the map.
     */
    @UiThread
    @NonNull
    public List<Marker> addMarkers(@NonNull List<MarkerOptions> markerOptionsList) {
        if (markerOptionsList == null) {
            throw new NullPointerException("markerOptionsList is null");
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

    /**
     * Adds a polyline to this map.
     *
     * @param polylineOptions A polyline options object that defines how to render the polyline.
     * @return The {@code Polyine} that was added to the map.
     */
    @UiThread
    @NonNull
    public Polyline addPolyline(@NonNull PolylineOptions polylineOptions) {
        if (polylineOptions == null) {
            throw new NullPointerException("polylineOptions is null");
        }

        Polyline polyline = polylineOptions.getPolyline();
        long id = mNativeMapView.addPolyline(polyline);
        polyline.setId(id);
        polyline.setMapView(this);
        mAnnotations.add(polyline);
        return polyline;
    }

    /**
     * Adds multiple polylines to this map.
     *
     * @param polylineOptionsList A list of polyline options objects that defines how to render the polylines.
     * @return A list of the {@code Polyline}s that were added to the map.
     */
    @UiThread
    @NonNull
    public List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList) {
        if (polylineOptionsList == null) {
            throw new NullPointerException("polylineOptionsList is null");
        }

        // TODO make faster in JNI
        int count = polylineOptionsList.size();
        List<Polyline> polylines = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            PolylineOptions polylineOptions = polylineOptionsList.get(i);
            polylines.add(addPolyline(polylineOptions));
        }

        return new ArrayList<>(polylines);
    }

    /**
     * Adds a polygon to this map.
     *
     * @param polygonOptions A polygon options object that defines how to render the polygon.
     * @return The {@code Polygon} that was added to the map.
     */
    @UiThread
    @NonNull
    public Polygon addPolygon(@NonNull PolygonOptions polygonOptions) {
        if (polygonOptions == null) {
            throw new NullPointerException("polygonOptions is null");
        }

        Polygon polygon = polygonOptions.getPolygon();
        long id = mNativeMapView.addPolygon(polygon);
        polygon.setId(id);
        polygon.setMapView(this);
        mAnnotations.add(polygon);
        return polygon;
    }


    /**
     * Adds multiple polygons to this map.
     *
     * @param polygonOptionsList A list of polygon options objects that defines how to render the polygons.
     * @return A list of the {@code Polygon}s that were added to the map.
     */
    @UiThread
    @NonNull
    public List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList) {
        if (polygonOptionsList == null) {
            throw new NullPointerException("polygonOptionsList is null");
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

    /**
     * Removes an annotation from the map.
     *
     * @param annotation The annotation object to remove.
     */
    @UiThread
    public void removeAnnotation(@NonNull Annotation annotation) {
        if (annotation == null) {
            throw new NullPointerException("annotation is null");
        }

        if (annotation instanceof Marker) {
            ((Marker) annotation).hideInfoWindow();
        }
        long id = annotation.getId();
        mNativeMapView.removeAnnotation(id);
        mAnnotations.remove(annotation);
    }

    /**
     * Removes multiple annotations from the map.
     *
     * @param annotationList A list of annotation objects to remove.
     */
    @UiThread
    public void removeAnnotations(@NonNull List<Annotation> annotationList) {
        if (annotationList == null) {
            throw new NullPointerException("annotationList is null");
        }

        // TODO make faster in JNI
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            removeAnnotation(annotation);
        }
    }

    /**
     * Removes all annotations from the map.
     */
    @UiThread
    public void removeAllAnnotations() {
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

    /**
     * Returns a list of all the annotations on the map.
     *
     * @return A list of all the annotation objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
    @NonNull
    public List<Annotation> getAllAnnotations() {
        return new ArrayList<>(mAnnotations);
    }

    private List<Marker> getMarkersInBounds(@NonNull BoundingBox bbox) {
        if (bbox == null) {
            throw new NullPointerException("bbox is null");
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

    private int getTopOffsetPixelsForSprite(Sprite sprite) {
        // This method will dead lock if map paused. Causes a freeze if you add a marker in an
        // activity's onCreate()
        if (mNativeMapView.isPaused()) {
            return 0;
        }

        return (int) (mNativeMapView.getTopOffsetPixelsForAnnotationSymbol(sprite.getId())
                * mScreenDensity);
    }

    /**
     * Returns the distance spanned by one pixel at the specified latitude and current zoom level.
     * <p/>
     * The distance between pixels decreases as the latitude approaches the poles.
     * This relationship parallels the relationship between longitudinal coordinates at different latitudes.
     *
     * @param latitude The latitude for which to return the value.
     * @return The distance measured in meters.
     */
    @UiThread
    public double getMetersPerPixelAtLatitude(@FloatRange(from = -180, to = 180) double latitude) {
        return mNativeMapView.getMetersPerPixelAtLatitude(latitude, getZoomLevel()) / mScreenDensity;
    }

    /**
     * Selects a marker. The selected marker will have it's info window opened.
     * Any other open info windows will be closed.
     * <p/>
     * Selecting an already selected marker will have no effect.
     *
     * @param marker The marker to select.
     */
    @UiThread
    public void selectMarker(@NonNull Marker marker) {
        if (marker == null) {
            throw new NullPointerException("marker is null");
        }

        if (marker == mSelectedMarker) {
            return;
        }

        // Need to deselect any currently selected annotation first
        deselectMarker();

        boolean handledDefaultClick = false;
        if (mOnMarkerClickListener != null) {
            // end developer has provided a custom click listener
            handledDefaultClick = mOnMarkerClickListener.onMarkerClick(marker);
        }

        if (!handledDefaultClick) {
            // default behaviour
            // Can't do this as InfoWindow will get hidden
            //setCenterCoordinate(marker.getPosition(), true);
            marker.showInfoWindow();
        }

        mSelectedMarker = marker;
    }

    /**
     * Deselects any currently selected marker. The selected marker will have it's info window closed.
     */
    @UiThread
    public void deselectMarker() {
        if (mSelectedMarker != null) {
            if (mSelectedMarker.isInfoWindowShown()) {
                mSelectedMarker.hideInfoWindow();
                mSelectedMarker = null;
            }
        }
    }
    
    //
    // Camera
    //

    /**
     * Changes the map's viewport to fit the given coordinate bounds.
     *
     * @param bounds The bounds that the viewport will show in its entirety.
     */
    @UiThread
    public void setVisibleCoordinateBounds(@NonNull CoordinateBounds bounds) {
        setVisibleCoordinateBounds(bounds, false);
    }

    /**
     * Changes the map's viewing area to fit the given coordinate bounds, optionally animating the change.
     *
     * @param bounds The bounds that the viewport will show in its entirety.
     * @param animated If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    public void setVisibleCoordinateBounds(@NonNull CoordinateBounds bounds, boolean animated) {
        setVisibleCoordinateBounds(bounds, new RectF(), animated);
    }

    /**
     * Changes the map’s viewport to fit the given coordinate bounds, optionally some additional padding on each side
     * and animating the change.
     *
     * @param bounds The bounds that the viewport will show in its entirety.
     * @param padding The minimum padding (in pixels) that will be visible around the given coordinate bounds.
     * @param animated If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    public void setVisibleCoordinateBounds(@NonNull CoordinateBounds bounds, @NonNull RectF padding, boolean animated) {
        LatLng[] coordinates = {
                new LatLng(bounds.getNe().getLatitude(), bounds.getSw().getLongitude()),
                bounds.getSw(),
                new LatLng(bounds.getSw().getLatitude(), bounds.getNe().getLongitude()),
                bounds.getNe()

        };
        setVisibleCoordinateBounds(coordinates, padding, animated);
    }

    /**
     * Changes the map’s viewport to fit the given coordinates, optionally some additional padding on each side
     * and animating the change.
     *
     * @param coordinates The coordinates that the viewport will show.
     * @param padding The minimum padding (in pixels) that will be visible around the given coordinate bounds.
     * @param animated If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    public void setVisibleCoordinateBounds(@NonNull LatLng[] coordinates, @NonNull RectF padding, boolean animated) {
        setVisibleCoordinateBounds(coordinates, padding, getDirection(), animated);
    }

    private void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, boolean animated) {
        setVisibleCoordinateBounds(coordinates, padding, direction, animated ? ANIMATION_DURATION : 0l);
    }

    private void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
        mNativeMapView.setVisibleCoordinateBounds(coordinates, new RectF(padding.left / mScreenDensity,
                padding.top / mScreenDensity, padding.right / mScreenDensity, padding.bottom / mScreenDensity),
                direction, duration);
    }

    /**
     * Gets the currently selected marker.
     * @return The currently selected marker.
     */
    @UiThread
    @Nullable
    public Marker getSelectedMarker() {
        return mSelectedMarker;
    }

    private void adjustTopOffsetPixels() {
        int count = mAnnotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = mAnnotations.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.setTopOffsetPixels(
                        getTopOffsetPixelsForSprite(marker.getIcon()));
            }
        }

        if (mSelectedMarker != null) {
            if (mSelectedMarker.isInfoWindowShown()) {
                Marker temp = mSelectedMarker;
                temp.hideInfoWindow();
                temp.showInfoWindow();
                mSelectedMarker = temp;
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
        if (!mNativeMapView.isPaused()) {
            mNativeMapView.renderSync();
        }
        super.onDraw(canvas);
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
        }
    }

    // Used by UserLocationView
    void update() {
        mNativeMapView.update();
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
        if (mZoomButtonsController != null) {
            mZoomButtonsController.setVisible(false);
        }
    }

    // Called when view is hidden and shown
    @Override
    protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
        // Required by ZoomButtonController (from Android SDK documentation)
        if ((mZoomButtonsController != null) && (visibility != View.VISIBLE)) {
            mZoomButtonsController.setVisible(false);
        }
        if ((mZoomButtonsController != null) && (visibility == View.VISIBLE)
                && mZoomEnabled) {
            mZoomButtonsController.setVisible(true);
        }
    }

    //
    // Touch events
    //

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
                boolean inProgress = mRotateGestureDetector.isInProgress() || mScaleGestureDetector.isInProgress();

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
        public boolean onDown(MotionEvent e) {
            // Show the zoom controls
            if ((mZoomButtonsController != null) && mZoomEnabled) {
                mZoomButtonsController.setVisible(true);
            }

            return true;
        }

        // Called for double taps
        @Override
        public boolean onDoubleTap(MotionEvent e) {
            if (!mZoomEnabled) {
                return false;
            }

            // Single finger double tap
            // Zoom in
            zoom(true, e.getX(), e.getY());
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

            float toleranceWidth = 40 * mScreenDensity;
            float toleranceHeight = 60 * mScreenDensity;

            RectF tapRect = new RectF(tapPoint.x - toleranceWidth / 2, tapPoint.y - 2 * toleranceHeight / 3,
                    tapPoint.x + toleranceWidth / 2, tapPoint.y + 1 * toleranceHeight / 3);

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
                    // the selection candidates haven't changed; cycle through them
                    if (mSelectedMarker != null && (mSelectedMarker.getId() == mMarkersNearLastTap.get(mMarkersNearLastTap.size() - 1).getId())) {
                        // the selected marker is the last in the set; cycle back to the first
                        // note: this could be the selected marker if only one in set
                        newSelectedMarkerId = mMarkersNearLastTap.get(0).getId();
                    } else if (mSelectedMarker != null) {
                        // otherwise increment the selection through the candidates
                        long result = mMarkersNearLastTap.indexOf(mSelectedMarker);
                        newSelectedMarkerId = mMarkersNearLastTap.get((int) result + 1).getId();
                    } else {
                        // no current selection; select the first one
                        newSelectedMarkerId = mMarkersNearLastTap.get(0).getId();
                    }
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
                            if (mSelectedMarker == null || annotation.getId() != mSelectedMarker.getId()) {
                                selectMarker((Marker) annotation);
                            }
                            break;
                        }
                    }
                }

            } else {
                // deselect any selected marker
                deselectMarker();

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
            if (mOnMapLongClickListener != null) {
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
            // TODO Google Maps also has a rotate and zoom fling

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
        // TODO use MoveGestureDetector
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (!mScrollEnabled) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions(); // TODO need to test canceling
            // transitions with touch

            // Scroll the map
            mNativeMapView.moveBy(-distanceX / mScreenDensity, -distanceY / mScreenDensity);

            if (mOnScrollListener != null) {
                mOnScrollListener.onScroll();
            }

            return true;
        }
    }

    // This class handles two finger gestures
    private class ScaleGestureListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {

        long mBeginTime = 0;
        float mScaleFactor = 1.0f;
        boolean mStarted = false;

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
            mStarted = false;
        }

        // Called each time one of the two fingers moves
        // Called for pinch zooms
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            if (!mZoomEnabled) {
                return false;
            }

            // If scale is large enough ignore a tap
            // TODO: Google Maps seem to use a velocity rather than absolute
            // value?
            mScaleFactor *= detector.getScaleFactor();
            if ((mScaleFactor > 1.05f) || (mScaleFactor < 0.95f)) {
                mStarted = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small scales
            long time = detector.getEventTime();
            long interval = time - mBeginTime;
            if (!mStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            // TODO complex decision between rotate or scale or both (see Google
            // Maps app)

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Scale the map
            mNativeMapView.scaleBy(detector.getScaleFactor(), detector.getFocusX() / mScreenDensity, detector.getFocusY() / mScreenDensity);

            return true;
        }
    }

    // This class handles two finger rotate gestures
    // TODO need way to single finger rotate - need to research how google maps
    // does this - for phones with single touch, or when using mouse etc
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
            // TODO: Google Maps seem to use a velocity rather than absolute
            // value, up to a point then they always rotate
            mTotalAngle += detector.getRotationDegreesDelta();
            if ((mTotalAngle > 5.0f) || (mTotalAngle < -5.0f)) {
                mStarted = true;
            }

            // Ignore short touches in case it is a tap
            // Also ignore small rotate
            long time = detector.getEventTime();
            long interval = time - mBeginTime;
            if (!mStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                return false;
            }

            // TODO complex decision between rotate or scale or both (see Google
            // Maps app). It seems if you start one or the other it takes more
            // to start the other too. Haven't figured out what it uses to
            // decide when to transition to both at the same time.

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Rotate the map
            double bearing = mNativeMapView.getBearing();
            bearing += detector.getRotationDegreesDelta();
            mNativeMapView.setBearing(bearing, detector.getFocusX() / mScreenDensity, detector.getFocusY() / mScreenDensity);

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
                if ((mZoomButtonsController != null) && mZoomEnabled) {
                    mZoomButtonsController.setVisible(true);
                }
                return true;

            case MotionEvent.ACTION_HOVER_EXIT:
                // Hide the zoom controls
                if (mZoomButtonsController != null) {
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

    /**
     * Add a callback that's invoked when the displayed map view changes.
     * <p/>
     * To remove the callback, use {@link MapView#removeOnMapChangedListener(OnMapChangedListener)}.
     *
     * @param listener The callback that's invoked on every frame rendered to the map view.
     * @see MapView#removeOnMapChangedListener(OnMapChangedListener)
     */
    @UiThread
    public void addOnMapChangedListener(@Nullable OnMapChangedListener listener) {
        if (listener != null) {
            mOnMapChangedListener.add(listener);
        }
    }

    /**
     * Remove a callback added with {@link MapView#addOnMapChangedListener(OnMapChangedListener)}
     *
     * @param listener The previously added callback to remove.
     * @see MapView#addOnMapChangedListener(OnMapChangedListener)
     */
    @UiThread
    public void removeOnMapChangedListener(@Nullable OnMapChangedListener listener) {
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

    /**
     * Sets a custom renderer for the contents of info window.
     * <p/>
     * When set your callback is invoked when an info window is about to be shown. By returning
     * a custom {@link View}, the default info window will be replaced.
     *
     * @param infoWindowAdapter The callback to be invoked when an info window will be shown.
     *                          To unset the callback, use null.
     */
    @UiThread
    public void setInfoWindowAdapter(@Nullable InfoWindowAdapter infoWindowAdapter) {
        mInfoWindowAdapter = infoWindowAdapter;
    }

    /**
     * Gets the callback to be invoked when an info window will be shown.
     *
     * @return The callback to be invoked when an info window will be shown.
     */
    @UiThread
    @Nullable
    public InfoWindowAdapter getInfoWindowAdapter() {
        return mInfoWindowAdapter;
    }



    /**
     * Sets a callback that's invoked on every frame rendered to the map view.
     *
     * @param listener The callback that's invoked on every frame rendered to the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnFpsChangedListener(@Nullable OnFpsChangedListener listener) {
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

    /**
     * Sets a callback that's invoked when the map is scrolled.
     *
     * @param listener The callback that's invoked when the map is scrolled.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnScrollListener(@Nullable OnScrollListener listener) {
        mOnScrollListener = listener;
    }

    /**
     * Sets a callback that's invoked when the map is flinged.
     *
     * @param listener The callback that's invoked when the map is flinged.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnFlingListener(@Nullable OnFlingListener listener) {
        mOnFlingListener = listener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on the map view.
     *
     * @param listener The callback that's invoked when the user clicks on the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMapClickListener(@Nullable OnMapClickListener listener) {
        mOnMapClickListener = listener;
    }

    /**
     * Sets a callback that's invoked when the user long clicks on the map view.
     *
     * @param listener The callback that's invoked when the user long clicks on the map view.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMapLongClickListener(@Nullable OnMapLongClickListener listener) {
        mOnMapLongClickListener = listener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on a marker.
     *
     * @param listener The callback that's invoked when the user clicks on a marker.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMarkerClickListener(@Nullable OnMarkerClickListener listener) {
        mOnMarkerClickListener = listener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on an info window.
     *
     * @return The callback that's invoked when the user clicks on an info window.
     */
    @UiThread
    @Nullable
    public OnInfoWindowClickListener getOnInfoWindowClickListener() {
        return mOnInfoWindowClickListener;
    }

    /**
     * Sets a callback that's invoked when the user clicks on an info window.
     *
     * @param listener The callback that's invoked when the user clicks on an info window.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnInfoWindowClickListener(@Nullable OnInfoWindowClickListener listener) {
        mOnInfoWindowClickListener = listener;
    }

    //
    // User location
    //

    /**
     * Returns the status of the my-location layer.
     *
     * @return True if the my-location layer is enabled, false otherwise.
     */
    @UiThread
    public boolean isMyLocationEnabled() {
        return mUserLocationView.isEnabled();
    }

    /**
     * Enables or disables the my-location layer.
     * While enabled, the my-location layer continuously draws an indication of a user's current
     * location and bearing.
     * <p/>
     * In order to use the my-location-layer feature you need to request permission for either
     * {@link android.Manifest.permission#ACCESS_COARSE_LOCATION}
     * or @link android.Manifest.permission#ACCESS_FINE_LOCATION.
     *
     * @param enabled True to enable; false to disable.
     */
    @UiThread
    public void setMyLocationEnabled(boolean enabled) {
        mUserLocationView.setEnabled(enabled);
    }

    /**
     * Returns the currently displayed user location, or null if there is no location data available.
     *
     * @return The currently displayed user location.
     */
    @UiThread
    @Nullable
    public Location getMyLocation() {
        return mUserLocationView.getLocation();
    }

    /**
     * Sets a callback that's invoked when the the My Location dot
     * (which signifies the user's location) changes location.
     *
     * @param listener The callback that's invoked when the user clicks on a marker.
     *                 To unset the callback, use null.
     */
    @UiThread
    public void setOnMyLocationChangeListener(@Nullable OnMyLocationChangeListener listener) {
        mUserLocationView.setOnMyLocationChangeListener(listener);
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
    public boolean isCompassEnabled() {
        return mCompassView.isEnabled();
    }

    /**
     * Enables or disables the compass. The compass is an icon on the map that indicates the
     * direction of north on the map. When a user clicks
     * the compass, the camera orients itself to its default orientation and fades away shortly
     * after. If disabled, the compass will never be displayed.
     * <p/>
     * By default, the compass is enabled.
     *
     * @param compassEnabled True to enable the compass; false to disable the compass.
     */
    @UiThread
    public void setCompassEnabled(boolean compassEnabled) {
        mCompassView.setEnabled(compassEnabled);
    }

    /**
     * Sets the gravity of the compass view. Use this to change the corner of the map view that the
     * compass is displayed in.
     * <p/>
     * By default, the compass is in the top right corner.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
        @UiThread
        public void setCompassGravity(int gravity) {
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
    public void setCompassMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mCompassView, left, top, right, bottom);
    }

    //
    // Logo
    //

    /**
     * Sets the gravity of the logo view. Use this to change the corner of the map view that the
     * Mapbox logo is displayed in.
     * <p/>
     * By default, the logo is in the bottom left corner.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
    @UiThread
    public void setLogoGravity(int gravity) {
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
    public void setLogoMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mLogoView, left, top, right, bottom);
    }

    /**
     * Enables or disables the Mapbox logo.
     * <p/>
     * By default, the compass is enabled.
     *
     * @param visibility True to enable the logo; false to disable the logo.
     */
    @UiThread
    public void setLogoVisibility(int visibility) {
        mLogoView.setVisibility(visibility);
    }

    //
    // Attribution
    //

    /**
     * Sets the gravity of the attribution button view. Use this to change the corner of the map
     * view that the attribution button is displayed in.
     * <p/>
     * By default, the attribution button is in the bottom left corner.
     *
     * @param gravity One of the values from {@link Gravity}.
     * @see Gravity
     */
    @UiThread
    public void setAttributionGravity(int gravity) {
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
    public void setAttributionMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mAttributionsView, left, top, right, bottom);
    }

    /**
     * Enables or disables the attribution button. The attribution is a button with an "i" than when
     * clicked shows a menu with copyright and legal notices. The menu also inlcudes the "Improve
     * this map" link which user can report map errors with.
     * <p/>
     * By default, the attribution button is enabled.
     *
     * @param visibility True to enable the attribution button; false to disable the attribution button.
     */
    @UiThread
    public void setAttributionVisibility(int visibility) {
        mAttributionsView.setVisibility(visibility);
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
}
