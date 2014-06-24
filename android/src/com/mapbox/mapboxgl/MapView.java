package com.mapbox.mapboxgl;

import java.io.IOException;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.IOUtils;

import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.TypedArray;
import android.location.Location;
import android.os.Bundle;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.ZoomButtonsController;

// Custom view that shows a Map
// Based on SurfaceView as we use OpenGL ES to render
public class MapView extends SurfaceView {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapView";

    // Used for saving instance state
    private static final String STATE_CENTER_LOCATION = "centerLocation";
    private static final String STATE_ZOOM_LEVEL = "zoomLevel";
    private static final String STATE_ZOOM_ENABLED = "zoomEnabled";
    private static final String STATE_SCROLL_ENABLED = "scrollEnabled";
    private static final String STATE_ROTATE_ENABLED = "rotateEnabled";
    private static final String STATE_DEBUG_ACTIVE = "debugActive";

    //
    // Instance members
    //

    // Holds the pointer to JNI NativeMapView
    private long nativeMapViewPtr = 0;

    // Used to style the map
    private String mDefaultStyleJSON;

    // Touch gesture detectors
    private GestureDetector mGestureDetector;
    private ScaleGestureDetector mScaleGestureDetector;

    // Shows zoom buttons
    private ZoomButtonsController mZoomButtonsController;

    // Used to track trackball long presses
    private TrackballLongPressTimeOut mCurrentTrackballLongPressTimeOut;

    //
    // Properties
    //

    private boolean mZoomEnabled = true; // TODO implement the checks for these
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

        // Load the map style
        try {
            mDefaultStyleJSON = IOUtils.toString(context.getResources()
                    .openRawResource(R.raw.style), Charsets.UTF_8);
        } catch (IOException e) {
            throw new RuntimeException(
                    "Couldn't load default style JSON resource", e);
        }

        // Create the NativeMapView
        nativeMapViewPtr = nativeCreate(mDefaultStyleJSON);

        // Load attributes
        TypedArray typedArray = context.obtainStyledAttributes(attrs,
                R.styleable.MapView, 0, 0);

