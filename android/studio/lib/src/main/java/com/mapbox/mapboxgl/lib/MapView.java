package com.mapbox.mapboxgl.lib;

import java.io.IOException;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.IOUtils;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.TypedArray;
import android.graphics.PointF;
import android.os.Bundle;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.ZoomButtonsController;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;

// Custom view that shows a Map
// Based on SurfaceView as we use OpenGL ES to render
public class MapView extends SurfaceView {
    // TODO try TextureView

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapView";

    // Used for saving instance state
    private static final String STATE_CENTER_COORDINATE = "centerCoordinate";
    private static final String STATE_CENTER_DIRECTION = "centerDirection";
    private static final String STATE_ZOOM_LEVEL = "zoomLevel";
    private static final String STATE_ZOOM_ENABLED = "zoomEnabled";
    private static final String STATE_SCROLL_ENABLED = "scrollEnabled";
    private static final String STATE_ROTATE_ENABLED = "rotateEnabled";
    private static final String STATE_DEBUG_ACTIVE = "debugActive";

    //
    // Instance members
    //

    // Used to call JNI NativeMapView
    private NativeMapView mNativeMapView;

    // Used to style the map
    private String mDefaultStyleJSON;

    // Touch gesture detectors
    private GestureDetector mGestureDetector;
    private ScaleGestureDetector mScaleGestureDetector;
    private RotateGestureDetector mRotateGestureDetector;
    private boolean mTwoTap = false;

    // Shows zoom buttons
    private ZoomButtonsController mZoomButtonsController;

    // Used to track trackball long presses
    private TrackballLongPressTimeOut mCurrentTrackballLongPressTimeOut;

    //
    // Properties
    //

    private boolean mZoomEnabled = true;
    private boolean mScrollEnabled = true;
    private boolean mRotateEnabled = true;

    //
    // Constructors
    //

    // Called when no properties are being set from XML
    public MapView(Context context) {
        super(context);
        initialize(context, null, 0);
    }

