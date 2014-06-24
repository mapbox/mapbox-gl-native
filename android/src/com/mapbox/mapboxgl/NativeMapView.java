package com.mapbox.mapboxgl;

import android.util.Log;
import android.view.Surface;

// Class that wraps the native methods for convenience
class NativeMapView {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "NativeMapView";

    //
    // Instance members
    //

    // Holds the pointer to JNI NativeMapView
    private long mNativeMapViewPtr = 0;

    // Used for callbacks
    private MapView mMapView;

    //
    // Static methods
    //

    static {
        System.loadLibrary("NativeMapView");
    }

    //
    // Constructors
    //

    public NativeMapView(MapView mapView, String defaultStyleJSON) {
        mMapView = mapView;

        // Create the NativeMapView
        mNativeMapViewPtr = nativeCreate(defaultStyleJSON);
    }

    //
    // Methods
    //

    public void initializeContext() {
        nativeInitializeContext(mNativeMapViewPtr);
    }

    public void terminateContext() {
        nativeTerminateContext(mNativeMapViewPtr);
    }

    public void createSurface(Surface surface) {
        nativeCreateSurface(mNativeMapViewPtr, surface);
    }

    public void destroySurface() {
        nativeDestroySurface(mNativeMapViewPtr);
    }

    public void start() {
        nativeStart(mNativeMapViewPtr);
    }

    public void stop() {
        nativeStop(mNativeMapViewPtr);
    }

    public void update() {
        nativeUpdate(mNativeMapViewPtr);
    }

    public void cleanup() {
        nativeCleanup(mNativeMapViewPtr);
    }

    public void resize(int width, int height) {
        nativeResize(mNativeMapViewPtr, width, height);
    }

    public void cancelTransitions() {
        nativeCancelTransitions(mNativeMapViewPtr);
    }

    public LonLat getLonLat() {
        return nativeGetLonLat(mNativeMapViewPtr);
    }

    public void setLonLat(LonLat lonLat) {
        nativeSetLonLat(mNativeMapViewPtr, lonLat);
    }

    public void resetPosition() {
        nativeResetPosition(mNativeMapViewPtr);
    }

    public void moveBy(double dx, double dy) {
        moveBy(dx, dy, 0.0);
    }

    public void moveBy(double dx, double dy, double duration) {
        nativeMoveBy(mNativeMapViewPtr, dx, dy, duration);
    }

    public double getZoom() {
        return nativeGetZoom(mNativeMapViewPtr);
    }

    public void setZoom(double zoom) {
        nativeSetZoom(mNativeMapViewPtr, zoom);
    }

    public void scaleBy(double ds) {
        scaleBy(ds, -1.0, -1.0, 0.0);
    }

    public void scaleBy(double ds, double cx, double cy) {
        scaleBy(ds, cx, cy, 0.0);
    }

    public void scaleBy(double ds, double cx, double cy, double duration) {
        nativeScaleBy(mNativeMapViewPtr, ds, cx, cy, duration);
    }

    public double getAngle() {
        return nativeGetAngle(mNativeMapViewPtr);
    }

    public void setAngle(double angle) {
        nativeSetAngle(mNativeMapViewPtr, angle);
    }

    public void resetNorth() {
        nativeResetNorth(mNativeMapViewPtr);
    }

    public boolean getDebug() {
        return nativeGetDebug(mNativeMapViewPtr);
    }

    public void setDebug(boolean debug) {
        nativeSetDebug(mNativeMapViewPtr, debug);
    }

    public void toggleDebug() {
        nativeToggleDebug(mNativeMapViewPtr);
    }

    //
    // Callbacks
    //

    protected void onMapChanged() {
        Log.v(TAG, "onMapChanged");
        mMapView.onMapChanged();
    }

    //
    // JNI methods
    //

    @Override
    protected void finalize() throws Throwable {
        nativeDestroy(mNativeMapViewPtr);
        mNativeMapViewPtr = 0;
        super.finalize();
    }

    // TODO implement all the other methods
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

    private native LonLat nativeGetLonLat(long nativeMapViewPtr);

    private native void nativeSetLonLat(long nativeMapViewPtr, LonLat lonLat);

    private native void nativeResetPosition(long nativeMapViewPtr);

    private native void nativeMoveBy(long nativeMapViewPtr, double dx,
            double dy, double duration);

    private native double nativeGetZoom(long nativeMapViewPtr);

    private native void nativeSetZoom(long nativeMapViewPtr, double zoom);

    private native void nativeScaleBy(long nativeMapViewPtr, double ds,
            double cx, double cy, double duration);

    private native double nativeGetAngle(long nativeMapViewPtr);

    private native void nativeSetAngle(long nativeMapViewPtr, double angle);

    private native void nativeResetNorth(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);
}
