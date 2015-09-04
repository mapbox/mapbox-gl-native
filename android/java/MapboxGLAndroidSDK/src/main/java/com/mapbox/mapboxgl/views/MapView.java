package com.mapbox.mapboxgl.views;

import android.annotation.TargetApi;
import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.hardware.GeomagneticField;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.text.TextUtils;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.ScaleGestureDetector;
import android.util.AttributeSet;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ZoomButtonsController;
import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.mapboxgl.annotations.Annotation;
import com.mapbox.mapboxgl.annotations.Marker;
import com.mapbox.mapboxgl.annotations.MarkerOptions;
import com.mapbox.mapboxgl.annotations.Polygon;
import com.mapbox.mapboxgl.annotations.PolygonOptions;
import com.mapbox.mapboxgl.annotations.Polyline;
import com.mapbox.mapboxgl.annotations.PolylineOptions;
import com.mapbox.mapboxgl.geometry.BoundingBox;
import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.geometry.LatLngZoom;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;
import com.squareup.okhttp.HttpUrl;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

// Custom view that shows a Map
// Based on SurfaceView as we use OpenGL ES to render
public class MapView extends FrameLayout implements LocationListener {

    //
    // Static members
    //
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
    private static final String STATE_COMPASS_ENABLED = "compassEnabled";
    private static final String STATE_MY_LOCATION_ENABLED = "myLocationEnabled";
    private static final String STATE_USER_LOCATION_TRACKING_MODE = "userLocationTrackingMode";

    /**
     * Every annotation that has been added to the map.
     */
    private List<Annotation> mAnnotations = new ArrayList<>();

    //
    // Instance members
    //

    // Used to call JNI NativeMapView
    private NativeMapView mNativeMapView;

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

    private ConnectivityReceiver mConnectivityReceiver;

    // Holds the context
    private Context mContext;

    // Used for GPS / Location
    private boolean mIsMyLocationEnabled = false;
    private LostApiClient mLocationClient;
    private LocationRequest mLocationRequest;
    private ImageView mGpsMarker;
    private Location mGpsLocation;

    public enum UserLocationTrackingMode {
        NONE, FOLLOW, FOLLOW_BEARING
    }
    private UserLocationTrackingMode mUserLocationTrackingMode = UserLocationTrackingMode.FOLLOW;

    // Used for compass
    private boolean mIsCompassEnabled = true;
    private ImageView mCompassView;
    private SensorManager mSensorManager;
    private Sensor mSensorAccelerometer;
    private Sensor mSensorMagneticField;
    private CompassListener mCompassListener;
    private float[] mValuesAccelerometer = new float[3];
    private float[] mValuesMagneticField = new float[3];
    private float[] mMatrixR = new float[9];
    private float[] mMatrixI = new float[9];
    private float[] mMatrixValues = new float[3];
    private float mCompassBearing;
    private boolean mCompassValid = false;

    // Used for map toggle mode
    private long t0 = new Date().getTime();

    // Used to manage Event Listeners
    private ArrayList<OnMapChangedListener> mOnMapChangedListener;

    private enum MapChange {
        MapChangeNullChange(-1),
        MapChangeRegionWillChange(0),
        MapChangeRegionWillChangeAnimated(1),
        MapChangeRegionIsChanging(2),
        MapChangeRegionDidChange(3),
        MapChangeRegionDidChangeAnimated(4),
        MapChangeWillStartLoadingMap(5),
        MapChangeDidFinishLoadingMap(6),
        MapChangeDidFailLoadingMap(7),
        MapChangeWillStartRenderingFrame(8),
        MapChangeDidFinishRenderingFrame(9),
        MapChangeDidFinishRenderingFrameFullyRendered(10),
        MapChangeWillStartRenderingMap(11),
        MapChangeDidFinishRenderingMap(12),
        MapChangeDidFinishRenderingMapFullyRendered(13);

        private int value;

        private MapChange(int value) {
            this.value = value;
        }

        public static MapChange fromInteger(int value) {
            switch (value) {
                case -1:
                    return MapChange.MapChangeNullChange;
                case 0:
                    return MapChange.MapChangeRegionWillChange;
                case 1:
                    return MapChange.MapChangeRegionWillChangeAnimated;
                case 2:
                    return MapChange.MapChangeRegionIsChanging;
                case 3:
                    return MapChange.MapChangeRegionDidChange;
                case 4:
                    return MapChange.MapChangeRegionDidChangeAnimated;
                case 5:
                    return MapChange.MapChangeWillStartLoadingMap;
                case 6:
                    return MapChange.MapChangeDidFinishLoadingMap;
                case 7:
                    return MapChange.MapChangeDidFailLoadingMap;
                case 8:
                    return MapChange.MapChangeWillStartRenderingFrame;
                case 9:
                    return MapChange.MapChangeDidFinishRenderingFrame;
                case 10:
                    return MapChange.MapChangeDidFinishRenderingFrameFullyRendered;
                case 11:
                    return MapChange.MapChangeWillStartRenderingMap;
                case 12:
                    return MapChange.MapChangeDidFinishRenderingMap;
                case 13:
                    return MapChange.MapChangeDidFinishRenderingMapFullyRendered;
                default:
                    return null;
            }
        }
    }

