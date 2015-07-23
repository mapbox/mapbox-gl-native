package com.mapbox.mapboxgl.views;

import android.annotation.TargetApi;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.view.GestureDetector;
import android.view.ScaleGestureDetector;
import android.util.AttributeSet;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewConfiguration;
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
import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.geometry.LatLngZoom;

import org.apache.commons.validator.routines.UrlValidator;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

// Custom view that shows a Map
// Based on SurfaceView as we use OpenGL ES to render
public class MapView extends SurfaceView {

    //
    // Static members
    //

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
    private static final String STATE_CLASSES = "classes";
    private static final String STATE_DEFAULT_TRANSITION_DURATION = "defaultTransitionDuration";

    /**
     * Every annotation that has been added to the map.
     */
    private List<Annotation> annotations = new ArrayList<>();

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
        mNativeMapView = new NativeMapView(this, cachePath, dataPath, apkPath, mScreenDensity);

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
        } finally {
            typedArray.recycle();
        }

        // Ensure this view is interactable
        setClickable(true);
        setLongClickable(true);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();

        // Register the SurfaceHolder callbacks
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
            getHolder().addCallback(new Callbacks2());
        } else {
            getHolder().addCallback(new Callbacks());
        }

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
    }

    public void setSprite(String symbol, float scale, Bitmap bitmap) {
        if(bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
            bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, false);
        }
        ByteBuffer buffer = ByteBuffer.allocate(bitmap.getByteCount());
        bitmap.copyPixelsToBuffer(buffer);

        mNativeMapView.setSprite(symbol, bitmap.getWidth(), bitmap.getHeight(), scale, buffer.array());
    }

    public Marker addMarker(MarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Long id = mNativeMapView.addMarker(marker);
        marker.setId(id); // the annotation needs to know its id
        marker.setMapView(this); // the annotation needs to know which map view it is in
        annotations.add(marker);
        return marker;
    }

    public Polyline addPolyline(PolylineOptions polylineOptions) {
        Polyline polyline = polylineOptions.getPolyline();
        Long id = mNativeMapView.addPolyline(polyline);
        polyline.setId(id);
        polyline.setMapView(this);
        annotations.add(polyline);
        return polyline;
    }

    public Polygon addPolygon(PolygonOptions polygonOptions) {
        Polygon polygon = polygonOptions.getPolygon();
        Long id = mNativeMapView.addPolygon(polygon);
        polygon.setId(id);
        polygon.setMapView(this);
        annotations.add(polygon);
        return polygon;
    }

    public List<Polygon> addPolygons(List<PolygonOptions> polygonOptions) {
        List<Polygon> polygons = new ArrayList<>();
        for(PolygonOptions popts : polygonOptions) {
            polygons.add(popts.getPolygon());
        }

        long[] ids = mNativeMapView.addPolygons(polygons);

        for(int i=0; i<polygons.size(); i++) {
            polygons.get(i).setId(ids[i]);
            polygons.get(i).setMapView(this);
            annotations.add(polygons.get(i));
        }

        return polygons;
    }

    public void removeAnnotation(Annotation annotation) {
        long id = annotation.getId();
        mNativeMapView.removeAnnotation(id);
    }

    public void removeAnnotation(long annotationId) {
        mNativeMapView.removeAnnotation(annotationId);
    }

    public void removeAnnotations() {
        long[] ids = new long[annotations.size()];
        for(int i=0; i<annotations.size(); i++) {
            Long id = annotations.get(i).getId();
            if(id == null) {
                ids[i] = -1;
            } else {
                ids[i] = id;
            }
        }
        mNativeMapView.removeAnnotations(ids);
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
        return mNativeMapView.getDebug();
    }

    public void setDebugActive(boolean debugActive) {
        mNativeMapView.setDebug(debugActive);
    }

    public void toggleDebug() {
        mNativeMapView.toggleDebug();
    }

    private void validateStyleUrl(String url) {
        String[] schemes = {"http", "https", "file", "asset"};
        UrlValidator urlValidator = new UrlValidator(schemes, UrlValidator.NO_FRAGMENTS | UrlValidator.ALLOW_LOCAL_URLS);
        if (!urlValidator.isValid(url)) {
            throw new RuntimeException("Style URL is not a valid http, https, file or asset URL.");
        }
    }

    public void setStyleUrl(String url) {
        mStyleUrl = url;
        mNativeMapView.setStyleUrl(url);
    }

    public String getStyleUrl() {
        return mStyleUrl;
    }

    private void validateAccessToken(String accessToken) {

        if (!accessToken.startsWith("pk.") && !accessToken.startsWith("sk.")) {
            throw new RuntimeException("Using MapView requires setting a valid access token. See the README.md");
        }
    }

    public void setAccessToken(String accessToken) {
        validateAccessToken(accessToken);
        mNativeMapView.setAccessToken(accessToken);
    }

    public String getAccessToken() {
        return mNativeMapView.getAccessToken();
    }

    public List<String> getClasses() {
        return mNativeMapView.getClasses();
    }

    public void setClasses(List<String> classes) {
        setClasses(classes, 0);
    }

    public void setClasses(List<String> classes, long transitionDuration) {
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        mNativeMapView.setClasses(classes);
    }

    public void addClass(String clazz) {
        mNativeMapView.addClass(clazz);
    }

    public void removeClass(String clazz) {
        mNativeMapView.removeClass(clazz);
    }

    public boolean hasClass(String clazz) {
        return mNativeMapView.hasClass(clazz);
    }

    public void removeAllClasses() {
        removeAllClasses(0);
    }

    public void removeAllClasses(long transitionDuration) {
        mNativeMapView.setDefaultTransitionDuration(transitionDuration);
        ArrayList<String> classes = new ArrayList<>(0);
        setClasses(classes);
    }

    public LatLng fromScreenLocation(PointF point) {
        return mNativeMapView.latLngForPixel(point);
    }

    public PointF toScreenLocation(LatLng location) {
        return mNativeMapView.pixelForLatLng(location);
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
            List<String> appliedClasses = savedInstanceState.getStringArrayList(STATE_CLASSES);
            if (!appliedClasses.isEmpty()) {
                setClasses(appliedClasses);
            }
            mNativeMapView.setDefaultTransitionDuration(savedInstanceState.getLong(STATE_DEFAULT_TRANSITION_DURATION));
        }

        validateAccessToken(getAccessToken());

        mNativeMapView.initializeDisplay();
        mNativeMapView.initializeContext();
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
        outState.putStringArrayList(STATE_CLASSES, new ArrayList<>(getClasses()));
        outState.putLong(STATE_DEFAULT_TRANSITION_DURATION, mNativeMapView.getDefaultTransitionDuration());
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
        // Do nothing
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

        mNativeMapView.pause();
    }

    // Called when we need to start the render thread
    // Must be called from Activity onResume

    public void onResume() {
        // Register for connectivity changes
        mConnectivityReceiver = new ConnectivityReceiver();
        mContext.registerReceiver(mConnectivityReceiver, new IntentFilter(ConnectivityManager.CONNECTIVITY_ACTION));

        mNativeMapView.resume();
    }

    public void onSizeChanged(int width, int height, int oldw, int oldh) {
        mNativeMapView.resizeView((int)(width / mScreenDensity), (int)(height / mScreenDensity));
    }

    // This class handles SurfaceHolder callbacks
    private class Callbacks implements SurfaceHolder.Callback {

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
    }

    @TargetApi(9)
    private class Callbacks2 extends Callbacks implements SurfaceHolder.Callback2 {

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
    @Override @TargetApi(14)
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
                break;

            case MotionEvent.ACTION_CANCEL:
                mTwoTap = false;
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

        // Called for single taps after a delay
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            return false;
        }

        // Called for a long press
        @Override
        public void onLongPress(MotionEvent e) {
            // TODO
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
    @Override @TargetApi(14)
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
                boolean inProgress = mRotateGestureDetector.isInProgress() || mScaleGestureDetector.isInProgress();
                mNativeMapView.invalidate(inProgress);
            }
        });
    }


    public interface OnMapChangedListener {
        void onMapChanged();
    }

    private OnMapChangedListener mOnMapChangedListener;

    // Adds a listener for onMapChanged
    public void setOnMapChangedListener(OnMapChangedListener listener) {
        mOnMapChangedListener = listener;
    }

    // Called when the map view transformation has changed
    // Called via JNI from NativeMapView
    // Need to update anything that relies on map state
    protected void onMapChanged() {
        if (mOnMapChangedListener != null) {
            post(new Runnable() {
                @Override
                public void run() {
                    mOnMapChangedListener.onMapChanged();
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
}
