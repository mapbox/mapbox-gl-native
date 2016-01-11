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
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.FloatRange;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresPermission;
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
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.exceptions.IconBitmapChangedException;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.layers.CustomLayer;
import com.mapbox.mapboxsdk.utils.ApiAccess;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

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
public class MapView extends FrameLayout {

    private static final String TAG = "MapView";
    private static final float DIMENSION_SEVEN_DP = 7f;
    private static final float DIMENSION_TEN_DP = 10f;
    private static final float DIMENSION_SIXTEEN_DP = 16f;
    private static final float DIMENSION_SEVENTYSIX_DP = 76f;

    private MapboxMap mMapboxMap;
    private List<Annotation> mAnnotations;
    private List<Icon> mIcons;

    private NativeMapView mNativeMapView;
    private CompassView mCompassView;
    private ImageView mLogoView;
    private ImageView mAttributionsView;
    private UserLocationView mUserLocationView;

    private List<OnMapChangedListener> mOnMapChangedListener;
    private ZoomButtonsController mZoomButtonsController;
    private ConnectivityReceiver mConnectivityReceiver;
    private float mScreenDensity = 1.0f;

    private TrackballLongPressTimeOut mCurrentTrackballLongPressTimeOut;
    private GestureDetectorCompat mGestureDetector;
    private ScaleGestureDetector mScaleGestureDetector;
    private RotateGestureDetector mRotateGestureDetector;
    private ShoveGestureDetector mShoveGestureDetector;
    private boolean mTwoTap = false;
    private boolean mZoomStarted = false;
    private boolean mQuickZoom = false;

  /*
    private int mContentPaddingLeft;
    private int mContentPaddingTop;
    private int mContentPaddingRight;
    private int mContentPaddingBottom;
*/

    @UiThread
    public MapView(@NonNull Context context) {
        super(context);
        initialize(context, null);
    }