    public interface OnFlingListener {
        void onFling();
    }

    public interface OnScrollListener {
        void onScroll();
    }

    private OnFlingListener onFlingListener;
    private OnScrollListener onScrollListener;

    public void setOnScrollListener(OnScrollListener onScrollListener) {
        this.onScrollListener = onScrollListener;
    }

    public void setOnFlingListener(OnFlingListener onFlingListener) {
        this.onFlingListener = onFlingListener;
    }

    //
    // Properties
    //

    private boolean mZoomEnabled = true;
    private boolean mScrollEnabled = true;
    private boolean mRotateEnabled = true;
    private String mStyleUrl;

    //
    // Constructors
    //

    // Called when no properties are being set from XML
    public MapView(Context context) {
        super(context);
        initialize(context, null);
    }

    public MapView(Context context, @NonNull String accessToken) {
        super(context);
        initialize(context, null);
        setAccessToken(accessToken);
    }

    public MapView(Context context, @NonNull String accessToken, String styleUrl) {
        super(context);
        initialize(context, null);
        setAccessToken(accessToken);
        setStyleUrl(styleUrl);
    }

    // Called when properties are being set from XML
    public MapView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context, attrs);
    }

    // Called when properties are being set from XML
    public MapView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initialize(context, attrs);
    }

    //
    // Initialization
    //

    // Common initialization code goes here
    private void initialize(Context context, AttributeSet attrs) {

        // Save the context
        mContext = context;

        SurfaceView surfaceView = new SurfaceView(mContext);
        addView(surfaceView);

        // Check if we are in Eclipse UI editor
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
        ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        activityManager.getMemoryInfo(memoryInfo);
        long maxMemory = memoryInfo.availMem;
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            maxMemory = memoryInfo.totalMem;
        }
        mNativeMapView = new NativeMapView(this, cachePath, dataPath, apkPath, mScreenDensity, availableProcessors, maxMemory);

        // Ensure this view is interactable
        setClickable(true);
        setLongClickable(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();

        // Register the SurfaceHolder callbacks
        surfaceView.getHolder().addCallback(new CallbacksHandler());

        // Touch gesture detectors
        mGestureDetector = new GestureDetectorCompat(context, new GestureListener());
        mGestureDetector.setIsLongpressEnabled(true);
        mScaleGestureDetector = new ScaleGestureDetector(context, new ScaleGestureListener());
        ScaleGestureDetectorCompat.setQuickScaleEnabled(mScaleGestureDetector, true);
        mRotateGestureDetector = new RotateGestureDetector(context, new RotateGestureListener());

        // Shows the zoom controls
        // But not when in Eclipse UI editor
        if (!isInEditMode()) {
            if (!context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
                mZoomButtonsController = new ZoomButtonsController(this);
                mZoomButtonsController.setZoomSpeed(ANIMATION_DURATION);
                mZoomButtonsController.setOnZoomListener(new OnZoomListener());
            }

            // Check current connection status
            ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo activeNetwork = connectivityManager.getActiveNetworkInfo();
            boolean isConnected = (activeNetwork != null) && activeNetwork.isConnectedOrConnecting();
            onConnectivityChanged(isConnected);
        }

        // Setup Location Services
        mLocationClient = new LostApiClient.Builder(getContext()).build();
        mLocationRequest = LocationRequest.create()
                .setInterval(1000)
                .setSmallestDisplacement(1)
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

        // Setup Compass
        mSensorManager = (SensorManager) mContext.getSystemService(Context.SENSOR_SERVICE);
        mSensorAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorMagneticField = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mCompassListener = new CompassListener();

        mCompassView = new ImageView(mContext);
        mCompassView.setImageDrawable(ContextCompat.getDrawable(getContext(), R.drawable.compass));
        mCompassView.setContentDescription(getResources().getString(R.string.compassContentDescription));
        LayoutParams lp = new FrameLayout.LayoutParams((int)(48 * mScreenDensity), (int)(48 * mScreenDensity));
        lp.gravity = Gravity.TOP | Gravity.END;
        int tenDp = (int)(10 * mScreenDensity);
        lp.setMargins(tenDp, tenDp, tenDp, tenDp);
        mCompassView.setLayoutParams(lp);
        addView(mCompassView);
        mCompassView.setOnClickListener(new CompassOnClickListener());

        // Setup Support For Listener Tracking
        // MapView's internal listener is setup in onCreate()
        mOnMapChangedListener = new ArrayList<>();

        // Load the attributes
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.MapView, 0, 0);
        try {
            double centerLatitude = typedArray.getFloat(R.styleable.MapView_centerLatitude, 0.0f);
            double centerLongitude = typedArray.getFloat(R.styleable.MapView_centerLongitude, 0.0f);
            LatLng centerCoordinate = new LatLng(centerLatitude, centerLongitude);
            setCenterCoordinate(centerCoordinate);
            setZoomLevel(typedArray.getFloat(R.styleable.MapView_zoomLevel, 0.0f)); // need to set zoom level first because of limitation on rotating when zoomed out
            setDirection(typedArray.getFloat(R.styleable.MapView_direction, 0.0f));
            setZoomEnabled(typedArray.getBoolean(R.styleable.MapView_zoomEnabled, true));
            setScrollEnabled(typedArray.getBoolean(R.styleable.MapView_scrollEnabled, true));
            setRotateEnabled(typedArray.getBoolean(R.styleable.MapView_rotateEnabled, true));
            setDebugActive(typedArray.getBoolean(R.styleable.MapView_debugActive, false));
            if (typedArray.getString(R.styleable.MapView_styleUrl) != null) {
                setStyleUrl(typedArray.getString(R.styleable.MapView_styleUrl));
            }
            if (typedArray.getString(R.styleable.MapView_accessToken) != null) {
                setAccessToken(typedArray.getString(R.styleable.MapView_accessToken));
            }
            if (typedArray.getString(R.styleable.MapView_styleClasses) != null) {
                List<String> styleClasses = Arrays.asList(typedArray.getString(R.styleable.MapView_styleClasses).split("\\s*,\\s*"));
                for (String styleClass : styleClasses) {
                    if (styleClass.length() == 0) {
                        styleClasses.remove(styleClass);
                    }
                }
                setStyleClasses(styleClasses);
            }
            setCompassEnabled(typedArray.getBoolean(R.styleable.MapView_compassEnabled, true));
            setMyLocationEnabled(typedArray.getBoolean(R.styleable.MapView_myLocationEnabled, false));
        } finally {
            typedArray.recycle();
        }
    }

    //
    // Annotations
    //

    public void setSprite(String symbol, float scale, Bitmap bitmap) {
        if(bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
            bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, false);
        }
        ByteBuffer buffer = ByteBuffer.allocate(bitmap.getRowBytes() * bitmap.getHeight());
        bitmap.copyPixelsToBuffer(buffer);

        mNativeMapView.setSprite(symbol, bitmap.getWidth(), bitmap.getHeight(), scale, buffer.array());
    }

    public Marker addMarker(MarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        long id = mNativeMapView.addMarker(marker);
        marker.setId(id);        // the annotation needs to know its id
        marker.setMapView(this); // the annotation needs to know which map view it is in
        mAnnotations.add(marker);
        return marker;
    }

    public Polyline addPolyline(PolylineOptions polylineOptions) {
        Polyline polyline = polylineOptions.getPolyline();
        long id = mNativeMapView.addPolyline(polyline);
        polyline.setId(id);
        polyline.setMapView(this);
        mAnnotations.add(polyline);
        return polyline;
    }

    public Polygon addPolygon(PolygonOptions polygonOptions) {
        Polygon polygon = polygonOptions.getPolygon();
        long id = mNativeMapView.addPolygon(polygon);
        polygon.setId(id);
        polygon.setMapView(this);
        mAnnotations.add(polygon);
        return polygon;
    }

    public List<Polygon> addPolygons(List<PolygonOptions> polygonOptions) {
        List<Polygon> polygons = new ArrayList<>();
        for(PolygonOptions popts : polygonOptions) {
            polygons.add(popts.getPolygon());
        }

        long[] ids = mNativeMapView.addPolygons(polygons);

        for(int i=0; i < polygons.size(); i++) {
            polygons.get(i).setId(ids[i]);
            polygons.get(i).setMapView(this);
            mAnnotations.add(polygons.get(i));
        }

        return Collections.unmodifiableList(polygons);
    }

    private void removeAnnotationsWithId(long annotationId){
        for (Iterator<Annotation> iterator = mAnnotations.iterator(); iterator.hasNext();) {
            Annotation annotation = iterator.next();
            if (annotation.getId() == annotationId) {
                iterator.remove();
            }
        }
    }

    public void removeAnnotation(Annotation annotation) {
        long id = annotation.getId();
        mNativeMapView.removeAnnotation(id);
        mAnnotations.remove(annotation);
    }

    public void removeAnnotation(long annotationId) {
        mNativeMapView.removeAnnotation(annotationId);
        removeAnnotationsWithId(annotationId);
    }

    public void removeAnnotations() {
        long[] ids = new long[mAnnotations.size()];
        for(int i = 0; i < mAnnotations.size(); i++) {
            long id = mAnnotations.get(i).getId();
            ids[i] = id;
        }
        mNativeMapView.removeAnnotations(ids);
        mAnnotations.clear();
    }

    public List<Annotation> getAnnotations() {
        return Collections.unmodifiableList(mAnnotations);
    }

    public List<Annotation> getAnnotationsInBounds(BoundingBox bbox) {
        List<Annotation> annotations = new ArrayList<>();
        long[] ids = mNativeMapView.getAnnotationsInBounds(bbox);
        List<Long> idsList = new ArrayList<>();
        for(int i = 0; i < ids.length; i++) {
            idsList.add(new Long(ids[i]));
        }
        for(int i = 0; i < mAnnotations.size(); i++) {
            Annotation annotation = mAnnotations.get(i);
            if (annotation instanceof Marker && idsList.contains(annotation.getId())) {
                annotations.add(annotation);
            }
        }
        for(int i = 0; i < annotations.size(); i++) {
            Log.d(TAG, "tapped: " + Long.toString(annotations.get(i).getId()));
        }
        return annotations;
    }

    //
    // Property methods
    //

    public LatLng getCenterCoordinate() {
        return mNativeMapView.getLatLng();
    }

    public void setCenterCoordinate(LatLng centerCoordinate) {
        setCenterCoordinate(centerCoordinate, false);
    }

    public void setCenterCoordinate(LatLng centerCoordinate, boolean animated) {
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.setLatLng(centerCoordinate, duration);
    }

    public void setCenterCoordinate(LatLngZoom centerCoordinate) {
        setCenterCoordinate(centerCoordinate, false);
    }

    public void setCenterCoordinate(LatLngZoom centerCoordinate,
                                    boolean animated) {
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.setLatLngZoom(centerCoordinate, duration);
    }

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

    public void setDirection(double direction) {
        setDirection(direction, false);
    }

    public void setDirection(double direction, boolean animated) {
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.setBearing(-direction, duration);
    }

    public void resetPosition() {
        mNativeMapView.resetPosition();
    }

    public void resetNorth() {
        mNativeMapView.resetNorth();
    }

    public double getZoomLevel() {
        return mNativeMapView.getZoom();
    }

    public void setZoomLevel(double zoomLevel) {
        setZoomLevel(zoomLevel, false);
    }

    public void setZoomLevel(double zoomLevel, boolean animated) {
        long duration = animated ? ANIMATION_DURATION : 0;
        mNativeMapView.setZoom(zoomLevel, duration);
    }

    public boolean isZoomEnabled() {
        return mZoomEnabled;
    }

    public void setZoomEnabled(boolean zoomEnabled) {
        this.mZoomEnabled = zoomEnabled;

        if ((mZoomButtonsController != null) && (getVisibility() == View.VISIBLE) && mZoomEnabled) {
            mZoomButtonsController.setVisible(true);
        }
    }

    public boolean isScrollEnabled() {
        return mScrollEnabled;
    }

    public void setScrollEnabled(boolean scrollEnabled) {
        this.mScrollEnabled = scrollEnabled;
    }

    public boolean isRotateEnabled() {
        return mRotateEnabled;
    }

    public void setRotateEnabled(boolean rotateEnabled) {
        this.mRotateEnabled = rotateEnabled;
    }

    public boolean isDebugActive() {
        return mNativeMapView.getDebug() || mNativeMapView.getCollisionDebug();
    }

    public void setDebugActive(boolean debugActive) {
        mNativeMapView.setDebug(debugActive);
        mNativeMapView.setCollisionDebug(debugActive);
    }

    public void toggleDebug() {
        mNativeMapView.toggleDebug();
        mNativeMapView.toggleCollisionDebug();
    }

    public UserLocationTrackingMode getUserLocationTrackingMode() {
        return mUserLocationTrackingMode;
    }

    public void setUserLocationTrackingMode(UserLocationTrackingMode userLocationTrackingMode) {
        this.mUserLocationTrackingMode = userLocationTrackingMode;
    }

    public boolean isFullyLoaded() {
        return mNativeMapView.isFullyLoaded();
    }

    private void validateStyleUrl(String url) {
        url = url.replaceFirst("asset://", "http://");
        HttpUrl parsedUrl = HttpUrl.parse(url);
        if (parsedUrl == null) {
            throw new RuntimeException("Style URL is not a valid http, https or asset URL.");
        }
    }

    public void setStyleUrl(String url) {
        validateStyleUrl(url);
        mStyleUrl = url;
        mNativeMapView.setStyleUrl(url);
    }

    public String getStyleUrl() {
        return mStyleUrl;
    }

    private void validateAccessToken(@NonNull String accessToken) {

        if (TextUtils.isEmpty(accessToken) || (!accessToken.startsWith("pk.") && !accessToken.startsWith("sk."))) {
            throw new RuntimeException("Using MapView requires setting a valid access token. See the README.md");
        }
    }

    public void setAccessToken(@NonNull String accessToken) {
        validateAccessToken(accessToken);
        mNativeMapView.setAccessToken(accessToken);
    }

    public String getAccessToken() {
        return mNativeMapView.getAccessToken();
    }

    public List<String> getStyleClasses() {
        return Collections.unmodifiableList(mNativeMapView.getClasses());
    }

    public void setStyleClasses(List<String> styleClasses) {
        setStyleClasses(styleClasses, 0);
    }

    public void setStyleClasses(List<String> styleClasses, long transitionDuration) {
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        mNativeMapView.setClasses(styleClasses);
    }

    public void addStyleClass(String styleClass) {
        mNativeMapView.addClass(styleClass);
    }

    public void removeStyleClass(String styleClass) {
        mNativeMapView.removeClass(styleClass);
    }

    public boolean hasStyleClass(String styleClass) {
        return mNativeMapView.hasClass(styleClass);
    }

    public void removeAllStyleClasses() {
        removeAllStyleClasses(0);
    }

    public void removeAllStyleClasses(long transitionDuration) {
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        ArrayList<String> styleClasses = new ArrayList<>(0);
        setStyleClasses(styleClasses);
    }

    public LatLng fromScreenLocation(PointF point) {
        return mNativeMapView.latLngForPixel(new PointF(point.x / mScreenDensity, point.y / mScreenDensity));
    }

    public PointF toScreenLocation(LatLng location) {
        PointF point = mNativeMapView.pixelForLatLng(location);
        return new PointF(point.x * mScreenDensity, point.y * mScreenDensity);
    }

    //
    // Lifecycle events
    //

    // Called when we need to restore instance state
    // Must be called from Activity onCreate
    public void onCreate(Bundle savedInstanceState) {
        if (savedInstanceState != null) {
            setCenterCoordinate((LatLng) savedInstanceState.getParcelable(STATE_CENTER_COORDINATE));
            setZoomLevel(savedInstanceState.getDouble(STATE_ZOOM_LEVEL)); // need to set zoom level first because of limitation on rotating when zoomed out
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
            mNativeMapView.setDefaultTransitionDuration(savedInstanceState.getLong(STATE_DEFAULT_TRANSITION_DURATION));
            setCompassEnabled(savedInstanceState.getBoolean(STATE_COMPASS_ENABLED));
            setMyLocationEnabled(savedInstanceState.getBoolean(STATE_MY_LOCATION_ENABLED));
            setUserLocationTrackingMode((UserLocationTrackingMode) savedInstanceState.getSerializable(STATE_USER_LOCATION_TRACKING_MODE));
        }

        // Force a check for an access token
        validateAccessToken(getAccessToken());

        mNativeMapView.initializeDisplay();
        mNativeMapView.initializeContext();

        addOnMapChangedListener(new OnMapChangedListener() {
            @Override
            public void onMapChanged(MapChange change) {
                updateMap(change);
            }
        });
    }

    // Called when we need to save instance state
    // Must be called from Activity onSaveInstanceState
    public void onSaveInstanceState(Bundle outState) {
        outState.putParcelable(STATE_CENTER_COORDINATE, getCenterCoordinate());
        outState.putDouble(STATE_ZOOM_LEVEL, getZoomLevel()); // need to set zoom level first because of limitation on rotating when zoomed out
        outState.putDouble(STATE_CENTER_DIRECTION, getDirection());
        outState.putBoolean(STATE_ZOOM_ENABLED, mZoomEnabled);
        outState.putBoolean(STATE_SCROLL_ENABLED, mScrollEnabled);
        outState.putBoolean(STATE_ROTATE_ENABLED, mRotateEnabled);
        outState.putBoolean(STATE_DEBUG_ACTIVE, isDebugActive());
        outState.putString(STATE_STYLE_URL, mStyleUrl);
        outState.putString(STATE_ACCESS_TOKEN, getAccessToken());
        outState.putStringArrayList(STATE_STYLE_CLASSES, new ArrayList<>(getStyleClasses()));
        outState.putLong(STATE_DEFAULT_TRANSITION_DURATION, mNativeMapView.getDefaultTransitionDuration());
        outState.putBoolean(STATE_COMPASS_ENABLED, isCompassEnabled());
        outState.putBoolean(STATE_MY_LOCATION_ENABLED, isMyLocationEnabled());
        outState.putSerializable(STATE_USER_LOCATION_TRACKING_MODE, getUserLocationTrackingMode());
    }

    // Called when we need to clean up
    // Must be called from Activity onDestroy
    public void onDestroy() {
        mNativeMapView.terminateContext();
        mNativeMapView.terminateDisplay();
    }

    // Called when we need to create the GL context
    // Must be called from Activity onStart
    public void onStart() {
    }

    // Called when we need to terminate the GL context
    // Must be called from Activity onPause
    public void onStop() {
    }

    // Called when we need to stop the render thread
    // Must be called from Activity onPause
    public void onPause() {
        // Register for connectivity changes
        getContext().unregisterReceiver(mConnectivityReceiver);
        mConnectivityReceiver = null;

        if (mIsMyLocationEnabled) {
            toggleGps(false);
        };

        mNativeMapView.pause();
    }

    // Called when we need to start the render thread
    // Must be called from Activity onResume

    public void onResume() {
        // Register for connectivity changes
        mConnectivityReceiver = new ConnectivityReceiver();
        mContext.registerReceiver(mConnectivityReceiver, new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));

        if (mIsMyLocationEnabled) {
            toggleGps(true);
        }

        mNativeMapView.resume();
    }

    public void onSizeChanged(int width, int height, int oldw, int oldh) {
        if (!isInEditMode()) {
            mNativeMapView.resizeView((int) (width / mScreenDensity), (int) (height / mScreenDensity));
        }
    }

    // This class handles SurfaceHolder callbacks
    private class CallbacksHandler implements SurfaceHolder.Callback, SurfaceHolder.Callback2 {

        // Called when the native surface buffer has been created
        // Must do all EGL/GL ES initialization here
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mNativeMapView.createSurface(holder.getSurface());
        }

        // Called when the native surface buffer has been destroyed
        // Must do all EGL/GL ES destruction here
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            mNativeMapView.destroySurface();
        }

        // Called when the format or size of the native surface buffer has been
        // changed
        // Must handle window resizing here.
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mNativeMapView.resizeFramebuffer(width, height);
        }

        // Called when we need to redraw the view
        // This is called before our view is first visible to prevent an initial
        // flicker (see Android SDK documentation)
        @Override
        public void surfaceRedrawNeeded(SurfaceHolder holder) {
            mNativeMapView.update();
        }
    }

    // Called when view is no longer connected
    @Override
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

    // Called when the system is running low on memory
    // Must be called from Activity onLowMemory
    public void onLowMemory() {
        mNativeMapView.onLowMemory();
    }

    //
    // Draw events
    //

    //
    // Input events
    //

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
            // TODO two finger tap zoom out
            mNativeMapView.scaleBy(0.5, x / mScreenDensity, y / mScreenDensity, ANIMATION_DURATION);
        }
    }

    // Called when user touches the screen, all positions are absolute
    @Override
    public boolean onTouchEvent(@NonNull MotionEvent event) {
        // Check and ignore non touch or left clicks
        if ((android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH) && (event.getButtonState() != 0) && (event.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
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
                mNativeMapView.setGestureInProgress(false);

                if (mTwoTap && isTap && !inProgress) {
                    PointF focalPoint = TwoFingerGestureDetector.determineFocalPoint(event);
                    zoom(false, focalPoint.x, focalPoint.y);
                    mTwoTap = false;
                    return true;
                }

                mTwoTap = false;
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

            // Select or deselect point annotations
            PointF tapPoint = new PointF(e.getX(), e.getY());

            float toleranceWidth = 60 * mScreenDensity;
            float toleranceHeight = 80 * mScreenDensity;

            PointF tr = new PointF(tapPoint.x + toleranceWidth / 2, tapPoint.y + 2 * toleranceHeight / 3);
            PointF bl = new PointF(tapPoint.x - toleranceWidth / 2, tapPoint.y - 1 * toleranceHeight / 3);

            LatLng sw = fromScreenLocation(bl);
            LatLng ne = fromScreenLocation(tr);

            BoundingBox bbox = new BoundingBox(ne, sw);

            List<Annotation> annotations = getAnnotationsInBounds(bbox);

            performClick();

            return true;
        }

        // Called for single taps after a delay
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            return false;
        }

        // Called for a long press
        @Override
        public void onLongPress(MotionEvent e) {
            // TODO
            performLongClick();
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

            if(onFlingListener != null){
                onFlingListener.onFling();
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

            if(onScrollListener != null){
                onScrollListener.onScroll();
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
    @Override @TargetApi(12)
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
    // Action events
    //

    // This class handles connectivity changes
    public class ConnectivityReceiver extends BroadcastReceiver {

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
    // Accessibility events
    //

    //
    // Map events
    //

    // Called when the map needs to be rerendered
    // Called via JNI from NativeMapView
    protected void onInvalidate() {
        post(new Runnable() {
            @Override
            public void run() {
                mNativeMapView.invalidate();
            }
        });
    }


    /**
     * Defines callback for events OnMapChange
     */
    public interface OnMapChangedListener {
        void onMapChanged(MapChange change);
    }

    /**
     * Add an OnMapChangedListner
     * @param listener Listener to add
     */
    public void addOnMapChangedListener(@NonNull OnMapChangedListener listener) {
        if (listener != null) {
            mOnMapChangedListener.add(listener);
        }
    }

    /**
     * Remove an OnMapChangedListener
     * @param listener Listener to remove
     */
    public void removeOnMapChangedListener(@NonNull OnMapChangedListener listener) {
        if (listener != null) {
            mOnMapChangedListener.remove(listener);
        }
    }

    // Called when the map view transformation has changed
    // Called via JNI from NativeMapView
    // Need to update anything that relies on map state
    protected void onMapChanged(int rawChange) {
        final MapChange change = MapChange.fromInteger(rawChange);
        if (mOnMapChangedListener != null) {
            post(new Runnable() {
                @Override
                public void run() {
                    for (OnMapChangedListener listener : mOnMapChangedListener) {
                        listener.onMapChanged(change);
                    }
                }
            });
        }
    }

    public interface OnFpsChangedListener {
        void onFpsChanged(double fps);
    }

    private OnFpsChangedListener mOnFpsChangedListener;

    // Adds a listener for onFpsChanged
    public void setOnFpsChangedListener(OnFpsChangedListener listener) {
        mOnFpsChangedListener = listener;
    }

    // Called when debug mode is enabled to update a FPS counter
    // Called via JNI from NativeMapView
    protected void onFpsChanged(final double fps) {
        if (mOnFpsChangedListener != null) {
            post(new Runnable() {
                @Override
                public void run() {
                    mOnFpsChangedListener.onFpsChanged(fps);
                }
            });
        }
    }

    // Google Maps Inspired API

    /**
     * Gets the status of the my-location layer.
     * @return True if the my-location layer is enabled, false otherwise.
     */
    public final boolean isMyLocationEnabled () {
        return mIsMyLocationEnabled;
    }

    /**
     * Enables or disables the my-location layer.
     * While enabled, the my-location layer continuously draws an indication of a user's current
     * location and bearing, and displays UI controls that allow a user to interact with their
     * location (for example, to enable or disable camera tracking of their location and bearing).
     * @param enabled True to enable; false to disable.
     */
    public final void setMyLocationEnabled (boolean enabled) {
        mIsMyLocationEnabled = enabled;
        toggleGps(enabled);
        updateMap(MapChange.MapChangeNullChange);
    }

    /**
     * Returns the currently displayed user location, or null if there is no location data available.
     * @return The currently displayed user location.
     */
    public final Location getMyLocation () {
        return mGpsLocation;
    }

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {
        if (enableGps) {
            if (!mLocationClient.isConnected()) {
                mGpsLocation = null;
                mLocationClient.connect();
                updateLocation(LocationServices.FusedLocationApi.getLastLocation());
                LocationServices.FusedLocationApi.requestLocationUpdates(mLocationRequest, this);
                mSensorManager.registerListener(mCompassListener, mSensorAccelerometer, SensorManager.SENSOR_DELAY_UI);
                mSensorManager.registerListener(mCompassListener, mSensorMagneticField, SensorManager.SENSOR_DELAY_UI);
            }
        } else {
            if (mLocationClient.isConnected()) {
                LocationServices.FusedLocationApi.removeLocationUpdates(this);
                mLocationClient.disconnect();
                mGpsLocation = null;
                mSensorManager.unregisterListener(mCompassListener, mSensorAccelerometer);
                mSensorManager.unregisterListener(mCompassListener, mSensorMagneticField);
            }
        }
    }

    /**
     * Gets whether the compass is enabled/disabled.
     * @return true if the compass is enabled; false if the compass is disabled.
     */
    public boolean isCompassEnabled () {
        return mIsCompassEnabled;
    }

    /**
     * Enables or disables the compass. The compass is an icon on the map that indicates the
     * direction of north on the map. If enabled, it is only shown when the camera is tilted or
     * rotated away from its default orientation (tilt of 0 and a bearing of 0). When a user clicks
     * the compass, the camera orients itself to its default orientation and fades away shortly
     * after. If disabled, the compass will never be displayed.
     *
     * By default, the compass is enabled
     * @param compassEnabled true to enable the compass; false to disable the compass.
     */
    public void setCompassEnabled (boolean compassEnabled) {
        // Set value
        this.mIsCompassEnabled = compassEnabled;

        // Toggle UI
        if (mIsCompassEnabled) {
            mCompassView.setVisibility(View.VISIBLE);
        } else {
            mCompassView.setVisibility(View.GONE);
        }

        // Update Map
        updateMap(MapChange.MapChangeNullChange);
    }

    // This class handles sensor updates to calculate compass bearing
    private class CompassListener implements SensorEventListener {

        @Override
        public void onSensorChanged(SensorEvent event) {
            switch (event.sensor.getType()) {
                case Sensor.TYPE_ACCELEROMETER:
                    System.arraycopy(event.values, 0, mValuesAccelerometer, 0, 3);
                    break;
                case Sensor.TYPE_MAGNETIC_FIELD:
                    System.arraycopy(event.values, 0, mValuesMagneticField, 0, 3);
                    break;
            }

            boolean valid = SensorManager.getRotationMatrix(mMatrixR, mMatrixI,
                    mValuesAccelerometer,
                    mValuesMagneticField);

            if (valid) {
                SensorManager.getOrientation(mMatrixR, mMatrixValues);
                //mAzimuthRadians.putValue(mMatrixValues[0]);
                //mAzimuth = Math.toDegrees(mAzimuthRadians.getAverage());

                Location mGpsLocation = getMyLocation();
                if (mGpsLocation != null) {
                    GeomagneticField geomagneticField = new GeomagneticField(
                            (float) mGpsLocation.getLatitude(),
                            (float) mGpsLocation.getLongitude(),
                            (float) mGpsLocation.getAltitude(),
                            System.currentTimeMillis());
                    mCompassBearing = (float) Math.toDegrees(mMatrixValues[0]) + geomagneticField.getDeclination();
                    mCompassValid = true;
                }
            }

            updateMap(MapChange.MapChangeNullChange);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            // TODO: ignore unreliable stuff
        }
    }

    // Called when someone presses the compass
    private class CompassOnClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            resetNorth();
        }

    }

    /**
     * LOST's LocationListener Callback
     * @param location New Location
     */
    @Override
    public void onLocationChanged(Location location) {
        updateLocation(location);
    }

    // Handles location updates from GPS
    private void updateLocation(Location location) {
        if (location != null) {
            mGpsLocation = location;
            updateMap(MapChange.MapChangeNullChange);
        }
    }

    // Rotates an ImageView - does not work if the ImageView has padding, use margins
    private void rotateImageView(ImageView imageView, float angle) {
        Matrix matrix = new Matrix();
        matrix.setScale((float) imageView.getWidth() / (float) imageView.getDrawable().getIntrinsicWidth(), (float) imageView.getHeight() / (float) imageView.getDrawable().getIntrinsicHeight());
        matrix.postRotate(angle, (float) imageView.getWidth() / 2.0f, (float) imageView.getHeight() / 2.0f);
        imageView.setImageMatrix(matrix);
        imageView.setScaleType(ImageView.ScaleType.MATRIX);
    }

    // Updates the UI to match the current map's position
    private void updateMap(MapChange change) {
        // Using direct access to mIsCompassEnabled instead of isCompassEnabled() for
        // small performance boost as this method is called rapidly.
        if (mIsCompassEnabled) {
            rotateImageView(mCompassView, (float) getDirection());
        }

        if (isMyLocationEnabled() && mGpsLocation != null) {
            if (mGpsMarker == null) {
                // Setup User Location UI
                // NOTE: mIsMyLocationEnabled == false to begin with
                mGpsMarker = new ImageView(getContext());
                mGpsMarker.setImageResource(R.drawable.location_marker);
                addView(mGpsMarker);
            }

            mGpsMarker.setVisibility(View.VISIBLE);
            LatLng coordinate = new LatLng(mGpsLocation);
            PointF screenLocation = toScreenLocation(coordinate);

            float iconSize = 27.0f * mScreenDensity;
            // Update Location
            FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) iconSize, (int) iconSize);
            lp.leftMargin = (int) (screenLocation.x - iconSize / 2.0f);
            lp.topMargin = getHeight() - (int) (screenLocation.y + iconSize / 2.0f);
            mGpsMarker.setLayoutParams(lp);
            rotateImageView(mGpsMarker, 0.0f);
            mGpsMarker.requestLayout();

            // Update direction if tracking mode
            if(mUserLocationTrackingMode == UserLocationTrackingMode.FOLLOW_BEARING && mCompassValid){
                // TODO need to do proper filtering (see branch filter-compass) or else map will lock up because of all the compass events
                long t = new Date().getTime();
                if((t-t0)>1000){
                    t0 = t;
                    setDirection(-mCompassBearing, true);
                }
            }

/*
            // TODO - Too much overhead on main thread.  Needs to be refactored before it
            // can be re-enabled
            // Update map position if NOT in NONE mode
            if (mUserLocationTrackingMode != UserLocationTrackingMode.NONE) {
                setCenterCoordinate(new LatLng(mGpsLocation));
            }
*/

/*
            // Used For User Location Bearing UI
            if (mGpsLocation.hasBearing() || mCompassValid) {
                mGpsMarker.setImageResource(R.drawable.direction_arrow);
                float iconSize = 54.0f * mScreenDensity;
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) iconSize, (int) iconSize);
                lp.leftMargin = (int) (screenLocation.x - iconSize / 2.0f);
                lp.topMargin = mMapFrameLayout.getHeight() - (int) (screenLocation.y + iconSize / 2.0f);
                mGpsMarker.setLayoutParams(lp);
                float bearing = mGpsLocation.hasBearing() ? mGpsLocation.getBearing() : mCompassBearing;
                rotateImageView(mGpsMarker, bearing);
                mGpsMarker.requestLayout();
            }
*/
        } else {
            if (mGpsMarker != null) {
                mGpsMarker.setVisibility(View.INVISIBLE);
            }
        }
    }
}