        try {
            double centerLongitude = typedArray.getFloat(
                    R.styleable.MapView_centerLongitude, 0.0f);
            double centerLatitude = typedArray.getFloat(
                    R.styleable.MapView_centerLatitude, 0.0f);
            float direction = typedArray.getFloat(
                    R.styleable.MapView_direction, 0.0f);
            Location centerLocation = new Location(TAG);
            centerLocation.setLongitude(centerLongitude);
            centerLocation.setLatitude(centerLatitude);
            centerLocation.setBearing(direction);
            setCenterLocation(centerLocation); // TODO set zoom to min zoom on
                                               // load
            setZoomLevel(typedArray.getFloat(R.styleable.MapView_zoomLevel,
                    0.0f)); // TODO these don't work?
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

        // Check if we are in Eclipse UI editor
        if (isInEditMode()) {
            // TODO
            // return;
        }

        // Touch gesture detectors
        mGestureDetector = new GestureDetector(context, new GestureListener());
        mGestureDetector.setIsLongpressEnabled(true);
        mScaleGestureDetector = new ScaleGestureDetector(context,
                new ScaleGestureListener());

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

    // TODO replace with custom lat/lon/dir class or use one from mapbox droid
    // sdk
    public Location getCenterLocation() {
        Location centerLocation = new Location(TAG);
        centerLocation.setLongitude(nativeGetLon(nativeMapViewPtr));
        centerLocation.setLatitude(nativeGetLat(nativeMapViewPtr));
        centerLocation.setBearing((float) nativeGetAngle(nativeMapViewPtr));
        return centerLocation;
    }

    public void setCenterLocation(Location centerLocation) {
        nativeSetLonLat(nativeMapViewPtr, centerLocation.getLongitude(),
                centerLocation.getLatitude());
    }

    public void resetPosition() {
        nativeResetPosition(nativeMapViewPtr);
    }

    public void resetNorth() {
        // TODO
    }

    public double getZoomLevel() {
        return nativeGetZoom(nativeMapViewPtr);
    }

    public void setZoomLevel(double zoomLevel) {
        nativeSetZoom(nativeMapViewPtr, zoomLevel);
    }

    public boolean isZoomEnabled() {
        return mZoomEnabled;
    }

    public void setZoomEnabled(boolean zoomEnabled) {
        this.mZoomEnabled = zoomEnabled;
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
        return nativeGetDebug(nativeMapViewPtr);
    }

    public void setDebugActive(boolean debugActive) {
        nativeSetDebug(nativeMapViewPtr, debugActive);
    }

    public void toggleDebug() {
        nativeToggleDebug(nativeMapViewPtr);
    }

    //
    // Lifecycle events
    //

    // Called when we need to restore instance state
    // Must be called from Activity onCreate
    public void onCreate(Bundle savedInstanceState) {
        if (savedInstanceState != null) {
            setCenterLocation((Location) savedInstanceState
                    .getParcelable(STATE_CENTER_LOCATION));
            setZoomEnabled(savedInstanceState.getBoolean(STATE_ZOOM_ENABLED));
            setScrollEnabled(savedInstanceState
                    .getBoolean(STATE_SCROLL_ENABLED));
            setRotateEnabled(savedInstanceState
                    .getBoolean(STATE_ROTATE_ENABLED));
            setDebugActive(savedInstanceState.getBoolean(STATE_DEBUG_ACTIVE));
        }
    }

    // Called when we need to save instance state
    // Must be called from Activity onSaveInstanceState
    public void onSaveInstanceState(Bundle outState) {
        outState.putParcelable(STATE_CENTER_LOCATION, getCenterLocation());
        outState.putDouble(STATE_ZOOM_LEVEL, getZoomLevel());
        outState.putBoolean(STATE_ZOOM_ENABLED, isZoomEnabled());
        outState.putBoolean(STATE_SCROLL_ENABLED, isScrollEnabled());
        outState.putBoolean(STATE_ROTATE_ENABLED, isRotateEnabled());
        outState.putBoolean(STATE_DEBUG_ACTIVE, isDebugActive());
    }

    // Called when we need to create the GL context
    // Must be called from Activity onStart
    public void onStart() {
        Log.v(TAG, "onStart");
        nativeInitializeContext(nativeMapViewPtr);
    }

    // Called when we need to terminate the GL context
    // Must be called from Activity onPause
    public void onStop() {
        Log.v(TAG, "onStop");
        nativeTerminateContext(nativeMapViewPtr);
    }

    // Called when we need to stop the render thread
    // Must be called from Activity onPause
    public void onPause() {
        Log.v(TAG, "onPause");
        nativeStop(nativeMapViewPtr);
    }

    // Called when we need to start the render thread
    // Must be called from Activity onResume
    public void onResume() {
        Log.v(TAG, "onResume");
        nativeStart(nativeMapViewPtr);
    }

    // This class handles SurfaceHolder callbacks
    private class Callbacks implements SurfaceHolder.Callback2 {

        // Called when we need to redraw the view
        // This is called before our view is first visible to prevent an initial
        // flicker (see Android SDK documentation)
        @Override
        public void surfaceRedrawNeeded(SurfaceHolder holder) {
            Log.v(TAG, "surfaceRedrawNeeded");
            nativeUpdate(nativeMapViewPtr);
        }

        // Called when the native surface buffer has been created
        // Must do all EGL/GL ES initialization here
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            Log.v(TAG, "surfaceCreated");
            nativeCreateSurface(nativeMapViewPtr, holder.getSurface());
            ;
        }

        // Called when the native surface buffer has been destroyed
        // Must do all EGL/GL ES destruction here
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            Log.v(TAG, "surfaceDestroyed");
            nativeDestroySurface(nativeMapViewPtr);
        }