    // Called when properties are being set from XML
    public MapView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context, attrs, 0);
    }

    // Called when properties are being set from XML
    public MapView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initialize(context, attrs, defStyle);
    }

    //
    // Initialization
    //

    // Common initialization code goes here
    private void initialize(Context context, AttributeSet attrs, int defStyle) {
        Log.v(TAG, "initialize");

        // Check if we are in Eclipse UI editor
        if (isInEditMode()) {
            // TODO editor does not load properly because
            return;
        }

        // Load the map style
        try {
            mDefaultStyleJSON = IOUtils.toString(context.getResources()
            // .openRawResource(R.raw.style_leith), Charsets.UTF_8);
                    .openRawResource(R.raw.style), Charsets.UTF_8);
        } catch (IOException e) {
            throw new RuntimeException(
                    "Couldn't load default style JSON resource", e);
        }

        // Create the NativeMapView
        mNativeMapView = new NativeMapView(this, mDefaultStyleJSON);
        mNativeMapView.addDefaultSource();
        // mNativeMapView.addSource("countries",
        // "http://192.168.20.103/%d/%d/%d.pbf");

        // Load the attributes
        TypedArray typedArray = context.obtainStyledAttributes(attrs,
                R.styleable.MapView, 0, 0);
        try {
            double centerLongitude = typedArray.getFloat(
                    R.styleable.MapView_centerLongitude, 0.0f);
            double centerLatitude = typedArray.getFloat(
                    R.styleable.MapView_centerLatitude, 0.0f);
            LonLat centerCoordinate = new LonLat(centerLongitude,
                    centerLatitude);
            setCenterCoordinate(centerCoordinate);
            setDirection(typedArray.getFloat(R.styleable.MapView_direction,
                    0.0f));
            setZoomLevel(typedArray.getFloat(R.styleable.MapView_zoomLevel,
                    0.0f));
            setZoomEnabled(typedArray.getBoolean(
                    R.styleable.MapView_zoomEnabled, true));
            setScrollEnabled(typedArray.getBoolean(
                    R.styleable.MapView_scrollEnabled, true));
            setRotateEnabled(typedArray.getBoolean(
                    R.styleable.MapView_rotateEnabled, true));
            setDebugActive(typedArray.getBoolean(
                    R.styleable.MapView_debugActive, false));
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
        getHolder().addCallback(new Callbacks());

        // Touch gesture detectors
        mGestureDetector = new GestureDetector(context, new GestureListener());
        mGestureDetector.setIsLongpressEnabled(true);
        mScaleGestureDetector = new ScaleGestureDetector(context,
                new ScaleGestureListener());
        mRotateGestureDetector = new RotateGestureDetector(context,
                new RotateGestureListener());

        // Shows the zoom controls
        // But not when in Eclipse UI editor
        if (!isInEditMode()) {
            if (!context.getPackageManager().hasSystemFeature(
                    PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
                mZoomButtonsController = new ZoomButtonsController(this);
                mZoomButtonsController.setZoomSpeed(300);
                mZoomButtonsController.setOnZoomListener(new OnZoomListener());
            }
        }
    }

    //
    // Property methods
    //

    public LonLat getCenterCoordinate() {
        return mNativeMapView.getLonLat();
    }

    public void setCenterCoordinate(LonLat centerCoordinate) {
        setCenterCoordinate(centerCoordinate, false);
    }

    public void setCenterCoordinate(LonLat centerCoordinate, boolean animated) {
        double duration = animated ? 0.3 : 0.0;
        mNativeMapView.setLonLat(centerCoordinate, duration);
    }

    public void setCenterCoordinate(LonLatZoom centerCoordinate) {
        setCenterCoordinate(centerCoordinate, false);
    }

    public void setCenterCoordinate(LonLatZoom centerCoordinate,
            boolean animated) {
        double duration = animated ? 0.3 : 0.0;
        mNativeMapView.setLonLatZoom(centerCoordinate, duration);
    }

    public double getDirection() {
        double direction = mNativeMapView.getAngle();

        direction *= 180 / Math.PI;

        while (direction > 360)
            direction -= 360;
        while (direction < 0)
            direction += 360;

        return direction;
    }

    public void setDirection(double direction) {
        setDirection(direction, false);
    }

    public void setDirection(double direction, boolean animated) {
        double duration = animated ? 0.3 : 0.0;

        direction *= Math.PI / 180;

        mNativeMapView.setAngle(direction, duration);
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
        double duration = animated ? 0.3 : 0.0;
        mNativeMapView.setZoom(zoomLevel, duration);
    }

    public boolean isZoomEnabled() {
        return mZoomEnabled;
    }

    public void setZoomEnabled(boolean zoomEnabled) {
        this.mZoomEnabled = zoomEnabled;

        if ((mZoomButtonsController != null)
                && (getVisibility() == View.VISIBLE) && mZoomEnabled) {
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

    //
    // Style methods
    //

    public void toggleStyle() {
        // TODO
    }

    // TODO seems like JSON simple may be better since it implements Map
    // interface
    // Other candidates: fastjson, json-smart, fossnova json,
    public JSONObject getRawStyle() {
        try {
            return new JSONObject(mNativeMapView.getStyleJSON());
        } catch (JSONException e) {
            e.printStackTrace();
            return null;
        }
    }

    public void setRawStyle(JSONObject style) {
        mNativeMapView.setStyleJSON(style.toString());
    }

    public void getStyleOrderedLayerNames() {
        // TODO
    }

    public void setStyleOrderedLayerNames() {
        // TODO
    }

    public void getAllStyleClasses() {
        // TODO
    }

    public void getAppliedStyleClasses() {
        // TODO
    }

    public void setAppliedStyleClasses() {
        // TODO
    }

    public void getStyleDescriptionForLayer() {
        // TODO
    }

    public void setStyleDescription() {
        // TODO
    }

    //
    // Lifecycle events
    //

    // Called when we need to restore instance state
    // Must be called from Activity onCreate
    public void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "onCreate");
        if (savedInstanceState != null) {
            lonlat = (LonLat) savedInstanceState
                    .getParcelable(STATE_CENTER_COORDINATE);
            angle = savedInstanceState.getDouble(STATE_CENTER_DIRECTION);
            zoom = savedInstanceState.getDouble(STATE_ZOOM_LEVEL);
            setCenterCoordinate((LonLat) savedInstanceState
                    .getParcelable(STATE_CENTER_COORDINATE));
            setDirection(savedInstanceState.getDouble(STATE_CENTER_DIRECTION));
            setZoomLevel(savedInstanceState.getDouble(STATE_ZOOM_LEVEL));
            setZoomEnabled(savedInstanceState.getBoolean(STATE_ZOOM_ENABLED));
            setScrollEnabled(savedInstanceState
                    .getBoolean(STATE_SCROLL_ENABLED));
            setRotateEnabled(savedInstanceState
                    .getBoolean(STATE_ROTATE_ENABLED));
            setDebugActive(savedInstanceState.getBoolean(STATE_DEBUG_ACTIVE));
        }

        mNativeMapView.initializeDisplay();
    }

    // Called when we need to save instance state
    // Must be called from Activity onSaveInstanceState
    public void onSaveInstanceState(Bundle outState) {
        Log.v(TAG, "onSaveInstanceState");
        outState.putParcelable(STATE_CENTER_COORDINATE, getCenterCoordinate());
        outState.putDouble(STATE_CENTER_DIRECTION, getDirection());
        outState.putDouble(STATE_ZOOM_LEVEL, getZoomLevel());
        outState.putBoolean(STATE_ZOOM_ENABLED, isZoomEnabled());
        outState.putBoolean(STATE_SCROLL_ENABLED, isScrollEnabled());
        outState.putBoolean(STATE_ROTATE_ENABLED, isRotateEnabled());
        outState.putBoolean(STATE_DEBUG_ACTIVE, isDebugActive());
    }

    // Called when we need to clean up
    // Must be called from Activity onDestroy
    public void onDestroy() {
        Log.v(TAG, "onDestroy");
        mNativeMapView.terminateDisplay();
    }

    // Called when we need to create the GL context
    // Must be called from Activity onStart
    public void onStart() {
        Log.v(TAG, "onStart");
        mNativeMapView.initializeContext();
    }

    // Called when we need to terminate the GL context
    // Must be called from Activity onPause
    public void onStop() {
        Log.v(TAG, "onStop");
        mNativeMapView.terminateContext();
    }

    // Called when we need to stop the render thread
    // Must be called from Activity onPause
    public void onPause() {
        Log.v(TAG, "onPause");
        lonlat = mNativeMapView.getLonLat();
        angle = mNativeMapView.getAngle();
        zoom = mNativeMapView.getZoom();
        mNativeMapView.stop();
    }

    // Called when we need to start the render thread
    // Must be called from Activity onResume

    // TODO need to fix this in Map C++ code
    // Seems map state gets reset when we start()
    private LonLat lonlat;
    private double angle, zoom;

    public void onResume() {
        Log.v(TAG, "onResume");
        mNativeMapView.start();
        if (lonlat != null) {
            mNativeMapView.setLonLat(lonlat);
            mNativeMapView.setAngle(angle);
            mNativeMapView.setZoom(zoom);
        }
    }

    // This class handles SurfaceHolder callbacks
    private class Callbacks implements SurfaceHolder.Callback2 {

        // Called when we need to redraw the view
        // This is called before our view is first visible to prevent an initial
        // flicker (see Android SDK documentation)
        @Override
        public void surfaceRedrawNeeded(SurfaceHolder holder) {
            Log.v(TAG, "surfaceRedrawNeeded");
            mNativeMapView.update();
        }

        // Called when the native surface buffer has been created
        // Must do all EGL/GL ES initialization here
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            Log.v(TAG, "surfaceCreated");
            mNativeMapView.createSurface(holder.getSurface());
            if (lonlat != null) {
                mNativeMapView.setLonLat(lonlat);
                mNativeMapView.setAngle(angle);
                mNativeMapView.setZoom(zoom);
            }
        }

        // Called when the native surface buffer has been destroyed
        // Must do all EGL/GL ES destruction here
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            Log.v(TAG, "surfaceDestroyed");
            mNativeMapView.destroySurface();
        }

        // Called when the format or size of the native surface buffer has been
        // changed
        // Must handle window resizing here.
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width,
                int height) {
            Log.v(TAG, "surfaceChanged");
            Log.i(TAG, "resize " + format + " " + width + " " + height);
            mNativeMapView.resize(width, height);
            // TODO fix bug when rotating - sometimes 1/2 map black
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
    protected void onVisibilityChanged(View changedView, int visibility) {
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

    // TODO: onDraw for editor?
    // By default it just shows a gray screen with "MapView"
    // Not too important but perhaps we could put a static demo map image there

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
            mNativeMapView.scaleBy(2.0, x, y, 0.3);
        } else {
            // TODO two finger tap zoom out
            mNativeMapView.scaleBy(0.5, x, y, 0.3);
        }
    }

    // Called when user touches the screen, all positions are absolute
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // Check and ignore non touch or left clicks
        if ((event.getButtonState() != 0)
                && (event.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
            return false;
        }

        // Check two finger gestures first
        boolean rotateRetVal = false;
        mRotateGestureDetector.onTouchEvent(event);
        boolean scaleRetVal = false;
        mScaleGestureDetector.onTouchEvent(event);

        // Handle two finger tap
        switch (event.getActionMasked()) {
        case MotionEvent.ACTION_DOWN:
            // First pointer down
            break;

        case MotionEvent.ACTION_POINTER_DOWN:
            // Second pointer down
            if (event.getPointerCount() == 2) {
                mTwoTap = true;
            } else {
                mTwoTap = false;
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
                    || mScaleGestureDetector.isInProgress();

            if (mTwoTap && isTap && !inProgress) {
                PointF focalPoint = TwoFingerGestureDetector
                        .determineFocalPoint(event);
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

        // Do not change this code! It will break very easily.
        boolean retVal = rotateRetVal || scaleRetVal;
        retVal = mGestureDetector.onTouchEvent(event) || retVal;
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
            // TODO does not work
            /*
             * float ease = 0.25f;
             * 
             * velocityX = velocityX * ease; velocityY = velocityY * ease;
             * 
             * double speed = Math.sqrt(velocityX * velocityX + velocityY *
             * velocityY); double deceleration = 2500; double duration = speed /
             * (deceleration * ease);
             * 
             * 
             * // Cancel any animation mNativeMapView.cancelTransitions();
             * 
             * mNativeMapView.moveBy(velocityX * duration / 2.0, velocityY *
             * duration / 2.0, duration);
             * 
             * return true;
             */
            return false;
        }

        // Called for drags
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2,
                float distanceX, float distanceY) {
            if (!mScrollEnabled) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions(); // TODO need to test canceling
                                                // transitions with touch

            // Scroll the map
            mNativeMapView.moveBy(-distanceX, -distanceY);
            return true;
        }
    }

    // This class handles two finger gestures
    private class ScaleGestureListener extends
            ScaleGestureDetector.SimpleOnScaleGestureListener {

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

            // TODO complex decision between roate or scale or both (see Google
            // Maps app)

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Scale the map
            mNativeMapView.scaleBy(detector.getScaleFactor(),
                    detector.getFocusX(), detector.getFocusY());

            return true;
        }
    }

    // This class handles two rotate gestures
    // TODO need way to single finger rotate - need to research how google maps
    // does this - for phones with single touch, or when using mouse etc
    private class RotateGestureListener extends
            RotateGestureDetector.SimpleOnRotateGestureListener {

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
            Log.d("rotate", "rotate begin");
            return true;
        }

        // Called when the fingers leave the screen
        @Override
        public void onRotateEnd(RotateGestureDetector detector) {
            mBeginTime = 0;
            mTotalAngle = 0.0f;
            mStarted = false;
            Log.d("rotate", "rotate end");
        }

        // Called each time one of the two fingers moves
        // Called for rotation
        @Override
        public boolean onRotate(RotateGestureDetector detector) {
            if (!mRotateEnabled) {
                return false;
            }

            Log.d("rotate", "rotate evt");

            // If rotate is large enough ignore a tap
            // TODO: Google Maps seem to use a velocity rather than absolute
            // value, up to a point then they always rotate
            mTotalAngle += detector.getRotationDegreesDelta();
            Log.d("rotate", "ttl angle " + mTotalAngle);
            if ((mTotalAngle > 5.0f) || (mTotalAngle < -5.0f)) {
                mStarted = true;
                Log.d("rotate", "rotate started");
            }

            // Ignore short touches in case it is a tap
            // Also ignore small rotate
            long time = detector.getEventTime();
            long interval = time - mBeginTime;
            if (!mStarted && (interval <= ViewConfiguration.getTapTimeout())) {
                Log.d("rotate", "rotate ignored");
                return false;
            }

            // TODO complex decision between rotate or scale or both (see Google
            // Maps app). It seems if you start one or the other it takes more
            // to start the other too. Haven't figured out what it uses to
            // decide when to transition to both at the same time.

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Rotate the map
            double angle = mNativeMapView.getAngle();
            angle -= detector.getRotationDegreesDelta() * Math.PI / 180.0;
            Log.d("rotate", "rotate to " + angle);
            mNativeMapView.setAngle(angle, detector.getFocusX(),
                    detector.getFocusY());

            return true;
        }
    }

    // This class handles input events from the zoom control buttons
    // Zoom controls allow single touch only devices to zoom in and out
    private class OnZoomListener implements
            ZoomButtonsController.OnZoomListener {

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
    public boolean onKeyDown(int keyCode, KeyEvent event) {
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
            mNativeMapView.moveBy(scrollDist, 0.0);
            return true;

        case KeyEvent.KEYCODE_DPAD_RIGHT:
            if (!mScrollEnabled) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Move right
            mNativeMapView.moveBy(-scrollDist, 0.0);
            return true;

        case KeyEvent.KEYCODE_DPAD_UP:
            if (!mScrollEnabled) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Move up
            mNativeMapView.moveBy(0.0, scrollDist);
            return true;

        case KeyEvent.KEYCODE_DPAD_DOWN:
            if (!mScrollEnabled) {
                return false;
            }

            // Cancel any animation
            mNativeMapView.cancelTransitions();

            // Move down
            mNativeMapView.moveBy(0.0, -scrollDist);
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
    // TODO: test trackball click and long click
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
            mNativeMapView.moveBy(-10.0 * event.getX(), -10.0 * event.getY());
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

        // Cancel the timeoht
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
        // TODO: SOURCE_TOUCH_NAVIGATION?
        // TODO: source device resolution?
        if (event.isFromSource(InputDevice.SOURCE_CLASS_POINTER)) {
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
                mNativeMapView.scaleBy(Math.pow(2.0, scrollDist), event.getX(),
                        event.getY());

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
    public boolean onHoverEvent(MotionEvent event) {
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
    // Accessibility events
    //

    //
    // Map events
    //

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
        Log.v(TAG, "onMapChanged");
        if (mOnMapChangedListener != null) {
            mOnMapChangedListener.onMapChanged();
        }
    }
}