    @UiThread
    public MapView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        initialize(context, attrs);
    }

    @UiThread
    public MapView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context, attrs);
    }

    private void initialize(@NonNull Context context, @Nullable AttributeSet attrs) {
        mMapboxMap = new MapboxMap(this);
        mAnnotations = new ArrayList<>();
        mIcons = new ArrayList<>();
        mOnMapChangedListener = new ArrayList<>();

        View view = LayoutInflater.from(context).inflate(R.layout.mapview_internal, this);

        if (!isInEditMode()) {
            setWillNotDraw(false);
        }

        // Reference the TextureView
        TextureView textureView = (TextureView) view.findViewById(R.id.textureView);
        textureView.setSurfaceTextureListener(new SurfaceTextureListener());

        // Check if we are in Android Studio UI editor to avoid error in layout preview
        if (isInEditMode()) {
            return;
        }

        mNativeMapView = new NativeMapView(this);

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
            mMapboxMap.setZoomControlsEnabled(true);
        }
        mZoomButtonsController = new ZoomButtonsController(this);
        mZoomButtonsController.setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
        mZoomButtonsController.setOnZoomListener(new OnZoomListener());

        // Connectivity
        onConnectivityChanged(isConnected());

        mUserLocationView = (UserLocationView) view.findViewById(R.id.userLocationView);
        mUserLocationView.setMapView(this);
        mCompassView = (CompassView) view.findViewById(R.id.compassView);
        mCompassView.setOnClickListener(new CompassView.CompassClickListener(this));
        mLogoView = (ImageView) view.findViewById(R.id.logoView);

        // Setup Attributions control
        mAttributionsView = (ImageView) view.findViewById(R.id.attributionView);
        mAttributionsView.setOnClickListener(new AttributionOnClickListener(this));

        mScreenDensity = context.getResources().getDisplayMetrics().density;

        // Load the attributes
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.MapView, 0, 0);
        try {

            // Debug mode
            mMapboxMap.setDebugActive(typedArray.getBoolean(R.styleable.MapView_debug_active, false));

            // Move camera
            CameraPosition cameraPosition = new CameraPosition.Builder(typedArray).build();
            mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));

            // Access token
            if (typedArray.getString(R.styleable.MapView_access_token) != null) {
                setAccessToken(typedArray.getString(R.styleable.MapView_access_token));
            }

            // Style url
            if (typedArray.getString(R.styleable.MapView_style_url) != null) {
                mMapboxMap.setStyleUrl(typedArray.getString(R.styleable.MapView_style_url));
            }

            // Enable gestures
            mMapboxMap.setZoomEnabled(typedArray.getBoolean(R.styleable.MapView_zoom_enabled, true));
            mMapboxMap.setScrollEnabled(typedArray.getBoolean(R.styleable.MapView_scroll_enabled, true));
            mMapboxMap.setRotateEnabled(typedArray.getBoolean(R.styleable.MapView_rotate_enabled, true));
            mMapboxMap.setTiltEnabled(typedArray.getBoolean(R.styleable.MapView_tilt_enabled, true));
            mMapboxMap.setZoomControlsEnabled(typedArray.getBoolean(R.styleable.MapView_zoom_controls_enabled, mMapboxMap.isZoomControlsEnabled()));

            // Compass
            mMapboxMap.setCompassEnabled(typedArray.getBoolean(R.styleable.MapView_compass_enabled, true));
            mMapboxMap.setCompassGravity(typedArray.getInt(R.styleable.MapView_compass_gravity, Gravity.TOP | Gravity.END));
            setWidgetMargins(mCompassView, typedArray.getDimension(R.styleable.MapView_compass_margin_left, DIMENSION_TEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_compass_margin_top, DIMENSION_TEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_compass_margin_right, DIMENSION_TEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_compass_margin_bottom, DIMENSION_TEN_DP));

            // Logo
            mMapboxMap.setLogoVisibility(typedArray.getInt(R.styleable.MapView_logo_visibility, View.VISIBLE));
            mMapboxMap.setLogoGravity(typedArray.getInt(R.styleable.MapView_logo_gravity, Gravity.BOTTOM | Gravity.START));
            setWidgetMargins(mLogoView, typedArray.getDimension(R.styleable.MapView_logo_margin_left, DIMENSION_SIXTEEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_logo_margin_top, DIMENSION_SIXTEEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_logo_margin_right, DIMENSION_SIXTEEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_logo_margin_bottom, DIMENSION_SIXTEEN_DP));

            // Attribution
            mMapboxMap.setAttributionVisibility(typedArray.getInt(R.styleable.MapView_attribution_visibility, View.VISIBLE));
            mMapboxMap.setAttributionGravity(typedArray.getInt(R.styleable.MapView_attribution_gravity, Gravity.BOTTOM));
            setWidgetMargins(mAttributionsView, typedArray.getDimension(R.styleable.MapView_attribution_margin_left, DIMENSION_SEVENTYSIX_DP)
                    , typedArray.getDimension(R.styleable.MapView_attribution_margin_top, DIMENSION_SEVEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_attribution_margin_right, DIMENSION_SEVEN_DP)
                    , typedArray.getDimension(R.styleable.MapView_attribution_margin_bottom, DIMENSION_SEVEN_DP));

            // User location
            try {
                //noinspection ResourceType
                mMapboxMap.setMyLocationEnabled(typedArray.getBoolean(R.styleable.MapView_my_location_enabled, false));
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

            // Get previous camera position
            CameraPosition cameraPosition = savedInstanceState.getParcelable(MapboxConstants.STATE_CAMERA_POSITION);
            if (cameraPosition != null) {
                mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
            }
            mMapboxMap.setZoomEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_ENABLED));
            mMapboxMap.setScrollEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_SCROLL_ENABLED));
            mMapboxMap.setRotateEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ROTATE_ENABLED));
            mMapboxMap.setTiltEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_TILT_ENABLED));
            mMapboxMap.setZoomControlsEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_ZOOM_CONTROLS_ENABLED));
            mMapboxMap.setDebugActive(savedInstanceState.getBoolean(MapboxConstants.STATE_DEBUG_ACTIVE));
            mMapboxMap.setStyleUrl(savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL));
            setAccessToken(savedInstanceState.getString(MapboxConstants.STATE_ACCESS_TOKEN));
            mNativeMapView.setDefaultTransitionDuration(
                    savedInstanceState.getLong(MapboxConstants.STATE_DEFAULT_TRANSITION_DURATION));

            // User location
            try {
                //noinspection ResourceType
                mMapboxMap.setMyLocationEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED));
            } catch (SecurityException ignore) {
                // User did not accept location permissions
            }

            // Compass
            mMapboxMap.setCompassEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_COMPASS_ENABLED));
            mMapboxMap.setCompassGravity(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_GRAVITY));
            mMapboxMap.setCompassMargins(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP)
                    , savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM));

            // Logo
            mMapboxMap.setLogoVisibility(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_VISIBILITY));
            mMapboxMap.setLogoGravity(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_GRAVITY));
            mMapboxMap.setLogoMargins(savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_LEFT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_TOP)
                    , savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_RIGHT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_LOGO_MARGIN_BOTTOM));

            // Attribution
            mMapboxMap.setAttributionVisibility(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_VISIBILITY));
            mMapboxMap.setAttributionGravity(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_GRAVITY));
            mMapboxMap.setAttributionMargins(savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_LEFT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_TOP)
                    , savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_RIGHT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_BOTTOM));

            //noinspection ResourceType
            mMapboxMap.setMyLocationTrackingMode(savedInstanceState.getInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, MyLocationTracking.TRACKING_NONE));
            //noinspection ResourceType
            mMapboxMap.setMyBearingTrackingMode(savedInstanceState.getInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, MyBearingTracking.NONE));
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
                if (change == DID_FINISH_LOADING_MAP) {
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
        outState.putParcelable(MapboxConstants.STATE_CAMERA_POSITION, mMapboxMap.getCameraPosition());
        outState.putBoolean(MapboxConstants.STATE_ZOOM_ENABLED, mMapboxMap.isZoomEnabled());
        outState.putBoolean(MapboxConstants.STATE_SCROLL_ENABLED, mMapboxMap.isScrollEnabled());
        outState.putBoolean(MapboxConstants.STATE_ROTATE_ENABLED, mMapboxMap.isRotateEnabled());
        outState.putBoolean(MapboxConstants.STATE_TILT_ENABLED, mMapboxMap.isTiltEnabled());
        outState.putBoolean(MapboxConstants.STATE_ZOOM_CONTROLS_ENABLED, mMapboxMap.isZoomControlsEnabled());
        outState.putBoolean(MapboxConstants.STATE_DEBUG_ACTIVE, mMapboxMap.isDebugActive());
        outState.putString(MapboxConstants.STATE_STYLE_URL, mMapboxMap.getStyleUrl());
        outState.putString(MapboxConstants.STATE_ACCESS_TOKEN, mMapboxMap.getAccessToken());
        outState.putLong(MapboxConstants.STATE_DEFAULT_TRANSITION_DURATION, mNativeMapView.getDefaultTransitionDuration());
        outState.putBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED, mMapboxMap.isMyLocationEnabled());
        outState.putInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, mMapboxMap.getMyLocationTrackingMode());
        outState.putInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, mMapboxMap.getMyBearingTrackingMode());

        // Compass
        LayoutParams compassParams = (LayoutParams) mCompassView.getLayoutParams();
        outState.putBoolean(MapboxConstants.STATE_COMPASS_ENABLED, mMapboxMap.isCompassEnabled());
        outState.putInt(MapboxConstants.STATE_COMPASS_GRAVITY, compassParams.gravity);
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT, compassParams.leftMargin);
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP, compassParams.topMargin);
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM, compassParams.bottomMargin);
        outState.putInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT, compassParams.rightMargin);

        // Logo
        LayoutParams logoParams = (LayoutParams) mLogoView.getLayoutParams();
        outState.putInt(MapboxConstants.STATE_LOGO_GRAVITY, logoParams.gravity);
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_LEFT, logoParams.leftMargin);
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_TOP, logoParams.topMargin);
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_RIGHT, logoParams.rightMargin);
        outState.putInt(MapboxConstants.STATE_LOGO_MARGIN_BOTTOM, logoParams.bottomMargin);
        outState.putInt(MapboxConstants.STATE_LOGO_VISIBILITY, mLogoView.getVisibility());

        // Attribution
        LayoutParams attrParams = (LayoutParams) mAttributionsView.getLayoutParams();
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_GRAVITY, attrParams.gravity);
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_LEFT, attrParams.leftMargin);
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_TOP, attrParams.topMargin);
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_RIGHT, attrParams.rightMargin);
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_MARGIN_BOTTOM, attrParams.bottomMargin);
        outState.putInt(MapboxConstants.STATE_ATTRIBUTION_VISIBILITY, mAttributionsView.getVisibility());
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

    // Called when debug mode is enabled to update a FPS counter
    // Called via JNI from NativeMapView
    // Forward to any listener
    protected void onFpsChanged(final double fps) {
        post(new Runnable() {
            @Override
            public void run() {
                MapboxMap.OnFpsChangedListener listener = mMapboxMap.getOnFpsChangedListener();
                if (listener != null) {
                    listener.onFpsChanged(fps);
                }
            }
        });
    }

    //
    // Pitch / Tilt
    //

    /**
     * Gets the current Tilt in degrees of the MapView
     *
     * @return tilt in degrees
     */
    double getTilt() {
        return mNativeMapView.getPitch();
    }

    /**
     * Sets the Tilt in degrees of the MapView.
     *
     * @param pitch    New tilt in degrees
     * @param duration Animation time in milliseconds.  If null then 0 is used, making the animation immediate.
     */
    @FloatRange(from = MapboxConstants.MINIMUM_TILT, to = MapboxConstants.MAXIMUM_TILT)
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

    /**
     * Returns the current heading of the map relative to true north.
     *
     * @return The current heading measured in degrees.
     */
    @UiThread
    @FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION)
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

    /**
     * <p>
     * Rotates the map to a new heading relative to true north immediately.
     * </p>
     * <ul>
     * <li>The value 0 means that the top edge of the map view will correspond to true north.</li>
     * <li>The value 90 means the top of the map will point due east.</li>
     * <li>The value 180 means the top of the map will point due south.</li>
     * <li>The value 270 means the top of the map will point due west.</li>
     * </ul>
     * <p>
     * The initial heading is 0.
     * </p>
     * If you want to animate the change, use {@link MapView#setDirection(double, boolean)}.
     *
     * @param direction The new heading measured in degrees.
     * @see MapView#setDirection(double, boolean)
     */
    @UiThread
    void setDirection(@FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION) double direction) {
        setDirection(direction, false);
    }

    /**
     * <p>
     * Rotates the map to a new heading relative to true north and optionally animates the change.
     * </p>
     * <ul>
     * <li>The value 0 means that the top edge of the map view will correspond to true north.</li>
     * <li>The value 90 means the top of the map will point due east.</li>
     * <li>The value 180 means the top of the map will point due south.</li>
     * <li>The value 270 means the top of the map will point due west.</li>
     * </ul>
     * The initial heading is 0.
     *
     * @param direction The new heading measured in degrees from true north.
     * @param animated  If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    void setDirection(@FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION) double direction, boolean animated) {
        long duration = animated ? MapboxConstants.ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        // Out of range direactions are normallised in setBearing
        mNativeMapView.setBearing(-direction, duration);
    }

    /**
     * Resets the map heading to true north and animates the change.
     */
    @UiThread
    void resetNorth() {
        mNativeMapView.cancelTransitions();
        mNativeMapView.resetNorth();
    }

    //
    // Zoom
    //

    /**
     * Returns the current zoom level of the map view.
     *
     * @return The current zoom.
     */
    @UiThread
    @FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM)
    double getZoom() {
        return mNativeMapView.getZoom();
    }

    /**
     * <p>
     * Zooms the map to a new zoom level immediately without changing the center coordinate.
     * </p>
     * <p>
     * At zoom level 0, tiles cover the entire world map;
     * at zoom level 1, tiles cover 1/14 of the world;
     * at zoom level 2, tiles cover 1/16 of the world, and so on.
     * </p>
     * <p>
     * The initial zoom level is 0. The maximum zoom level is {@link MapboxConstants#MAXIMUM_ZOOM}.
     * </p>
     * If you want to animate the change, use {@link MapView#setZoom(double, boolean)}.
     *
     * @param zoomLevel The new zoom.
     * @see MapView#setZoom(double, boolean)
     * @see MapboxConstants#MAXIMUM_ZOOM
     */
    @UiThread
    void setZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double zoomLevel) {
        setZoom(zoomLevel, false);
    }

    /**
     * <p>
     * Zooms the map to a new zoom level and optionally animates the change without changing the center coordinate.
     * </p>
     * <p>
     * At zoom level 0, tiles cover the entire world map;
     * at zoom level 1, tiles cover 1/14 of the world;
     * at zoom level 2, tiles cover 1/16 of the world, and so on.
     * </p>
     * The initial zoom level is 0. The maximum zoom level is {@link MapboxConstants#MAXIMUM_ZOOM}.
     *
     * @param zoomLevel The new zoom level.
     * @param animated  If true, animates the change. If false, immediately changes the map.
     * @see MapboxConstants#MAXIMUM_ZOOM
     */
    @UiThread
    void setZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double zoomLevel, boolean animated) {
        long duration = animated ? MapboxConstants.ANIMATION_DURATION : 0;
        setZoom(zoomLevel, duration);
    }

    /**
     * <p>
     * Zooms the map to a new zoom level and optionally animates the change without changing the center coordinate.
     * </p>
     * <p>
     * At zoom level 0, tiles cover the entire world map;
     * at zoom level 1, tiles cover 1/14 of the world;
     * at zoom level 2, tiles cover 1/16 of the world, and so on.
     * </p>
     * The initial zoom level is 0. The maximum zoom level is {@link MapboxConstants#MAXIMUM_ZOOM}.
     *
     * @param zoomLevel The new zoom level.
     * @param duration  The length of the animation.
     * @see MapboxConstants#MAXIMUM_ZOOM
     */
    @UiThread
    void setZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double zoomLevel, long duration) {
        if ((zoomLevel < MapboxConstants.MINIMUM_ZOOM) || (zoomLevel > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.w(TAG, "Not setting zoom, value is in a unsupported range: " + zoomLevel);
            return;
        }
        mNativeMapView.cancelTransitions();
        mNativeMapView.setZoom(zoomLevel, duration);
    }

    void setZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double zoomLevel, float x, float y) {
        if ((zoomLevel < MapboxConstants.MINIMUM_ZOOM) || (zoomLevel > MapboxConstants.MAXIMUM_ZOOM)) {
            Log.w(TAG, "Not setting zoom, value is in a unsupported range: " + zoomLevel);
            return;
        }
        LatLng target = fromScreenLocation(new PointF(x, y));
        flyTo(-1, target, MapboxConstants.ANIMATION_DURATION, -1, zoomLevel, null);
    }

    void setZoom(CameraUpdateFactory.ZoomUpdate update, long duration) {
        switch (update.getType()) {
            case CameraUpdateFactory.ZoomUpdate.ZOOM_IN:
                setZoom(getZoom() + 1, duration);
                break;
            case CameraUpdateFactory.ZoomUpdate.ZOOM_OUT:
                setZoom(getZoom() - 1, duration);
                break;
            case CameraUpdateFactory.ZoomUpdate.ZOOM_TO:
                setZoom(update.getZoom(), duration);
                break;
            case CameraUpdateFactory.ZoomUpdate.ZOOM_BY:
                setZoom(getZoom() + update.getZoom(), duration);
                break;
            case CameraUpdateFactory.ZoomUpdate.ZOOM_TO_POINT:
                setZoom(update.getZoom(), update.getX(), update.getY());
                break;
        }
    }