        // Called when the format or size of the native surface buffer has been
        // changed
        // Must handle window resizing here.
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width,
                int height) {
            Log.v(TAG, "surfaceChanged");
            nativeResize(nativeMapViewPtr, width, height);
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
        if (mZoomButtonsController != null && visibility != View.VISIBLE) {
            mZoomButtonsController.setVisible(false);
        }
        if (mZoomButtonsController != null && visibility == View.VISIBLE) {
            mZoomButtonsController.setVisible(true);
        }
    }

    //
    // Draw events
    //

    // TODO: onDraw for editor?

    // TODO replace with callback from map
    // Called when the map's display has changed
    /*
     * private class OnChangeListener implements MapController.OnChangeListener
     * {
     * 
     * // Redraw map to show change
     * 
     * @Override public void onChange(MapController mapController) {
     * //postInvalidateOnAnimation();
     * 
     * // Tell accessibility the description text has changed
     * //sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_TEXT_CHANGED); } }
     */

    //
    // Input events
    //

    // Zoom in or out
    private void zoom(boolean zoomIn) {
        zoom(zoomIn, -1.0f, -1.0f);
    }

    private void zoom(boolean zoomIn, float x, float y) {
        // Cancel any animation
        nativeCancelTransitions(nativeMapViewPtr);

        if (zoomIn) {
            nativeScaleBy(nativeMapViewPtr, 2.0, x, y, 0.3);
        } else {
            // TODO check min zoom like ios
            // TODO two finger tap zoom out
            nativeScaleBy(nativeMapViewPtr, 0.5, x, y, 0.3);
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

        // Ensure we cascade the touch events correctly through the gesture
        // detectors
        // If an early detector consumes the event it is not passed to any more
        // Must use the || operator for this to work correctly
        boolean ret = mScaleGestureDetector.onTouchEvent(event);
        ret = mGestureDetector.onTouchEvent(event) || ret;
        return ret || super.onTouchEvent(event);
    }

    // This class handles one finger gestures
    private class GestureListener extends
            GestureDetector.SimpleOnGestureListener {

        // Must always return true otherwise all events are ignored
        @Override
        public boolean onDown(MotionEvent e) {
            // Show the zoom controls
            if (mZoomButtonsController != null) {
                mZoomButtonsController.setVisible(true);
            }

            return true;
        }

        // Called for double taps
        @Override
        public boolean onDoubleTap(MotionEvent e) {
            // Single finger double tap
            // Zoom in
            zoom(true, e.getX(), e.getY());
            return true;
        }

        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

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
            // TODO quick zoom, (but android one is double tap and swipe from
            // scale gesture detector?
            // https://github.com/inukshuk/gestures-lib
        }

        // Called for flings
        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
                float velocityY) {
            // Fling the map
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
             * // Cancel any animation
             * nativeCancelTransitions(nativeMapViewPtr);
             * 
             * nativeMoveBy(nativeMapViewPtr, velocityX * duration / 2.0,
             * velocityY * duration / 2.0, duration);
             * 
             * return true;
             */
            return false;
        }

        // Called for drags
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2,
                float distanceX, float distanceY) {
            // TODO start scrolling/cancel
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Scroll the map
            nativeMoveBy(nativeMapViewPtr, -distanceX, -distanceY);
            return true;
        }
    }

    // This class handles two finger gestures
    private class ScaleGestureListener extends
            ScaleGestureDetector.SimpleOnScaleGestureListener {

        // Called when two fingers first touch the screen
        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            // TODO start scaling/cancel
            return true;
        }

        // Called each time one of the two fingers moves
        // Called for pinch zooms
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            // Zoom the map
            // TODO hande min zoom?
            // TODO: rotation?
            // https://github.com/Almeros/android-gesture-detectors

            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Scale the map
            nativeScaleBy(nativeMapViewPtr, detector.getScaleFactor(),
                    detector.getFocusX(), detector.getFocusY());

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
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Move left
            nativeMoveBy(nativeMapViewPtr, scrollDist, 0.0);
            return true;

        case KeyEvent.KEYCODE_DPAD_RIGHT:
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Move right
            nativeMoveBy(nativeMapViewPtr, -scrollDist, 0.0);
            return true;

        case KeyEvent.KEYCODE_DPAD_UP:
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Move up
            nativeMoveBy(nativeMapViewPtr, 0.0, scrollDist);
            return true;

        case KeyEvent.KEYCODE_DPAD_DOWN:
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Move down
            nativeMoveBy(nativeMapViewPtr, 0.0, -scrollDist);
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
            // Cancel any animation
            nativeCancelTransitions(nativeMapViewPtr);

            // Scroll the map
            nativeMoveBy(nativeMapViewPtr, -10.0 * event.getX(),
                    -10.0 * event.getY());

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
            // Only handle if we have not already long pressed
            if (mCurrentTrackballLongPressTimeOut != null) {
                // Zoom in
                zoom(true);
            }
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
                // Cancel any animation
                nativeCancelTransitions(nativeMapViewPtr);

                // Get the vertical scroll amount, one click = 1
                float scrollDist = event.getAxisValue(MotionEvent.AXIS_VSCROLL);

                // Scale the map by the appropriate power of two factor
                nativeScaleBy(nativeMapViewPtr, Math.pow(2.0, scrollDist),
                        event.getX(), event.getY());

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
            if (mZoomButtonsController != null) {
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

    // Called when the map view transformation has changed
    // Called via JNI from NativeMapView
    // Need to update anything that relies on map state
    // TODO need to make this a listener so users of this view can register for
    // it
    protected void onMapChanged() {
        Log.v(TAG, "onMapChanged");
        // TODO
    }

    //
    // JNI methods
    //

    static {
        System.loadLibrary("NativeMapView");
    }

    @Override
    protected void finalize() throws Throwable {
        nativeDestroy(nativeMapViewPtr);
        nativeMapViewPtr = 0;
        super.finalize();
    }

    // TODO for each function implement a wrapper that handles the pointer
    // TODO implement all the other methods
    // TODO move to private wrapper class
    private native long nativeCreate(String defaultStyleJSON);

    private native void nativeDestroy(long nativeMapViewPtr);

    private native void nativeInitializeContext(long nativeMapViewPtr);

    private native void nativeTerminateContext(long nativeMapViewPtr);

    private native void nativeCreateSurface(long nativeMapViewPtr,
            Surface surface);

    private native void nativeDestroySurface(long nativeMapViewPtr);

    private native void nativeStart(long nativeMapViewPtr);

    private native void nativeStop(long nativeMapViewPtr);

    private native void nativeUpdate(long nativeMapViewPtr);

    private native void nativeCleanup(long nativeMapViewPtr);

    private native void nativeResize(long nativeMapViewPtr, int width,
            int height);

    private native void nativeCancelTransitions(long nativeMapViewPtr);

    // TODO use JNI on C++ side to pass back and forth lat/lng/zoom etc class?
    private native double nativeGetLon(long nativeMapViewPtr);

    private native double nativeGetLat(long nativeMapViewPtr);

    private native void nativeSetLonLat(long nativeMapViewPtr, double lon,
            double lat);

    private native void nativeResetPosition(long nativeMapViewPtr);

    private void nativeMoveBy(long nativeMapViewPtr, double dx, double dy) {
        nativeMoveBy(nativeMapViewPtr, dx, dy, 0.0);
    }

    private native void nativeMoveBy(long nativeMapViewPtr, double dx,
            double dy, double duration);

    private native double nativeGetZoom(long nativeMapViewPtr);

    private native void nativeSetZoom(long nativeMapViewPtr, double zoom);

    private void nativeScaleBy(long nativeMapViewPtr, double ds) {
        nativeScaleBy(nativeMapViewPtr, ds, -1.0, -1.0, 0.0);
    }

    private void nativeScaleBy(long nativeMapViewPtr, double ds, double cx,
            double cy) {
        nativeScaleBy(nativeMapViewPtr, ds, cx, cy, 0.0);
    }

    private native void nativeScaleBy(long nativeMapViewPtr, double ds,
            double cx, double cy, double duration);

    private native double nativeGetAngle(long nativeMapViewPtr);

    private native void nativeSetAngle(long nativeMapViewPtr, double angle);

    // private native void nativeResetNorth(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);
}
