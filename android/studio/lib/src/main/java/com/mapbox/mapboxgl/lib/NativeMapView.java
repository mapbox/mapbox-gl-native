package com.mapbox.mapboxgl.lib;

import java.util.Set;

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

    public void initializeDisplay() {
        nativeInitializeDisplay(mNativeMapViewPtr);
    }

    public void terminateDisplay() {
        nativeTerminateDisplay(mNativeMapViewPtr);
    }

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

    public void rerender() {
        nativeRerender(mNativeMapViewPtr);
    }

    public void update() {
        nativeUpdate(mNativeMapViewPtr);
    }

    public void cleanup() {
        nativeCleanup(mNativeMapViewPtr);
    }

    public void addDefaultSource() {
        nativeAddDefaultSource(mNativeMapViewPtr);
    }

    public void removeDefaultSource() {
        nativeRemoveDefaultSource(mNativeMapViewPtr);
    }

    public void addSource(String name, String url) {
        nativeAddSource(mNativeMapViewPtr, name, url);
    }

    public void removeSource(String name) {
        nativeRemoveSource(mNativeMapViewPtr, name);
    }

    public void resize(int width, int height) {
        resize(width, height, 1.0f);
    }

    public void resize(int width, int height, float ratio) {
        if (width < 0) {
            throw new IllegalArgumentException("width cannot be negative.");
        }

        if (height < 0) {
            throw new IllegalArgumentException("height cannot be negative.");
        }

        if (width > 65535) {
            throw new IllegalArgumentException(
                    "width cannot be greater than 65535.");
        }

        if (height > 65535) {
            throw new IllegalArgumentException(
                    "height cannot be greater than 65535.");
        }

        nativeResize(mNativeMapViewPtr, width, height, ratio);
    }

    public void resize(int width, int height, float ratio, int fbWidth,
            int fbHeight) {
        if (width < 0) {
            throw new IllegalArgumentException("width cannot be negative.");
        }

        if (height < 0) {
            throw new IllegalArgumentException("height cannot be negative.");
        }

        if (fbWidth < 0) {
            throw new IllegalArgumentException("fbWidth cannot be negative.");
        }

        if (fbHeight < 0) {
            throw new IllegalArgumentException("fbHeight cannot be negative.");
        }

        if (fbWidth > 65535) {
            throw new IllegalArgumentException(
                    "fbWidth cannot be greater than 65535.");
        }

        if (fbHeight > 65535) {
            throw new IllegalArgumentException(
                    "fbHeight cannot be greater than 65535.");
        }
        nativeResize(mNativeMapViewPtr, width, height, ratio, fbWidth, fbHeight);
    }

    public void setAppliedClasses(Set<String> appliedClasses) {
        nativeSetAppliedClasses(mNativeMapViewPtr, appliedClasses);
    }

    public Set<String> getAppliedClasses() {
        return nativeGetAppliedClasses(mNativeMapViewPtr);
    }

    public void setDefaultTransitionDuration() {
        setDefaultTransitionDuration(0);
    }

    public void setDefaultTransitionDuration(long durationMilliseconds) {
        if (durationMilliseconds < 0) {
            throw new IllegalArgumentException(
                    "durationMilliseconds cannot be negative.");
        }

        nativeSetDefaultTransitionDuration(mNativeMapViewPtr,
                durationMilliseconds);
    }

    public void setStyleJSON(String newStyleJSON) {
        nativeSetStyleJSON(mNativeMapViewPtr, newStyleJSON);
    }

    public String getStyleJSON() {
        return nativeGetStyleJSON(mNativeMapViewPtr);
    }

    public void cancelTransitions() {
        nativeCancelTransitions(mNativeMapViewPtr);
    }

    public void moveBy(double dx, double dy) {
        moveBy(dx, dy, 0.0);
    }

    public void moveBy(double dx, double dy, double duration) {
        nativeMoveBy(mNativeMapViewPtr, dx, dy, duration);
    }

    public void setLonLat(LonLat lonLat) {
        setLonLat(lonLat, 0.0);
    }

    public void setLonLat(LonLat lonLat, double duration) {
        nativeSetLonLat(mNativeMapViewPtr, lonLat, duration);
    }

    public LonLat getLonLat() {
        return nativeGetLonLat(mNativeMapViewPtr);
    }

    public void startPanning() {
        nativeStartPanning(mNativeMapViewPtr);
    }

    public void stopPanning() {
        nativeStopPanning(mNativeMapViewPtr);
    }

    public void resetPosition() {
        nativeResetPosition(mNativeMapViewPtr);
    }

    public void scaleBy(double ds) {
        scaleBy(ds, -1.0, -1.0);
    }

    public void scaleBy(double ds, double cx, double cy) {
        scaleBy(ds, cx, cy, 0.0);
    }

    public void scaleBy(double ds, double cx, double cy, double duration) {
        nativeScaleBy(mNativeMapViewPtr, ds, cx, cy, duration);
    }

    public void setScale(double scale) {
        setScale(scale, -1.0, -1.0);
    }

    public void setScale(double scale, double cx, double cy) {
        setScale(scale, cx, cy, 0.0);
    }

    public void setScale(double scale, double cx, double cy, double duration) {
        nativeSetScale(mNativeMapViewPtr, scale, cx, cy, duration);
    }

    public double getScale() {
        return nativeGetScale(mNativeMapViewPtr);
    }

    public void setZoom(double zoom) {
        setZoom(zoom, 0.0);
    }

    public void setZoom(double zoom, double duration) {
        nativeSetZoom(mNativeMapViewPtr, zoom, duration);
    }

    public double getZoom() {
        return nativeGetZoom(mNativeMapViewPtr);
    }

    public void setLonLatZoom(LonLatZoom lonLatZoom) {
        setLonLatZoom(lonLatZoom, 0.0);
    }

    public void setLonLatZoom(LonLatZoom lonLatZoom, double duration) {
        nativeSetLonLatZoom(mNativeMapViewPtr, lonLatZoom, duration);
    }

    public LonLatZoom getLonLatZoom() {
        return nativeGetLonLatZoom(mNativeMapViewPtr);
    }

    public void resetZoom() {
        nativeResetZoom(mNativeMapViewPtr);
    }

    public void startScaling() {
        nativeStartScaling(mNativeMapViewPtr);
    }

    public void stopScaling() {
        nativeStopScaling(mNativeMapViewPtr);
    }

    public double getMinZoom() {
        return nativeGetMinZoom(mNativeMapViewPtr);
    }

    public double getMaxZoom() {
        return nativeGetMaxZoom(mNativeMapViewPtr);
    }

    public void rotateBy(double sx, double sy, double ex, double ey) {
        rotateBy(sx, sy, ex, ey, 0.0);
    }

    public void rotateBy(double sx, double sy, double ex, double ey,
            double duration) {
        nativeRotateBy(mNativeMapViewPtr, sx, sy, ex, ey, duration);
    }

    public void setAngle(double angle) {
        setAngle(angle, 0.0);
    }

    public void setAngle(double angle, double duration) {
        nativeSetAngle(mNativeMapViewPtr, angle, duration);
    }

    public void setAngle(double angle, double cx, double cy) {
        nativeSetAngle(mNativeMapViewPtr, angle, cx, cy);
    }

    public double getAngle() {
        return nativeGetAngle(mNativeMapViewPtr);
    }

    public void resetNorth() {
        nativeResetNorth(mNativeMapViewPtr);
    }

    public void startRotating() {
        nativeStartRotating(mNativeMapViewPtr);
    }

    public void stopRotating() {
        nativeStopRotating(mNativeMapViewPtr);
    }

    public boolean canRotate() {
        return nativeCanRotate(mNativeMapViewPtr);
    }

    public void setDebug(boolean debug) {
        nativeSetDebug(mNativeMapViewPtr, debug);
    }

    public void toggleDebug() {
        nativeToggleDebug(mNativeMapViewPtr);
    }

    public boolean getDebug() {
        return nativeGetDebug(mNativeMapViewPtr);
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

    private native long nativeCreate(String defaultStyleJSON);

    private native void nativeDestroy(long nativeMapViewPtr);

    private native void nativeInitializeDisplay(long nativeMapViewPtr);

    private native void nativeTerminateDisplay(long nativeMapViewPtr);

    private native void nativeInitializeContext(long nativeMapViewPtr);

    private native void nativeTerminateContext(long nativeMapViewPtr);

    private native void nativeCreateSurface(long nativeMapViewPtr,
            Surface surface);

    private native void nativeDestroySurface(long nativeMapViewPtr);

    private native void nativeStart(long nativeMapViewPtr);

    private native void nativeStop(long nativeMapViewPtr);

    private native void nativeRerender(long nativeMapViewPtr);

    private native void nativeUpdate(long nativeMapViewPtr);

    private native void nativeCleanup(long nativeMapViewPtr);

    private native void nativeAddDefaultSource(long nativeMapViewPtr);

    private native void nativeRemoveDefaultSource(long nativeMapViewPtr);

    private native void nativeAddSource(long nativeMapViewPtr, String name,
            String url);

    private native void nativeRemoveSource(long nativeMapViewPtr, String name);

    private native void nativeResize(long nativeMapViewPtr, int width,
            int height, float ratio);

    private native void nativeResize(long nativeMapViewPtr, int width,
            int height, float ratio, int fbWidth, int fbHeight);

    private native void nativeSetAppliedClasses(long nativeMapViewPtr,
            Set<String> appliedClasses);

    private native Set<String> nativeGetAppliedClasses(long nativeMapViewPtr);

    private native void nativeSetDefaultTransitionDuration(
            long nativeMapViewPtr, long durationMilliseconds);

    private native void nativeSetStyleJSON(long nativeMapViewPtr,
            String newStyleJSON);

    private native String nativeGetStyleJSON(long nativeMapViewPtr);

    private native void nativeCancelTransitions(long nativeMapViewPtr);

    private native void nativeMoveBy(long nativeMapViewPtr, double dx,
            double dy, double duration);

    private native void nativeSetLonLat(long nativeMapViewPtr, LonLat lonLat,
            double duration);

    private native LonLat nativeGetLonLat(long nativeMapViewPtr);

    private native void nativeStartPanning(long nativeMapViewPtr);

    private native void nativeStopPanning(long nativeMapViewPtr);

    private native void nativeResetPosition(long nativeMapViewPtr);

    private native void nativeScaleBy(long nativeMapViewPtr, double ds,
            double cx, double cy, double duration);

    private native void nativeSetScale(long nativeMapViewPtr, double scale,
            double cx, double cy, double duration);

    private native double nativeGetScale(long nativeMapViewPtr);

    private native void nativeSetZoom(long nativeMapViewPtr, double zoom,
            double duration);

    private native double nativeGetZoom(long nativeMapViewPtr);

    private native void nativeSetLonLatZoom(long nativeMapViewPtr,
            LonLatZoom lonLatZoom, double duration);

    private native LonLatZoom nativeGetLonLatZoom(long nativeMapViewPtr);

    private native void nativeResetZoom(long nativeMapViewPtr);

    private native void nativeStartScaling(long nativeMapViewPtr);

    private native void nativeStopScaling(long nativeMapViewPtr);

    private native double nativeGetMinZoom(long nativeMapViewPtr);

    private native double nativeGetMaxZoom(long nativeMapViewPtr);

    private native void nativeRotateBy(long nativeMapViewPtr, double sx,
            double sy, double ex, double ey, double duration);

    private native void nativeSetAngle(long nativeMapViewPtr, double angle,
            double duration);

    private native void nativeSetAngle(long nativeMapViewPtr, double angle,
            double cx, double cy);

    private native double nativeGetAngle(long nativeMapViewPtr);

    private native void nativeResetNorth(long nativeMapViewPtr);

    private native void nativeStartRotating(long nativeMapViewPtr);

    private native void nativeStopRotating(long nativeMapViewPtr);

    private native boolean nativeCanRotate(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);
}