//    /**
//     * Return The current content padding left of the map view viewport.
//     *
//     * @return The current content padding left
//     */
///*
//    public int getContentPaddingLeft() {
//        return mContentPaddingLeft;
//    }
//*/
//
//    /**
//     * Return The current content padding left of the map view viewport.
//     *
//     * @return The current content padding left
//     */
///*
//    public int getContentPaddingTop() {
//        return mContentPaddingTop;
//    }
//*/
//
//    /**
//     * Return The current content padding left of the map view viewport.
//     *
//     * @return The current content padding left
//     */
///*
//    public int getContentPaddingRight() {
//        return mContentPaddingRight;
//    }
//*/
//
//    /**
//     * Return The current content padding left of the map view viewport.
//     *
//     * @param zoomLevel The new zoom level.
//     * @param animated  If true, animates the change. If false, immediately changes the map.
//     * @see MapboxMap#MAXIMUM_ZOOM
//     */
///*
//    public int getContentPaddingBottom() {
//        return mContentPaddingBottom;
//*/

    /**
     * <p>
     * Sets the minimum zoom level the map can be displayed at.
     * </p>
     *
     * @param minZoom The new minimum zoom level.
     */
    @UiThread
    void setMinZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
        mNativeMapView.setMinZoom(minZoom);
    }

    /**
     * <p>
     * Gets the maximum zoom level the map can be displayed at.
     * </p>
     *
     * @return The minimum zoom level.
     */
    @UiThread
    public double getMinZoom() {
        return mNativeMapView.getMinZoom();
    }

    /**
     * <p>
     * Sets the maximum zoom level the map can be displayed at.
     * </p>
     *
     * @param maxZoom The new maximum zoom level.
     */
    @UiThread
    public void setMaxZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double maxZoom) {
        mNativeMapView.setMaxZoom(maxZoom);
    }

    /**
     * <p>
     * Gets the maximum zoom level the map can be displayed at.
     * </p>
     *
     * @return The maximum zoom level.
     */
    @UiThread
    public double getMaxZoom() {
        return mNativeMapView.getMaxZoom();
    }

    /**
     * <p>
     * Changes whether the user may zoom the map.
     * </p>
     * <p>
     * This setting controls only user interactions with the map. If you set the value to false,
     * you may still change the map location programmatically.
     * </p>
     * The default value is true.
     *
     * @param zoomEnabled If true, zooming is enabled.
     */
    @UiThread
    void setZoomEnabled(boolean zoomEnabled) {
        if (mMapboxMap.isZoomControlsEnabled() && (getVisibility() == View.VISIBLE) && zoomEnabled) {
            mZoomButtonsController.setVisible(true);
        } else {
            mZoomButtonsController.setVisible(false);
        }
    }

    /**
     * <p>
     * Sets whether the zoom controls are enabled.
     * If enabled, the zoom controls are a pair of buttons
     * (one for zooming in, one for zooming out) that appear on the screen.
     * When pressed, they cause the camera to zoom in (or out) by one zoom level.
     * If disabled, the zoom controls are not shown.
     * </p>
     * By default the zoom controls are enabled if the device is only single touch capable;
     *
     * @param enabled If true, the zoom controls are enabled.
     */
    void setZoomControlsEnabled(boolean enabled) {
        if (enabled && (getVisibility() == View.VISIBLE) && mMapboxMap.isZoomEnabled()) {
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
            mNativeMapView.scaleBy(2.0, x / mScreenDensity, y / mScreenDensity, MapboxConstants.ANIMATION_DURATION);
        } else {
            mNativeMapView.scaleBy(0.5, x / mScreenDensity, y / mScreenDensity, MapboxConstants.ANIMATION_DURATION);
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
    boolean isDebugActive() {
        return mNativeMapView.getDebug();
    }

    /**
     * <p>
     * Changes whether the map debug information is shown.
     * </p>
     * The default value is false.
     *
     * @param debugActive If true, map debug information is shown.
     */
    @UiThread
    void setDebugActive(boolean debugActive) {
        mNativeMapView.setDebug(debugActive);
    }

    /**
     * <p>
     * Cycles through the map debug options.
     * </p>
     * The value of {@link MapView#isDebugActive()} reflects whether there are
     * any map debug options enabled or disabled.
     *
     * @see MapView#isDebugActive()
     */
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
    @UiThread
    void setStyleUrl(@NonNull String url) {
        mNativeMapView.setStyleUrl(url);
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
    LatLng fromScreenLocation(@NonNull PointF point) {
        float x = point.x;
        float y = point.y;

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
    PointF toScreenLocation(@NonNull LatLng location) {
        PointF point = mNativeMapView.pixelForLatLng(location);

        float x = point.x * mScreenDensity;
        float y = point.y * mScreenDensity;

        return new PointF(x, y);
    }

    //
    // Annotations
    //

    private void loadIcon(Icon icon) {
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
                bitmap.getWidth(),
                bitmap.getHeight(),
                scale, buffer.array());
    }

    private void reloadIcons() {
        int count = mIcons.size();
        for (int i = 0; i < count; i++) {
            Icon icon = mIcons.get(i);
            loadIcon(icon);
        }
    }

    private Marker prepareMarker(MarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = marker.getIcon();
        if (icon == null) {
            icon = IconFactory.getInstance(getContext()).defaultMarker();
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

    /**
     * <p>
     * Adds a marker to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptions A marker options object that defines how to render the marker.
     * @return The {@code Marker} that was added to the map.
     */
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
        marker.setMapboxMap(mMapboxMap); // the annotation needs to know which map view it is in
        mAnnotations.add(marker);
        return marker;
    }

    /**
     * <p>
     * Adds multiple markers to this map.
     * </p>
     * The marker's icon is rendered on the map at the location {@code Marker.position}.
     * If {@code Marker.title} is defined, the map shows an info box with the marker's title and snippet.
     *
     * @param markerOptionsList A list of marker options objects that defines how to render the markers.
     * @return A list of the {@code Marker}s that were added to the map.
     */
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
            m.setMapboxMap(mMapboxMap);
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
    Polyline addPolyline(@NonNull PolylineOptions polylineOptions) {
        if (polylineOptions == null) {
            Log.w(TAG, "polylineOptions was null, so just returning null");
            return null;
        }

        Polyline polyline = polylineOptions.getPolyline();
        long id = mNativeMapView.addPolyline(polyline);
        polyline.setId(id);
        polyline.setMapboxMap(mMapboxMap);
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
            p.setMapboxMap(mMapboxMap);
            mAnnotations.add(p);
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
    Polygon addPolygon(@NonNull PolygonOptions polygonOptions) {
        if (polygonOptions == null) {
            Log.w(TAG, "polygonOptions was null, so just returning null");
            return null;
        }

        Polygon polygon = polygonOptions.getPolygon();
        long id = mNativeMapView.addPolygon(polygon);
        polygon.setId(id);
        polygon.setMapboxMap(mMapboxMap);
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
            p.setMapboxMap(mMapboxMap);
            mAnnotations.add(p);
        }

        return new ArrayList<>(polygons);
    }


    /**
     * <p>
     * Convenience method for removing a Marker from the map.
     * </p>
     * Calls removeAnnotation() internally
     *
     * @param marker Marker to remove
     */
    @UiThread
    void removeMarker(@NonNull Marker marker) {
        removeAnnotation(marker);
    }

    /**
     * Removes an annotation from the map.
     *
     * @param annotation The annotation object to remove.
     */
    @UiThread
    void removeAnnotation(@NonNull Annotation annotation) {
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

    /**
     * Removes multiple annotations from the map.
     *
     * @param annotationList A list of annotation objects to remove.
     */
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

    /**
     * Removes all annotations from the map.
     */
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

    /**
     * Returns a list of all the annotations on the map.
     *
     * @return A list of all the annotation objects. The returned object is a copy so modifying this
     * list will not update the map.
     */
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

    /**
     * <p>
     * Returns the distance spanned by one pixel at the specified latitude and current zoom level.
     * </p>
     * The distance between pixels decreases as the latitude approaches the poles.
     * This relationship parallels the relationship between longitudinal coordinates at different latitudes.
     *
     * @param latitude The latitude for which to return the value.
     * @return The distance measured in meters.
     */
    @UiThread
    double getMetersPerPixelAtLatitude(@FloatRange(from = -180, to = 180) double latitude) {
        return mNativeMapView.getMetersPerPixelAtLatitude(latitude, getZoom()) / mScreenDensity;
    }

    //
    // Mapbox Core GL Camera
    //

    /**
     * Change any combination of center, zoom, bearing, and pitch, without
     * a transition. The map will retain the current values for any options
     * not included in `options`.
     *
     * @param bearing Bearing in Radians
     * @param center  Center Coordinate
     * @param pitch   Pitch in Radians
     * @param zoom    Zoom Level
     */
    @UiThread
    void jumpTo(double bearing, LatLng center, double pitch, double zoom) {
        if (mNativeMapView == null) {
            return;
        }
        mNativeMapView.cancelTransitions();
        mNativeMapView.jumpTo(bearing, center, pitch, zoom);
    }

    /**
     * Change any combination of center, zoom, bearing, and pitch, with a smooth animation
     * between old and new values. The map will retain the current values for any options
     * not included in `options`.
     *
     * @param bearing  Bearing in Radians
     * @param center   Center Coordinate
     * @param duration Animation time in Nanoseconds
     * @param pitch    Pitch in Radians
     * @param zoom     Zoom Level
     */
    @UiThread
    void easeTo(double bearing, LatLng center, long duration, double pitch, double zoom, @Nullable final MapboxMap.CancelableCallback cancelableCallback) {
        if (mNativeMapView == null) {
            return;
        }
        mNativeMapView.cancelTransitions();

        // Register callbacks early enough
        if (cancelableCallback != null) {
            addOnMapChangedListener(new OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED) {
                        cancelableCallback.onFinish();

                        // Clean up after self
                        removeOnMapChangedListener(this);
                    }
                }
            });
        }

        mNativeMapView.easeTo(bearing, center, duration, pitch, zoom);
    }

    /**
     * Flying animation to a specified location/zoom/bearing with automatic curve.
     *
     * @param bearing  Bearing in Radians
     * @param center   Center Coordinate
     * @param duration Animation time in Nanoseconds
     * @param pitch    Pitch in Radians
     * @param zoom     Zoom Level
     */
    @UiThread
    void flyTo(double bearing, LatLng center, long duration, double pitch, double zoom, @Nullable final MapboxMap.CancelableCallback cancelableCallback) {
        if (mNativeMapView == null) {
            return;
        }
        mNativeMapView.cancelTransitions();

        // Register callbacks early enough
        if (cancelableCallback != null) {
            addOnMapChangedListener(new OnMapChangedListener() {
                @Override
                public void onMapChanged(@MapChange int change) {
                    if (change == REGION_DID_CHANGE_ANIMATED) {
                        cancelableCallback.onFinish();

                        // Clean up after self
                        removeOnMapChangedListener(this);
                    }
                }
            });
        }

        mNativeMapView.flyTo(bearing, center, duration, pitch, zoom);
    }

    /**
     * Changes the map's viewport to fit the given coordinate bounds.
     *
     * @param bounds The bounds that the viewport will show in its entirety.
     */
    @UiThread
    void setVisibleCoordinateBounds(@NonNull CoordinateBounds bounds) {
        setVisibleCoordinateBounds(bounds, false);
    }

    /**
     * Changes the map's viewing area to fit the given coordinate bounds, optionally animating the change.
     *
     * @param bounds   The bounds that the viewport will show in its entirety.
     * @param animated If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    void setVisibleCoordinateBounds(@NonNull CoordinateBounds bounds, boolean animated) {
        setVisibleCoordinateBounds(bounds, new RectF(), animated);
    }

    /**
     * Changes the map’s viewport to fit the given coordinate bounds with additional padding at the
     * edge of the map,  optionally animating the change.
     *
     * @param bounds   The bounds that the viewport will show in its entirety.
     * @param padding  The minimum padding (in pixels) that will be visible around the given coordinate bounds.
     * @param animated If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    void setVisibleCoordinateBounds(@NonNull CoordinateBounds bounds, @NonNull RectF padding, boolean animated) {
        LatLng[] coordinates = {
                new LatLng(bounds.getNorthEast().getLatitude(), bounds.getSouthWest().getLongitude()),
                bounds.getSouthWest(),
                new LatLng(bounds.getSouthWest().getLatitude(), bounds.getNorthEast().getLongitude()),
                bounds.getNorthEast()

        };
        setVisibleCoordinateBounds(coordinates, padding, animated);
    }

    /**
     * Changes the map’s viewport to fit the given coordinates, optionally some additional padding on each side
     * and animating the change.
     *
     * @param coordinates The coordinates that the viewport will show.
     * @param padding     The minimum padding (in pixels) that will be visible around the given coordinate bounds.
     * @param animated    If true, animates the change. If false, immediately changes the map.
     */
    @UiThread
    void setVisibleCoordinateBounds(@NonNull LatLng[] coordinates, @NonNull RectF padding, boolean animated) {
        setVisibleCoordinateBounds(coordinates, padding, getDirection(), animated);
    }

    private void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, boolean animated) {
        setVisibleCoordinateBounds(coordinates, padding, direction, animated ? MapboxConstants.ANIMATION_DURATION : 0l);
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

        for (Marker marker : mMapboxMap.getSelectedMarkers()) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
                marker.showInfoWindow(mMapboxMap, this);
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
            for (InfoWindow infoWindow : mMapboxMap.getInfoWindows()) {
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
     * <p/>
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
        if (mMapboxMap.isZoomControlsEnabled()) {
            mZoomButtonsController.setVisible(false);
        }
    }

    // Called when view is hidden and shown
    @Override
    protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
        // Required by ZoomButtonController (from Android SDK documentation)
        if (mMapboxMap.isZoomControlsEnabled() && (visibility != View.VISIBLE)) {
            mZoomButtonsController.setVisible(false);
        }
        if (mMapboxMap.isZoomControlsEnabled() && (visibility == View.VISIBLE) && mMapboxMap.isZoomEnabled()) {
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
        @SuppressLint("ResourceType")
        public boolean onDown(MotionEvent event) {
            // Show the zoom controls
            if (mMapboxMap.isZoomControlsEnabled() && mMapboxMap.isZoomEnabled()) {
                mZoomButtonsController.setVisible(true);
            }
            return true;
        }

        // Called for double taps
        @Override
        public boolean onDoubleTapEvent(MotionEvent e) {
            if (!mMapboxMap.isZoomEnabled()) {
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

            List<Marker> selectedMarkers = mMapboxMap.getSelectedMarkers();

            final float toleranceSides = 15 * mScreenDensity;
            final float toleranceTop = 20 * mScreenDensity;
            final float toleranceBottom = 5 * mScreenDensity;

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
                int count = mAnnotations.size();
                for (int i = 0; i < count; i++) {
                    Annotation annotation = mAnnotations.get(i);
                    if (annotation instanceof Marker) {
                        if (annotation.getId() == newSelectedMarkerId) {
                            if (selectedMarkers.isEmpty() || !selectedMarkers.contains(annotation)) {
                                mMapboxMap.selectMarker((Marker) annotation);
                            }
                            break;
                        }
                    }
                }
            } else {
                // deselect any selected marker
                mMapboxMap.deselectMarkers();

                // notify app of map click
                MapboxMap.OnMapClickListener listener = mMapboxMap.getOnMapClickListener();
                if (listener != null) {
                    LatLng point = fromScreenLocation(tapPoint);
                    listener.onMapClick(point);
                }
            }

            return true;
        }

        // Called for a long press
        @Override
        public void onLongPress(MotionEvent e) {
            MapboxMap.OnMapLongClickListener listener = mMapboxMap.getOnMapLongClickListener();
            if (listener != null && !mQuickZoom) {
                LatLng point = fromScreenLocation(new PointF(e.getX(), e.getY()));
                listener.onMapLongClick(point);
            }
        }

        // Called for flings
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                               float velocityY) {
            if (!mMapboxMap.isScrollEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModes();

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

            MapboxMap.OnFlingListener listener = mMapboxMap.getOnFlingListener();
            if (listener != null) {
                listener.onFling();
            }

            return true;
        }

        // Called for drags
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (!mMapboxMap.isScrollEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModes();

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Scroll the map
            mNativeMapView.moveBy(-distanceX / mScreenDensity, -distanceY / mScreenDensity);

            MapboxMap.OnScrollListener listener = mMapboxMap.getOnScrollListener();
            if (listener != null) {
                listener.onScroll();
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
            if (!mMapboxMap.isZoomEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModes();

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
            if (!mMapboxMap.isZoomEnabled()) {
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
            if (mMapboxMap.isScrollEnabled() && !mQuickZoom && mUserLocationView.getMyLocationTrackingMode() == MyLocationTracking.TRACKING_NONE) {
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
            if (!mMapboxMap.isRotateEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModes();

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
            if (!mMapboxMap.isRotateEnabled()) {
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
                        detector.getFocusY() / mScreenDensity);
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
            if (!mMapboxMap.isTiltEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModes();

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
            if (!mMapboxMap.isTiltEnabled()) {
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
            pitch = Math.max(MapboxConstants.MINIMUM_TILT, Math.min(MapboxConstants.MAXIMUM_TILT, pitch));

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
            if (!mMapboxMap.isZoomEnabled()) {
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
                if (!mMapboxMap.isScrollEnabled()) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move left
                mNativeMapView.moveBy(scrollDist / mScreenDensity, 0.0 / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_RIGHT:
                if (!mMapboxMap.isScrollEnabled()) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move right
                mNativeMapView.moveBy(-scrollDist / mScreenDensity, 0.0 / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_UP:
                if (!mMapboxMap.isScrollEnabled()) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move up
                mNativeMapView.moveBy(0.0 / mScreenDensity, scrollDist / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_DOWN:
                if (!mMapboxMap.isScrollEnabled()) {
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
                if (!mMapboxMap.isZoomEnabled()) {
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
                if (!mMapboxMap.isZoomEnabled()) {
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
                if (!mMapboxMap.isScrollEnabled()) {
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
                if (!mMapboxMap.isZoomEnabled()) {
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
                    if (!mMapboxMap.isZoomEnabled()) {
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
                if (mMapboxMap.isZoomControlsEnabled() && mMapboxMap.isZoomEnabled()) {
                    mZoomButtonsController.setVisible(true);
                }
                return true;

            case MotionEvent.ACTION_HOVER_EXIT:
                // Hide the zoom controls
                if (mMapboxMap.isZoomControlsEnabled()) {
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

    // Called when MapView is being created
    private boolean isConnected() {
        ConnectivityManager connectivityManager = (ConnectivityManager) getContext().getApplicationContext()
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
        return (activeNetwork != null) && activeNetwork.isConnectedOrConnecting();
    }

    // Called when our Internet connectivity has changed
    private void onConnectivityChanged(boolean isConnected) {
        mNativeMapView.setReachability(isConnected);
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

    //
    // User location
    //

    /**
     * <p>
     * Enables or disables the my-location layer.
     * While enabled, the my-location layer continuously draws an indication of a user's current
     * location and bearing.
     * </p>
     * In order to use the my-location layer feature you need to request permission for either
     * {@link android.Manifest.permission#ACCESS_COARSE_LOCATION}
     * or @link android.Manifest.permission#ACCESS_FINE_LOCATION.
     *
     * @param enabled True to enable; false to disable.
     * @throws SecurityException if no suitable permission is present
     */
    @UiThread
    @RequiresPermission(anyOf = {
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION})
    void setMyLocationEnabled(boolean enabled) {
        mUserLocationView.setEnabled(enabled);
    }

    /**
     * Returns the currently displayed user location, or null if there is no location data available.
     *
     * @return The currently displayed user location.
     */
    @UiThread
    @Nullable
    Location getMyLocation() {
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
    void setOnMyLocationChangeListener(@Nullable MapboxMap.OnMyLocationChangeListener listener) {
        mUserLocationView.setOnMyLocationChangeListener(listener);
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
    @RequiresPermission(anyOf = {
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION})
    void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && !mMapboxMap.isMyLocationEnabled()) {
            //noinspection ResourceType
            mMapboxMap.setMyLocationEnabled(true);
        }

        mUserLocationView.setMyLocationTrackingMode(myLocationTrackingMode);
        MapboxMap.OnMyLocationTrackingModeChangeListener listener = mMapboxMap.getOnMyLocationTrackingModeChangeListener();
        if (listener != null) {
            listener.onMyLocationTrackingModeChange(myLocationTrackingMode);
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
    int getMyLocationTrackingMode() {
        return mUserLocationView.getMyLocationTrackingMode();
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
    @RequiresPermission(anyOf = {
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION})
    void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        if (myBearingTrackingMode != MyBearingTracking.NONE && !mMapboxMap.isMyLocationEnabled()) {
            //noinspection ResourceType
            mMapboxMap.setMyLocationEnabled(true);
        }
        mUserLocationView.setMyBearingTrackingMode(myBearingTrackingMode);
        MapboxMap.OnMyBearingTrackingModeChangeListener listener = mMapboxMap.getOnMyBearingTrackingModeChangeListener();
        if (listener != null) {
            listener.onMyBearingTrackingModeChange(myBearingTrackingMode);
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
    @MyLocationTracking.Mode
    int getMyBearingTrackingMode() {
        //noinspection ResourceType
        return mUserLocationView.getMyBearingTrackingMode();
    }

    private void resetTrackingModes() {
        try {
            //noinspection ResourceType
            setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
            //noinspection ResourceType
            setMyBearingTrackingMode(MyBearingTracking.NONE);
        } catch (SecurityException ignore) {
            // User did not accept location permissions
        }
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

    //
    // Content padding
    //

//    /**
//     * Sets the distance from the edges of the map view’s frame to the edges of the map
//     * view’s logical viewport.
//     * <p/>
//     * When the value of this property is equal to {0,0,0,0}, viewport
//     * properties such as `getLatLng` assume a viewport that matches the map
//     * view’s frame. Otherwise, those properties are inset, excluding part of the
//     * frame from the viewport. For instance, if the only the top edge is inset, the
//     * map center is effectively shifted downward.
//     *
//     * @param left   The left margin in pixels.
//     * @param top    The top margin in pixels.
//     * @param right  The right margin in pixels.
//     * @param bottom The bottom margin in pixels.
//     */
//
//    @UiThread
//    void setContentPadding(int left, int top, int right, int bottom) {
//
//        if (left == mContentPaddingLeft && top == mContentPaddingTop && right == mContentPaddingRight && bottom == mContentPaddingBottom) {
//            return;
//        }
//
//        mContentPaddingLeft = left;
//        mContentPaddingTop = top;
//        mContentPaddingRight = right;
//        mContentPaddingBottom = bottom;
//
//        mNativeMapView.setContentPadding(top / mScreenDensity, left / mScreenDensity, bottom / mScreenDensity, right / mScreenDensity);
//    }

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

    //
    // Custom layer
    //

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

    /**
     * Sets a callback object which will be triggered when the {@link MapboxMap} instance is ready to be used.
     *
     * @param callback The callback object that will be triggered when the map is ready to be used.
     */
    @UiThread
    public void getMapAsync(@NonNull OnMapReadyCallback callback) {
        callback.onMapReady(mMapboxMap);
    }

    MapboxMap getMapboxMap() {
        return mMapboxMap;
    }

    void setMapboxMap(MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
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
        layoutParams.setMargins(left, top, right, bottom);
        view.setLayoutParams(layoutParams);
    }

    private void setWidgetMargins(@NonNull final View view, float leftDp, float topDp, float rightDp, float bottomDp) {
        LayoutParams layoutParams = (LayoutParams) view.getLayoutParams();
        layoutParams.setMargins((int) (leftDp * mScreenDensity), (int) (topDp * mScreenDensity), (int) (rightDp * mScreenDensity), (int) (bottomDp * mScreenDensity));
        view.setLayoutParams(layoutParams);
    }

    private static class AttributionOnClickListener implements View.OnClickListener, DialogInterface.OnClickListener {

        private static final int ATTRIBUTION_INDEX_IMPROVE_THIS_MAP = 2;
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
                LatLng latLng = mMapView.getMapboxMap().getCameraPosition().target;
                url = String.format(url, latLng.getLongitude(), latLng.getLatitude(), (int) mMapView.getZoom());
            }
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(url));
            context.startActivity(intent);
        }
    }


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
     * <p>
     * This {@link MapChange} is triggered whenever the currently displayed map region is about to changing
     * without an animation.
     * </p>
     * <p>
     * This event is followed by a series of {@link MapView#REGION_IS_CHANGING} and ends
     * with {@link MapView#REGION_DID_CHANGE}.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_WILL_CHANGE = 0;

    /**
     * <p>
     * This {@link MapChange} is triggered whenever the currently displayed map region is about to changing
     * with an animation.
     * </p>
     * <p>
     * This event is followed by a series of {@link MapView#REGION_IS_CHANGING} and ends
     * with {@link MapView#REGION_DID_CHANGE_ANIMATED}.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_WILL_CHANGE_ANIMATED = 1;

    /**
     * <p>
     * This {@link MapChange} is triggered whenever the currently displayed map region is changing.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_IS_CHANGING = 2;

    /**
     * <p>
     * This {@link MapChange} is triggered whenever the currently displayed map region finished changing
     * without an animation.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_DID_CHANGE = 3;

    /**
     * <p>
     * This {@link MapChange} is triggered whenever the currently displayed map region finished changing
     * with an animation.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int REGION_DID_CHANGE_ANIMATED = 4;

    /**
     * <p>
     * This {@link MapChange} is triggered when the map is about to start loading a new map style.
     * </p>
     * <p>
     * This event is followed by {@link MapView#DID_FINISH_LOADING_MAP} or
     * {@link MapView#DID_FAIL_LOADING_MAP}.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int WILL_START_LOADING_MAP = 5;

    /**
     * <p>
     * This {@link MapChange} is triggered when the map has successfully loaded a new map style.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_LOADING_MAP = 6;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     * <p>
     * This event is triggered when the map has failed to load a new map style.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FAIL_LOADING_MAP = 7;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int WILL_START_RENDERING_FRAME = 8;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_FRAME = 9;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_FRAME_FULLY_RENDERED = 10;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int WILL_START_RENDERING_MAP = 11;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_MAP = 12;

    /**
     * <p>
     * This {@link MapChange} is currently not implemented.
     * </p>
     *
     * @see MapView.OnMapChangedListener
     */
    public static final int DID_FINISH_RENDERING_MAP_FULLY_RENDERED = 13;


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
