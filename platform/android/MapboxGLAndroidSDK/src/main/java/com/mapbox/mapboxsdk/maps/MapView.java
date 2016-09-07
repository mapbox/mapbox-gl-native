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
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.drawable.ColorDrawable;
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
import android.util.DisplayMetrics;
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
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ZoomButtonsController;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.ShoveGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.mapboxsdk.MapboxAccountManager;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.exceptions.IconBitmapChangedException;
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
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

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

    private MapboxMap mMapboxMap;
    private boolean mInitialLoad;
    private boolean mDestroyed;

    private List<Icon> mIcons;
    private int mAverageIconHeight;
    private int mAverageIconWidth;

    private NativeMapView mNativeMapView;
    private boolean mHasSurface = false;

    private ViewGroup mMarkerViewContainer;
    private CompassView mCompassView;
    private ImageView mLogoView;
    private ImageView mAttributionsView;
    private MyLocationView mMyLocationView;
    private LocationListener mMyLocationListener;

    private CopyOnWriteArrayList<OnMapChangedListener> mOnMapChangedListener;
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
    private boolean mDragStarted = false;
    private boolean mQuickZoom = false;
    private boolean mScrollInProgress = false;

    private int mContentPaddingLeft;
    private int mContentPaddingTop;
    private int mContentPaddingRight;
    private int mContentPaddingBottom;

    private PointF mFocalPoint;

    private String mStyleUrl;
    private String mInitalStyle;

    private List<OnMapReadyCallback> mOnMapReadyCallbackList;
    private SnapshotRequest mSnapshotRequest;

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
            LayoutInflater.from(context).inflate(R.layout.mapview_preview, this);
            return;
        }

        mInitialLoad = true;
        mOnMapReadyCallbackList = new ArrayList<>();
        mOnMapChangedListener = new CopyOnWriteArrayList<>();
        mMapboxMap = new MapboxMap(this);
        mIcons = new ArrayList<>();
        View view = LayoutInflater.from(context).inflate(R.layout.mapview_internal, this);
        setWillNotDraw(false);

        // Reference the TextureView
        SurfaceView surfaceView = (SurfaceView) view.findViewById(R.id.surfaceView);

        mNativeMapView = new NativeMapView(this);

        // Ensure this view is interactable
        setClickable(true);
        setLongClickable(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();

        surfaceView.getHolder().addCallback(new SurfaceCallback());

        // Touch gesture detectors
        mGestureDetector = new GestureDetectorCompat(context, new GestureListener());
        mGestureDetector.setIsLongpressEnabled(true);
        mScaleGestureDetector = new ScaleGestureDetector(context, new ScaleGestureListener());
        ScaleGestureDetectorCompat.setQuickScaleEnabled(mScaleGestureDetector, true);
        mRotateGestureDetector = new RotateGestureDetector(context, new RotateGestureListener());
        mShoveGestureDetector = new ShoveGestureDetector(context, new ShoveGestureListener());

        mZoomButtonsController = new ZoomButtonsController(this);
        mZoomButtonsController.setZoomSpeed(MapboxConstants.ANIMATION_DURATION);
        mZoomButtonsController.setOnZoomListener(new OnZoomListener());

        // Connectivity
        onConnectivityChanged(isConnected());

        mMarkerViewContainer = (ViewGroup) view.findViewById(R.id.markerViewContainer);

        mMyLocationView = (MyLocationView) view.findViewById(R.id.userLocationView);
        mMyLocationView.setMapboxMap(mMapboxMap);

        mCompassView = (CompassView) view.findViewById(R.id.compassView);
        mCompassView.setMapboxMap(mMapboxMap);

        mLogoView = (ImageView) view.findViewById(R.id.logoView);

        // Setup Attributions control
        mAttributionsView = (ImageView) view.findViewById(R.id.attributionView);
        mAttributionsView.setOnClickListener(new AttributionOnClickListener(this));

        mScreenDensity = context.getResources().getDisplayMetrics().density;

        setInitialState(options);

        // Shows the zoom controls
        if (!context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
            mMapboxMap.getUiSettings().setZoomControlsEnabled(true);
        }
    }

    private void setInitialState(MapboxMapOptions options) {
        mMapboxMap.setDebugActive(options.getDebugActive());

        CameraPosition position = options.getCamera();
        if (position != null) {
            mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(position));
            mMyLocationView.setTilt(position.tilt);
        }

        // access token
        String accessToken = options.getAccessToken();
        if (!TextUtils.isEmpty(accessToken)) {
            mMapboxMap.setAccessToken(accessToken);
        }

        // style url
        String style = options.getStyle();
        if (!TextUtils.isEmpty(style)) {
            mInitalStyle = style;
        }

        // MyLocationView
        MyLocationViewSettings myLocationViewSettings = mMapboxMap.getMyLocationViewSettings();
        myLocationViewSettings.setForegroundDrawable(options.getMyLocationForegroundDrawable(), options.getMyLocationForegroundBearingDrawable());
        myLocationViewSettings.setForegroundTintColor(options.getMyLocationForegroundTintColor());
        myLocationViewSettings.setBackgroundDrawable(options.getMyLocationBackgroundDrawable(), options.getMyLocationBackgroundPadding());
        myLocationViewSettings.setBackgroundTintColor(options.getMyLocationBackgroundTintColor());
        myLocationViewSettings.setAccuracyAlpha(options.getMyLocationAccuracyAlpha());
        myLocationViewSettings.setAccuracyTintColor(options.getMyLocationAccuracyTintColor());
        mMapboxMap.setMyLocationEnabled(options.getLocationEnabled());

        // Enable gestures
        UiSettings uiSettings = mMapboxMap.getUiSettings();
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
        mMapboxMap.setMaxZoom(options.getMaxZoom());
        mMapboxMap.setMinZoom(options.getMinZoom());

        // Compass
        uiSettings.setCompassEnabled(options.getCompassEnabled());
        uiSettings.setCompassGravity(options.getCompassGravity());
        int[] compassMargins = options.getCompassMargins();
        if (compassMargins != null) {
            uiSettings.setCompassMargins(compassMargins[0], compassMargins[1], compassMargins[2], compassMargins[3]);
        } else {
            int tenDp = (int) getResources().getDimension(R.dimen.ten_dp);
            uiSettings.setCompassMargins(tenDp, tenDp, tenDp, tenDp);
        }

        // Logo
        uiSettings.setLogoEnabled(options.getLogoEnabled());
        uiSettings.setLogoGravity(options.getLogoGravity());
        int[] logoMargins = options.getLogoMargins();
        if (logoMargins != null) {
            uiSettings.setLogoMargins(logoMargins[0], logoMargins[1], logoMargins[2], logoMargins[3]);
        } else {
            int sixteenDp = (int) getResources().getDimension(R.dimen.sixteen_dp);
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
            int sevenDp = (int) resources.getDimension(R.dimen.seven_dp);
            int seventySixDp = (int) resources.getDimension(R.dimen.seventy_six_dp);
            uiSettings.setAttributionMargins(seventySixDp, sevenDp, sevenDp, sevenDp);
        }

        int attributionTintColor = options.getAttributionTintColor();
        uiSettings.setAttributionTintColor(attributionTintColor != -1 ?
                attributionTintColor : ColorUtils.getPrimaryColor(getContext()));
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
        String accessToken = mMapboxMap.getAccessToken();
        if (TextUtils.isEmpty(accessToken)) {
            accessToken = MapboxAccountManager.getInstance().getAccessToken();
            mMapboxMap.setAccessToken(accessToken);
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
                mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
            }

            UiSettings uiSettings = mMapboxMap.getUiSettings();
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
            uiSettings.setCompassMargins(savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_LEFT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_TOP)
                    , savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_RIGHT)
                    , savedInstanceState.getInt(MapboxConstants.STATE_COMPASS_MARGIN_BOTTOM));

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

            mMapboxMap.setDebugActive(savedInstanceState.getBoolean(MapboxConstants.STATE_DEBUG_ACTIVE));
            mMapboxMap.setStyleUrl(savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL));

            // User location
            try {
                mMapboxMap.setMyLocationEnabled(savedInstanceState.getBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED));
            } catch (SecurityException ignore) {
                // User did not accept location permissions
            }

            TrackingSettings trackingSettings = mMapboxMap.getTrackingSettings();
            //noinspection ResourceType
            trackingSettings.setMyLocationTrackingMode(savedInstanceState.getInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, MyLocationTracking.TRACKING_NONE));
            //noinspection ResourceType
            trackingSettings.setMyBearingTrackingMode(savedInstanceState.getInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, MyBearingTracking.NONE));
        } else if (savedInstanceState == null) {
            // Start Telemetry (authorization determined in initial MapboxEventManager constructor)
            Log.i(MapView.class.getCanonicalName(), "MapView start Telemetry...");
            MapboxEventManager eventManager = MapboxEventManager.getMapboxEventManager();
            eventManager.initialize(getContext(), getAccessToken());
        }

        // Initialize EGL
        mNativeMapView.initializeDisplay();
        mNativeMapView.initializeContext();

        // Add annotation deselection listener
        addOnMapChangedListener(new OnMapChangedListener() {
            @Override
            public void onMapChanged(@MapChange int change) {
                if (change == WILL_START_RENDERING_MAP && mInitialLoad) {
                    mInitialLoad = false;
                    reloadIcons();
                    reloadMarkers();
                    adjustTopOffsetPixels();
                    if (mOnMapReadyCallbackList.size() > 0) {
                        Iterator<OnMapReadyCallback> iterator = mOnMapReadyCallbackList.iterator();
                        while (iterator.hasNext()) {
                            OnMapReadyCallback callback = iterator.next();
                            callback.onMapReady(mMapboxMap);
                            iterator.remove();
                        }
                        mMapboxMap.getMarkerViewManager().scheduleViewMarkerInvalidation();
                    }
                } else if (change == REGION_IS_CHANGING || change == REGION_DID_CHANGE || change == DID_FINISH_LOADING_MAP) {
                    mMapboxMap.getMarkerViewManager().scheduleViewMarkerInvalidation();

                    mCompassView.update(getDirection());
                    mMyLocationView.update();
                    mMapboxMap.getMarkerViewManager().update();

                    for (InfoWindow infoWindow : mMapboxMap.getInfoWindows()) {
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
        outState.putParcelable(MapboxConstants.STATE_CAMERA_POSITION, mMapboxMap.getCameraPosition());
        outState.putBoolean(MapboxConstants.STATE_DEBUG_ACTIVE, mMapboxMap.isDebugActive());
        outState.putString(MapboxConstants.STATE_STYLE_URL, mStyleUrl);
        outState.putBoolean(MapboxConstants.STATE_MY_LOCATION_ENABLED, mMapboxMap.isMyLocationEnabled());

        // TrackingSettings
        TrackingSettings trackingSettings = mMapboxMap.getTrackingSettings();
        outState.putInt(MapboxConstants.STATE_MY_LOCATION_TRACKING_MODE, trackingSettings.getMyLocationTrackingMode());
        outState.putInt(MapboxConstants.STATE_MY_BEARING_TRACKING_MODE, trackingSettings.getMyBearingTrackingMode());

        // UiSettings
        UiSettings uiSettings = mMapboxMap.getUiSettings();
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
     * You must call this method from the parent's {@link Activity#onDestroy()} or {@link Fragment#onDestroy()}.
     */
    @UiThread
    public void onDestroy() {
        mDestroyed = true;
        mNativeMapView.terminateContext();
        mNativeMapView.terminateDisplay();
        mNativeMapView.destroySurface();
        mNativeMapView.destroy();
        mNativeMapView = null;
    }

    /**
     * You must call this method from the parent's {@link Activity#onPause()} or {@link Fragment#onPause()}.
     */
    @UiThread
    public void onPause() {
        // Register for connectivity changes
        if (mConnectivityReceiver != null) {
            getContext().unregisterReceiver(mConnectivityReceiver);
            mConnectivityReceiver = null;
        }

        mMyLocationView.onPause();
    }

    /**
     * You must call this method from the parent's {@link Activity#onResume()} or {@link Fragment#onResume()}.
     */
    @UiThread
    public void onResume() {
        // Register for connectivity changes
        mConnectivityReceiver = new ConnectivityReceiver();
        getContext().registerReceiver(mConnectivityReceiver, new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));

        mNativeMapView.update();
        mMyLocationView.onResume();

        if (mStyleUrl == null) {
            // user supplied style through xml
            // user has failed to supply a style url
            setStyleUrl(mInitalStyle == null ? Style.MAPBOX_STREETS : mInitalStyle);
        }
    }

    void setFocalPoint(PointF focalPoint) {
        if (focalPoint == null) {
            // resetting focal point,
            UiSettings uiSettings = mMapboxMap.getUiSettings();
            // need to validate if we need to reset focal point with user provided one
            if (uiSettings.getFocalPoint() != null) {
                focalPoint = uiSettings.getFocalPoint();
            }
        }
        mFocalPoint = focalPoint;
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
    // LatLng / CenterCoordinate
    //

    LatLng getLatLng() {
        return mNativeMapView.getLatLng();
    }

    //
    // Pitch / Tilt
    //

    double getTilt() {
        return mNativeMapView.getPitch();
    }

    void setTilt(Double pitch) {
        mMyLocationView.setTilt(pitch);
        mNativeMapView.setPitch(pitch, 0);
    }


    //
    // Direction
    //

    double getDirection() {
        if (mDestroyed) {
            return 0;
        }

        double direction = -mNativeMapView.getBearing();

        while (direction > 360) {
            direction -= 360;
        }
        while (direction < 0) {
            direction += 360;
        }

        return direction;
    }

    void setDirection(@FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION) double direction) {
        if (mDestroyed) {
            return;
        }
        setDirection(direction, false);
    }

    void setDirection(@FloatRange(from = MapboxConstants.MINIMUM_DIRECTION, to = MapboxConstants.MAXIMUM_DIRECTION) double direction, boolean animated) {
        if (mDestroyed) {
            return;
        }
        long duration = animated ? MapboxConstants.ANIMATION_DURATION : 0;
        mNativeMapView.cancelTransitions();
        // Out of range directions are normalised in setBearing
        mNativeMapView.setBearing(-direction, duration);
    }

    void resetNorth() {
        if (mDestroyed) {
            return;
        }
        mMyLocationView.setBearing(0);
        mNativeMapView.cancelTransitions();
        mNativeMapView.resetNorth();
    }

    //
    // Content padding
    //

    int getContentPaddingLeft() {
        return mContentPaddingLeft;
    }

    int getContentPaddingTop() {
        return mContentPaddingTop;
    }

    int getContentPaddingRight() {
        return mContentPaddingRight;
    }

    int getContentPaddingBottom() {
        return mContentPaddingBottom;
    }

    int getContentWidth() {
        return getWidth() - mContentPaddingLeft - mContentPaddingRight;
    }

    int getContentHeight() {
        return getHeight() - mContentPaddingBottom - mContentPaddingTop;
    }

    //
    // Zoom
    //

    double getZoom() {
        if (mDestroyed) {
            return 0;
        }
        return mNativeMapView.getZoom();
    }

    void setMinZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double minZoom) {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.setMinZoom(minZoom);
    }

    double getMinZoom() {
        if (mDestroyed) {
            return 0;
        }
        return mNativeMapView.getMinZoom();
    }

    void setMaxZoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM, to = MapboxConstants.MAXIMUM_ZOOM) double maxZoom) {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.setMaxZoom(maxZoom);
    }

    double getMaxZoom() {
        if (mDestroyed) {
            return 0;
        }
        return mNativeMapView.getMaxZoom();
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

        // work around to invalidate camera position
        postDelayed(new ZoomInvalidator(mMapboxMap), MapboxConstants.ANIMATION_DURATION);
    }

    //
    // Debug
    //

    boolean isDebugActive() {
        if (mDestroyed) {
            return false;
        }
        return mNativeMapView.getDebug();
    }

    void setDebugActive(boolean debugActive) {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.setDebug(debugActive);
    }

    void cycleDebugOptions() {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.cycleDebugOptions();
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
        if (mDestroyed) {
            return;
        }

        // stopgap for https://github.com/mapbox/mapbox-gl-native/issues/6242
        if (TextUtils.isEmpty(mNativeMapView.getAccessToken())) {
            setAccessToken(MapboxAccountManager.getInstance().getAccessToken());
        }

        mStyleUrl = url;
        mNativeMapView.setStyleUrl(url);
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
        return mStyleUrl;
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
        if (mDestroyed) {
            return;
        }
        // validateAccessToken does the null check
        if (!TextUtils.isEmpty(accessToken)) {
            accessToken = accessToken.trim();
        }
        MapboxAccountManager.validateAccessToken(accessToken);
        mNativeMapView.setAccessToken(accessToken);
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
        if (mDestroyed) {
            return "";
        }
        return mNativeMapView.getAccessToken();
    }

    //
    // Projection
    //

    LatLng fromScreenLocation(@NonNull PointF point) {
        if (mDestroyed) {
            return new LatLng();
        }
        point.set(point.x / mScreenDensity, point.y / mScreenDensity);
        return mNativeMapView.latLngForPixel(point);
    }

    PointF toScreenLocation(@NonNull LatLng location) {
        if (mDestroyed || location == null) {
            return new PointF();
        }
        PointF pointF = mNativeMapView.pixelForLatLng(location);
        pointF.set(pointF.x * mScreenDensity, pointF.y * mScreenDensity);
        return pointF;
    }

    //
    // Annotations
    //

    Icon loadIconForMarker(Marker marker) {
        Icon icon = marker.getIcon();

        // calculating average before adding
        int iconSize = mIcons.size() + 1;

        // TODO replace former if case with anchor implementation,
        // current workaround for having extra pixels is diving height by 2
        if (icon == null) {
            icon = IconFactory.getInstance(getContext()).defaultMarker();
            Bitmap bitmap = icon.getBitmap();
            mAverageIconHeight = mAverageIconHeight + (bitmap.getHeight() / 2 - mAverageIconHeight) / iconSize;
            mAverageIconWidth = mAverageIconHeight + (bitmap.getWidth() - mAverageIconHeight) / iconSize;
            marker.setIcon(icon);
        } else {
            Bitmap bitmap = icon.getBitmap();
            mAverageIconHeight = mAverageIconHeight + (bitmap.getHeight() - mAverageIconHeight) / iconSize;
            mAverageIconWidth = mAverageIconHeight + (bitmap.getWidth() - mAverageIconHeight) / iconSize;
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
        return icon;
    }

    void loadIcon(Icon icon) {
        if (mDestroyed) {
            return;
        }
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

    void reloadIcons() {
        int count = mIcons.size();
        for (int i = 0; i < count; i++) {
            Icon icon = mIcons.get(i);
            loadIcon(icon);
        }
    }

    void updateMarker(@NonNull Marker updatedMarker) {
        if (mDestroyed) {
            return;
        }
        if (updatedMarker == null) {
            Log.w(MapboxConstants.TAG, "marker was null, doing nothing");
            return;
        }

        if (updatedMarker.getId() == -1) {
            Log.w(MapboxConstants.TAG, "marker has an id of -1, possibly was not added yet, doing nothing");
            return;
        }

        if (!(updatedMarker instanceof MarkerView)) {
            ensureIconLoaded(updatedMarker);
        }

        mNativeMapView.updateMarker(updatedMarker);
    }


    void updatePolygon(Polygon polygon) {
        if (mDestroyed) {
            return;
        }

        if (polygon == null) {
            Log.w(MapboxConstants.TAG, "polygon was null, doing nothing");
            return;
        }

        if (polygon.getId() == -1) {
            Log.w(MapboxConstants.TAG, "polygon has an id of -1, indicating the polygon was not added to the map yet.");
            return;
        }

        mNativeMapView.updatePolygon(polygon);
    }

    void updatePolyline(Polyline polyline) {
        if (mDestroyed) {
            return;
        }

        if (polyline == null) {
            Log.w(MapboxConstants.TAG, "polygon was null, doing nothing");
            return;
        }

        if (polyline.getId() == -1) {
            Log.w(MapboxConstants.TAG, "polygon has an id of -1, indicating the polygon was not added to the map yet.");
            return;
        }

        mNativeMapView.updatePolyline(polyline);
    }

    private void ensureIconLoaded(Marker marker) {
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

        // this seems to be a costly operation according to the profiler so I'm trying to save some calls
        Marker previousMarker = marker.getId() != -1 ? (Marker) mMapboxMap.getAnnotation(marker.getId()) : null;
        if (previousMarker == null || previousMarker.getIcon() == null || previousMarker.getIcon() != marker.getIcon()) {
            marker.setTopOffsetPixels(getTopOffsetPixelsForIcon(icon));
        }
    }

    long addMarker(@NonNull Marker marker) {
        if (mDestroyed) {
            return 0l;
        }
        return mNativeMapView.addMarker(marker);
    }

    long[] addMarkers(@NonNull List<Marker> markerList) {
        if (mDestroyed) {
            return new long[]{};
        }
        return mNativeMapView.addMarkers(markerList);
    }

    long addPolyline(@NonNull Polyline polyline) {
        if (mDestroyed) {
            return 0l;
        }
        return mNativeMapView.addPolyline(polyline);
    }

    long[] addPolylines(@NonNull List<Polyline> polylines) {
        if (mDestroyed) {
            return new long[]{};
        }
        return mNativeMapView.addPolylines(polylines);
    }

    long addPolygon(@NonNull Polygon polygon) {
        if (mDestroyed) {
            return 0l;
        }
        return mNativeMapView.addPolygon(polygon);
    }

    long[] addPolygons(@NonNull List<Polygon> polygons) {
        if (mDestroyed) {
            return new long[]{};
        }
        return mNativeMapView.addPolygons(polygons);
    }

    void removeAnnotation(long id) {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.removeAnnotation(id);
    }

    void removeAnnotations(@NonNull long[] ids) {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.removeAnnotations(ids);
    }

    List<Marker> getMarkersInRect(@NonNull RectF rect) {
        if (mDestroyed || rect == null) {
            return new ArrayList<>();
        }

        long[] ids = mNativeMapView.queryPointAnnotations(rect);

        List<Long> idsList = new ArrayList<>(ids.length);
        for (int i = 0; i < ids.length; i++) {
            idsList.add(ids[i]);
        }

        List<Marker> annotations = new ArrayList<>(ids.length);
        List<Annotation> annotationList = mMapboxMap.getAnnotations();
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            if (annotation instanceof Marker && idsList.contains(annotation.getId())) {
                annotations.add((Marker) annotation);
            }
        }

        return new ArrayList<>(annotations);
    }

    public List<MarkerView> getMarkerViewsInRect(@NonNull RectF rect) {
        if (mDestroyed || rect == null) {
            return new ArrayList<>();
        }

        long[] ids = mNativeMapView.queryPointAnnotations(rect);

        List<Long> idsList = new ArrayList<>(ids.length);
        for (int i = 0; i < ids.length; i++) {
            idsList.add(ids[i]);
        }

        List<MarkerView> annotations = new ArrayList<>(ids.length);
        List<Annotation> annotationList = mMapboxMap.getAnnotations();
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            if (annotation instanceof MarkerView) {
                annotations.add((MarkerView) annotation);
            }
        }

        return new ArrayList<>(annotations);
    }

    /**
     * @return the ViewGroup containing the marker views
     */
    public ViewGroup getMarkerViewContainer() {
        return mMarkerViewContainer;
    }


    int getTopOffsetPixelsForIcon(Icon icon) {
        if (mDestroyed) {
            return 0;
        }

        return (int) (mNativeMapView.getTopOffsetPixelsForAnnotationSymbol(icon.getId())
                * mScreenDensity);
    }

    void setContentPadding(int left, int top, int right, int bottom) {
        if (mDestroyed) {
            return;
        }

//        if (left == mContentPaddingLeft && top == mContentPaddingTop && right == mContentPaddingRight && bottom == mContentPaddingBottom) {
//            return;
//        }

        mContentPaddingLeft = left;
        mContentPaddingTop = top;
        mContentPaddingRight = right;
        mContentPaddingBottom = bottom;

        int[] userLocationViewPadding = mMapboxMap.getMyLocationViewSettings().getPadding();
        left += userLocationViewPadding[0];
        top += userLocationViewPadding[1];
        right += userLocationViewPadding[2];
        bottom += userLocationViewPadding[3];

        mNativeMapView.setContentPadding(top / mScreenDensity, left / mScreenDensity, bottom / mScreenDensity, right / mScreenDensity);
    }

    public void invalidateContentPadding() {
        setContentPadding(mContentPaddingLeft, mContentPaddingTop, mContentPaddingRight, mContentPaddingBottom);
    }

    double getMetersPerPixelAtLatitude(@FloatRange(from = -180, to = 180) double latitude) {
        if (mDestroyed) {
            return 0;
        }

        return mNativeMapView.getMetersPerPixelAtLatitude(latitude, getZoom()) / mScreenDensity;
    }

    //
    // Mapbox Core GL Camera
    //

    void jumpTo(double bearing, LatLng center, double pitch, double zoom) {
        if (mDestroyed) {
            return;
        }
        mNativeMapView.cancelTransitions();
        mNativeMapView.jumpTo(bearing, center, pitch, zoom);
    }

    void easeTo(double bearing, LatLng center, long duration, double pitch, double zoom, boolean easingInterpolator, @Nullable final MapboxMap.CancelableCallback cancelableCallback) {
        if (mDestroyed) {
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

        mNativeMapView.easeTo(bearing, center, duration, pitch, zoom, easingInterpolator);
    }

    void flyTo(double bearing, LatLng center, long duration, double pitch, double zoom, @Nullable final MapboxMap.CancelableCallback cancelableCallback) {
        if (mDestroyed) {
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

    private void adjustTopOffsetPixels() {
        List<Annotation> annotations = mMapboxMap.getAnnotations();
        int count = annotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotations.get(i);
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
        if (mDestroyed) {
            return;
        }
        List<Annotation> annotations = mMapboxMap.getAnnotations();
        int count = annotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotations.get(i);
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

        if (mDestroyed) {
            return;
        }

        if (!mHasSurface) {
            return;
        }

        mNativeMapView.render();
    }

    @Override
    protected void onSizeChanged(int width, int height, int oldw, int oldh) {
        if (mDestroyed) {
            return;
        }

        if (!isInEditMode()) {
            mNativeMapView.resizeView((int) (width / mScreenDensity), (int) (height / mScreenDensity));
        }
    }

    double getScale() {
        if (mDestroyed) {
            return 0;
        }

        return mNativeMapView.getScale();
    }

    private class SurfaceCallback implements SurfaceHolder.Callback {

        private Surface mSurface;

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mNativeMapView.createSurface(mSurface = holder.getSurface());
            mHasSurface = true;
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            if (mDestroyed) {
                return;
            }
            mNativeMapView.resizeFramebuffer(width, height);
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            mHasSurface = false;

            if (mNativeMapView != null) {
                mNativeMapView.destroySurface();
            }
            mSurface.release();
        }
    }

    CameraPosition invalidateCameraPosition() {
        if (mDestroyed) {
            return new CameraPosition.Builder().build();
        }
        CameraPosition position = new CameraPosition.Builder(mNativeMapView.getCameraValues()).build();
        mMyLocationView.setCameraPosition(position);
        return position;
    }

    double getBearing() {
        if (mDestroyed) {
            return 0;
        }

        double direction = -mNativeMapView.getBearing();

        while (direction > 360) {
            direction -= 360;
        }
        while (direction < 0) {
            direction += 360;
        }

        return direction;
    }

    void setBearing(float bearing) {
        if (mDestroyed) {
            return;
        }
        mMyLocationView.setBearing(bearing);
        mNativeMapView.setBearing(bearing);
    }

    void setBearing(float bearing, long duration) {
        if (mDestroyed) {
            return;
        }
        mMyLocationView.setBearing(bearing);
        mNativeMapView.setBearing(bearing, duration);
    }

    void setBearing(double bearing, float focalX, float focalY) {
        if (mDestroyed) {
            return;
        }
        mMyLocationView.setBearing(bearing);
        mNativeMapView.setBearing(bearing, focalX, focalY);
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
        if (mMapboxMap.getUiSettings().isZoomControlsEnabled()) {
            mZoomButtonsController.setVisible(false);
        }

        // make sure we don't leak location listener
        if (mMyLocationListener != null) {
            // cleanup to prevent memory leak
            LocationServices services = LocationServices.getLocationServices(getContext());
            services.removeLocationListener(mMyLocationListener);
            mMyLocationListener = null;
        }
    }

    // Called when view is hidden and shown
    @Override
    protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
        if (isInEditMode()) {
            return;
        }

        // Required by ZoomButtonController (from Android SDK documentation)
        if (mMapboxMap.getUiSettings().isZoomControlsEnabled() && (visibility != View.VISIBLE)) {
            mZoomButtonsController.setVisible(false);
        }
        if (mMapboxMap.getUiSettings().isZoomControlsEnabled() && (visibility == View.VISIBLE)) {
            mZoomButtonsController.setVisible(true);
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
    private void trackGestureEvent(@NonNull String gestureId, @NonNull float xCoordinate, @NonNull float yCoordinate) {
        LatLng tapLatLng = fromScreenLocation(new PointF(xCoordinate, yCoordinate));

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
        evt.put(MapboxEvent.KEY_ZOOM, mMapboxMap.getCameraPosition().zoom);

        MapboxEventManager.getMapboxEventManager().pushEvent(evt);
    }

    /**
     * Helper method for tracking DragEnd gesture event
     * See {@see MapboxEvent#TYPE_MAP_DRAGEND}
     *
     * @param xCoordinate Original x screen coordinate at end of drag
     * @param yCoordinate Orginal y screen coordinate at end of drag
     */
    private void trackGestureDragEndEvent(@NonNull float xCoordinate, @NonNull float yCoordinate) {
        LatLng tapLatLng = fromScreenLocation(new PointF(xCoordinate, yCoordinate));

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
        evt.put(MapboxEvent.KEY_ZOOM, mMapboxMap.getCameraPosition().zoom);

        MapboxEventManager.getMapboxEventManager().pushEvent(evt);
    }

    // Called when user touches the screen, all positions are absolute
    @Override
    public boolean onTouchEvent(@NonNull MotionEvent event) {
        // Check and ignore non touch or left clicks
        if (mDestroyed) {
            return super.onTouchEvent(event);
        }

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
                mTwoTap = event.getPointerCount() == 2
                        && mMapboxMap.getUiSettings().isZoomGesturesEnabled();
                if (mTwoTap) {
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
                boolean inProgress = mRotateGestureDetector.isInProgress()
                        || mScaleGestureDetector.isInProgress()
                        || mShoveGestureDetector.isInProgress();

                if (mTwoTap && isTap && !inProgress) {
                    if (mFocalPoint != null) {
                        zoom(false, mFocalPoint.x / mScreenDensity, mFocalPoint.y / mScreenDensity);
                    } else {
                        PointF focalPoint = TwoFingerGestureDetector.determineFocalPoint(event);
                        zoom(false, focalPoint.x, focalPoint.y);
                    }
                    mTwoTap = false;
                    return true;
                }

                // Scroll / Pan Has Stopped
                if (mScrollInProgress) {
                    trackGestureDragEndEvent(event.getX(), event.getY());
                    mScrollInProgress = false;
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
    private class GestureListener extends GestureDetector.SimpleOnGestureListener {

        // Must always return true otherwise all events are ignored
        @Override
        @SuppressLint("ResourceType")
        public boolean onDown(MotionEvent event) {
            // Show the zoom controls
            if (mMapboxMap.getUiSettings().isZoomControlsEnabled()) {
                mZoomButtonsController.setVisible(true);
            }
            return true;
        }

        // Called for double taps
        @Override
        public boolean onDoubleTapEvent(MotionEvent e) {
            if (mDestroyed || !mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                return false;
            }

            switch (e.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    break;
                case MotionEvent.ACTION_MOVE:
                    break;
                case MotionEvent.ACTION_UP:
                    if (mQuickZoom) {
                        // insert here?
                        mQuickZoom = false;
                        break;
                    }

                    // Single finger double tap
                    if (mFocalPoint != null) {
                        // User provided focal point
                        zoom(true, mFocalPoint.x, mFocalPoint.y);
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
        public boolean onSingleTapUp(MotionEvent e) {
            if (mDestroyed) {
                return false;
            }
            // Cancel any animation
            mNativeMapView.cancelTransitions();
            return true;
        }

        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            List<Marker> selectedMarkers = mMapboxMap.getSelectedMarkers();

            PointF tapPoint = new PointF(e.getX(), e.getY());
            float toleranceSides = 4 * mScreenDensity;
            float toleranceTopBottom = 10 * mScreenDensity;

            RectF tapRect = new RectF((tapPoint.x - mAverageIconWidth / 2 - toleranceSides) / mScreenDensity,
                    (tapPoint.y - mAverageIconHeight / 2 - toleranceTopBottom) / mScreenDensity,
                    (tapPoint.x + mAverageIconWidth / 2 + toleranceSides) / mScreenDensity,
                    (tapPoint.y + mAverageIconHeight / 2 + toleranceTopBottom) / mScreenDensity);
            
            List<Marker> nearbyMarkers = getMarkersInRect(tapRect);
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
                List<Annotation> annotations = mMapboxMap.getAnnotations();
                int count = annotations.size();
                for (int i = 0; i < count; i++) {
                    Annotation annotation = annotations.get(i);
                    if (annotation instanceof Marker) {
                        if (annotation.getId() == newSelectedMarkerId) {
                            if (selectedMarkers.isEmpty() || !selectedMarkers.contains(annotation)) {
                                // only handle click if no marker view is available
                                if (!(annotation instanceof MarkerView)) {
                                    mMapboxMap.selectMarker((Marker) annotation);
                                }
                            }
                            break;
                        }
                    }
                }
            } else {
                if (mMapboxMap.getUiSettings().isDeselectMarkersOnTap()) {
                    // deselect any selected marker
                    mMapboxMap.deselectMarkers();
                }

                // notify app of map click
                MapboxMap.OnMapClickListener listener = mMapboxMap.getOnMapClickListener();
                if (listener != null) {
                    LatLng point = fromScreenLocation(tapPoint);
                    listener.onMapClick(point);
                }
            }

            trackGestureEvent(MapboxEvent.GESTURE_SINGLETAP, e.getX(), e.getY());
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
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            if (mDestroyed || !mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModesIfRequired();

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

            trackGestureEvent(MapboxEvent.GESTURE_PAN_START, e1.getX(), e1.getY());
            return true;
        }

        // Called for drags
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            if (!mScrollInProgress) {
                mScrollInProgress = true;
            }
            if (mDestroyed || !mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
                return false;
            }

            if (mDragStarted) {
                return false;
            }

            requestDisallowInterceptTouchEvent(true);

            // reset tracking modes if gesture occurs
            resetTrackingModesIfRequired();

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
            if (mDestroyed || !mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModesIfRequired();

            mBeginTime = detector.getEventTime();
            trackGestureEvent(MapboxEvent.GESTURE_PINCH_START, detector.getFocusX(), detector.getFocusY());
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
            UiSettings uiSettings = mMapboxMap.getUiSettings();
            if (mDestroyed || !uiSettings.isZoomGesturesEnabled()) {
                return super.onScale(detector);
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

            if (mDragStarted) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Gesture is a quickzoom if there aren't two fingers
            mQuickZoom = !mTwoTap;

            // Scale the map
            if (mFocalPoint != null) {
                // arround user provided focal point
                mNativeMapView.scaleBy(detector.getScaleFactor(), mFocalPoint.x / mScreenDensity, mFocalPoint.y / mScreenDensity);
            } else if (mQuickZoom) {
                // around center map
                mNativeMapView.scaleBy(detector.getScaleFactor(), (getWidth() / 2) / mScreenDensity, (getHeight() / 2) / mScreenDensity);
            } else {
                // around gesture
                mNativeMapView.scaleBy(detector.getScaleFactor(), detector.getFocusX() / mScreenDensity, detector.getFocusY() / mScreenDensity);
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
            if (mDestroyed || !mMapboxMap.getUiSettings().isRotateGesturesEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModesIfRequired();

            mBeginTime = detector.getEventTime();
            trackGestureEvent(MapboxEvent.GESTURE_ROTATION_START, detector.getFocusX(), detector.getFocusY());
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
            if (mDestroyed || !mMapboxMap.getUiSettings().isRotateGesturesEnabled()) {
                return false;
            }

            if (mDragStarted) {
                return false;
            }

            // If rotate is large enough ignore a tap
            // Also is zoom already started, don't rotate
            mTotalAngle += detector.getRotationDegreesDelta();
            if (!mZoomStarted && ((mTotalAngle > 20.0f) || (mTotalAngle < -20.0f))) {
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
            if (mFocalPoint != null) {
                // User provided focal point
                setBearing(bearing, mFocalPoint.x / mScreenDensity, mFocalPoint.y / mScreenDensity);
            } else {
                // around gesture
                setBearing(bearing, detector.getFocusX() / mScreenDensity, detector.getFocusY() / mScreenDensity);
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
            if (!mMapboxMap.getUiSettings().isTiltGesturesEnabled()) {
                return false;
            }

            // reset tracking modes if gesture occurs
            resetTrackingModesIfRequired();

            mBeginTime = detector.getEventTime();
            trackGestureEvent(MapboxEvent.GESTURE_PITCH_START, detector.getFocusX(), detector.getFocusY());
            return true;
        }

        @Override
        public void onShoveEnd(ShoveGestureDetector detector) {
            mBeginTime = 0;
            mTotalDelta = 0.0f;
            mStarted = false;
            mDragStarted = false;
        }

        @Override
        public boolean onShove(ShoveGestureDetector detector) {
            if (mDestroyed || !mMapboxMap.getUiSettings().isTiltGesturesEnabled()) {
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
            mMapboxMap.setTilt(pitch);

            mDragStarted = true;

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
            if (!mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
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
        if (mDestroyed) {
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
                if (!mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move left
                mNativeMapView.moveBy(scrollDist / mScreenDensity, 0.0 / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_RIGHT:
                if (!mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move right
                mNativeMapView.moveBy(-scrollDist / mScreenDensity, 0.0 / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_UP:
                if (!mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
                    return false;
                }

                // Cancel any animation
                mNativeMapView.cancelTransitions();

                // Move up
                mNativeMapView.moveBy(0.0 / mScreenDensity, scrollDist / mScreenDensity);
                return true;

            case KeyEvent.KEYCODE_DPAD_DOWN:
                if (!mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
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
                if (!mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
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
                if (!mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
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
        if (mDestroyed) {
            return false;
        }
        // Choose the action
        switch (event.getActionMasked()) {
            // The trackball was rotated
            case MotionEvent.ACTION_MOVE:
                if (!mMapboxMap.getUiSettings().isScrollGesturesEnabled()) {
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
                if (!mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
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
        if (mDestroyed) {
            return false;
        }
        // Mouse events
        //if (event.isFromSource(InputDevice.SOURCE_CLASS_POINTER)) { // this is not available before API 18
        if ((event.getSource() & InputDevice.SOURCE_CLASS_POINTER) == InputDevice.SOURCE_CLASS_POINTER) {
            // Choose the action
            switch (event.getActionMasked()) {
                // Mouse scrolls
                case MotionEvent.ACTION_SCROLL:
                    if (!mMapboxMap.getUiSettings().isZoomGesturesEnabled()) {
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
                if (mMapboxMap.getUiSettings().isZoomControlsEnabled()) {
                    mZoomButtonsController.setVisible(true);
                }
                return true;

            case MotionEvent.ACTION_HOVER_EXIT:
                // Hide the zoom controls
                if (mMapboxMap.getUiSettings().isZoomControlsEnabled()) {
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
        Context appContext = getContext().getApplicationContext();
        ConnectivityManager connectivityManager = (ConnectivityManager) appContext.getSystemService(Context.CONNECTIVITY_SERVICE);
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
            OnMapChangedListener listener;
            final Iterator<OnMapChangedListener> iterator = mOnMapChangedListener.iterator();
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
        mMyLocationView.setEnabled(enabled);
    }

    Location getMyLocation() {
        return mMyLocationView.getLocation();
    }

    void setOnMyLocationChangeListener(@Nullable final MapboxMap.OnMyLocationChangeListener listener) {
        if (listener != null) {
            mMyLocationListener = new LocationListener() {
                @Override
                public void onLocationChanged(Location location) {
                    if (listener != null) {
                        listener.onMyLocationChange(location);
                    }
                }
            };
            LocationServices.getLocationServices(getContext()).addLocationListener(mMyLocationListener);
        } else {
            LocationServices.getLocationServices(getContext()).removeLocationListener(mMyLocationListener);
            mMyLocationListener = null;
        }
    }

    void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && !mMapboxMap.isMyLocationEnabled()) {
            mMapboxMap.setMyLocationEnabled(true);
        }
        mMyLocationView.setMyLocationTrackingMode(myLocationTrackingMode);

        if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
            setFocalPoint(new PointF(mMyLocationView.getCenterX(), mMyLocationView.getCenterY()));
        } else {
            setFocalPoint(null);
        }

        MapboxMap.OnMyLocationTrackingModeChangeListener listener = mMapboxMap.getOnMyLocationTrackingModeChangeListener();
        if (listener != null) {
            listener.onMyLocationTrackingModeChange(myLocationTrackingMode);
        }
    }

    void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        if (myBearingTrackingMode != MyBearingTracking.NONE && !mMapboxMap.isMyLocationEnabled()) {
            mMapboxMap.setMyLocationEnabled(true);
        }
        mMyLocationView.setMyBearingTrackingMode(myBearingTrackingMode);
        MapboxMap.OnMyBearingTrackingModeChangeListener listener = mMapboxMap.getOnMyBearingTrackingModeChangeListener();
        if (listener != null) {
            listener.onMyBearingTrackingModeChange(myBearingTrackingMode);
        }
    }

    boolean isPermissionsAccepted() {
        return (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED) ||
                ContextCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    private void resetTrackingModesIfRequired() {
        TrackingSettings trackingSettings = mMapboxMap.getTrackingSettings();
        if (trackingSettings.isDismissLocationTrackingOnGesture()) {
            resetLocationTrackingMode();
        }
        if (trackingSettings.isDismissBearingTrackingOnGesture()) {
            resetBearingTrackingMode();
        }
    }

    private void resetLocationTrackingMode() {
        try {
            TrackingSettings trackingSettings = mMapboxMap.getTrackingSettings();
            trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_NONE);
        } catch (SecurityException ignore) {
            // User did not accept location permissions
        }
    }

    private void resetBearingTrackingMode() {
        try {
            setMyBearingTrackingMode(MyBearingTracking.NONE);
        } catch (SecurityException ignore) {
            // User did not accept location permissions
        }
    }

    //
    // Compass
    //

    void setCompassEnabled(boolean compassEnabled) {
        mCompassView.setEnabled(compassEnabled);
    }

    void setCompassGravity(int gravity) {
        setWidgetGravity(mCompassView, gravity);
    }

    void setCompassMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mCompassView, left, top, right, bottom);
    }

    //
    // Logo
    //

    void setLogoGravity(int gravity) {
        setWidgetGravity(mLogoView, gravity);
    }

    void setLogoMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mLogoView, left, top, right, bottom);
    }

    void setLogoEnabled(boolean visible) {
        mLogoView.setVisibility(visible ? View.VISIBLE : View.GONE);
    }

    //
    // Attribution
    //

    void setAttributionGravity(int gravity) {
        setWidgetGravity(mAttributionsView, gravity);
    }

    void setAttributionMargins(int left, int top, int right, int bottom) {
        setWidgetMargins(mAttributionsView, left, top, right, bottom);
    }

    void setAttributionEnabled(int visibility) {
        mAttributionsView.setVisibility(visibility);
    }

    void setAtttibutionTintColor(int tintColor) {
        ColorUtils.setTintList(mAttributionsView, tintColor);
    }

    int getAttributionTintColor() {
        return mMapboxMap.getUiSettings().getAttributionTintColor();
    }

    /**
     * Sets a callback object which will be triggered when the {@link MapboxMap} instance is ready to be used.
     *
     * @param callback The callback object that will be triggered when the map is ready to be used.
     */
    @UiThread
    public void getMapAsync(final OnMapReadyCallback callback) {
        if (!mInitialLoad && callback != null) {
            callback.onMapReady(mMapboxMap);
        } else {
            if (callback != null) {
                mOnMapReadyCallbackList.add(callback);
            }
        }
    }

    MapboxMap getMapboxMap() {
        return mMapboxMap;
    }

    void setMapboxMap(MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
    }

    MyLocationView getUserLocationView() {
        return mMyLocationView;
    }

    NativeMapView getNativeMapView() {
        return mNativeMapView;
    }

    //
    // Snapshot API
    //

    @UiThread
    void snapshot(@NonNull final MapboxMap.SnapshotReadyCallback callback, @Nullable final Bitmap bitmap) {
        mSnapshotRequest = new SnapshotRequest(bitmap, callback);
        mNativeMapView.scheduleTakeSnapshot();
        mNativeMapView.render();
    }

    // Called when the snapshot method was executed
    // Called via JNI from NativeMapView
    // Forward to any listeners
    protected void onSnapshotReady(byte[] bytes) {
        if (mSnapshotRequest != null && bytes != null) {
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inBitmap = mSnapshotRequest.getBitmap();  // the old Bitmap to be reused
            options.inMutable = true;
            options.inSampleSize = 1;
            Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.length, options);

            MapboxMap.SnapshotReadyCallback callback = mSnapshotRequest.getCallback();
            if (callback != null) {
                callback.onSnapshotReady(bitmap);
            }
        }
    }

    private class SnapshotRequest {
        private Bitmap bitmap;
        private MapboxMap.SnapshotReadyCallback callback;

        public SnapshotRequest(Bitmap bitmap, MapboxMap.SnapshotReadyCallback callback) {
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
        left += mContentPaddingLeft;
        top += mContentPaddingTop;
        right += mContentPaddingRight;
        bottom += mContentPaddingBottom;
        layoutParams.setMargins(left, top, right, bottom);
        view.setLayoutParams(layoutParams);
    }

    private static class AttributionOnClickListener implements View.OnClickListener, DialogInterface.OnClickListener {

        private static final int ATTRIBUTION_INDEX_IMPROVE_THIS_MAP = 2;
        private static final int ATTRIBUTION_INDEX_TELEMETRY_SETTINGS = 3;
        private MapView mMapView;

        public AttributionOnClickListener(MapView mapView) {
            super();
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
            AlertDialog dialog = builder.show();
            dialog.getWindow().setBackgroundDrawable(new ColorDrawable(mMapView.getAttributionTintColor()));
        }

        // Called when someone selects an attribution, 'Improve this map' adds location data to the url
        @Override
        public void onClick(DialogInterface dialog, int which) {
            final Context context = ((Dialog) dialog).getContext();
            if (which == ATTRIBUTION_INDEX_TELEMETRY_SETTINGS) {

                int array = R.array.attribution_telemetry_options;
                if (MapboxEventManager.getMapboxEventManager().isTelemetryEnabled()) {
                    array = R.array.attribution_telemetry_options_already_participating;
                }
                String[] items = context.getResources().getStringArray(array);
                AlertDialog.Builder builder = new AlertDialog.Builder(context, R.style.AttributionAlertDialogStyle);
                builder.setTitle(R.string.attributionTelemetryTitle);
                LayoutInflater factory = LayoutInflater.from(context);
                View content = factory.inflate(R.layout.attribution_telemetry_view, null);

                ListView lv = (ListView) content.findViewById(R.id.telemetryOptionsList);
                lv.setAdapter(new ArrayAdapter<String>(context, R.layout.attribution_list_item, items));
                lv.setChoiceMode(ListView.CHOICE_MODE_SINGLE);

                builder.setView(content);
                final AlertDialog telemDialog = builder.show();
                lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                    @Override
                    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                        switch (position) {
                            case 0:
                                String url = context.getResources().getStringArray(R.array.attribution_links)[3];
                                Intent intent = new Intent(Intent.ACTION_VIEW);
                                intent.setData(Uri.parse(url));
                                context.startActivity(intent);
                                telemDialog.cancel();
                                return;
                            case 1:
                                MapboxEventManager.getMapboxEventManager().setTelemetryEnabled(false);
                                telemDialog.cancel();
                                return;
                            case 2:
                                MapboxEventManager.getMapboxEventManager().setTelemetryEnabled(true);
                                telemDialog.cancel();
                                return;
                        }
                    }
                });
                return;
            }
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

    private static class ZoomInvalidator implements Runnable {

        private MapboxMap mapboxMap;

        public ZoomInvalidator(MapboxMap mapboxMap) {
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
