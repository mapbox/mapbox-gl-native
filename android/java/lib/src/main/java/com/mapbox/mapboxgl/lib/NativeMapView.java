package com.mapbox.mapboxgl.lib;

import android.view.Surface;

import java.util.List;

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
        System.loadLibrary("mapbox-gl");
    }

    //
    // Constructors
    //

    public NativeMapView(MapView mapView, String cachePath, String dataPath, String apkPath) {
        mMapView = mapView;

        // Create the NativeMapView
        mNativeMapViewPtr = nativeCreate(cachePath, dataPath, apkPath);
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

    public void pause() {
        nativePause(mNativeMapViewPtr);
    }

    public void resume() {
        nativeResume(mNativeMapViewPtr);
    }

    public void run() {
        nativeRun(mNativeMapViewPtr);
    }

    public void rerender() {
        nativeRerender(mNativeMapViewPtr);
    }

    public void update() {
        nativeUpdate(mNativeMapViewPtr);
    }

    public void terminate() {
        nativeTerminate(mNativeMapViewPtr);
    }

    public boolean needsSwap() {
        return nativeNeedsSwap(mNativeMapViewPtr);
    }

    public void swapped() {
        nativeSwapped(mNativeMapViewPtr);
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

    public void setAppliedClasses(List<String> classes) {
        nativeSetAppliedClasses(mNativeMapViewPtr, classes);
    }

    public List<String> getAppliedClasses() {
        return nativeGetAppliedClasses(mNativeMapViewPtr);
    }

    public void setDefaultTransitionDuration() {
        setDefaultTransitionDuration(0);
    }

    public long getDefaultTransitionDuration() {
        return nativeGetDefaultTransitionDuration(mNativeMapViewPtr);
    }

    public void setDefaultTransitionDuration(long milliseconds) {
        if (milliseconds < 0) {
            throw new IllegalArgumentException(
                    "durationMilliseconds cannot be negative.");
        }

        nativeSetDefaultTransitionDuration(mNativeMapViewPtr,
                milliseconds);
    }

    public void setStyleUrl(String url) {
        nativeSetStyleUrl(mNativeMapViewPtr, url);
    }

    public void setStyleJson(String newStyleJson) {
        setStyleJson(newStyleJson,  "");
    }

    public void setStyleJson(String newStyleJson, String base) {
        nativeSetStyleJson(mNativeMapViewPtr, newStyleJson, base);
    }

    public String getStyleJson() {
        return nativeGetStyleJson(mNativeMapViewPtr);
    }

    public void setAccessToken(String accessToken) {
        nativeSetAccessToken(mNativeMapViewPtr, accessToken);
    }

    public String getAccessToken() {
        return nativeGetAccessToken(mNativeMapViewPtr);
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

    public void setBearing(double degrees) {
        setBearing(degrees, 0.0);
    }

    public void setBearing(double degrees, double duration) {
        nativeSetBearing(mNativeMapViewPtr, degrees, duration);
    }

    public void setBearing(double degrees, double cx, double cy) {
        nativeSetBearing(mNativeMapViewPtr, degrees, cx, cy);
    }

    public double getBearing() {
        return nativeGetBearing(mNativeMapViewPtr);
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

    public void setDebug(boolean debug) {
        nativeSetDebug(mNativeMapViewPtr, debug);
    }

    public void toggleDebug() {
        nativeToggleDebug(mNativeMapViewPtr);
    }

    public boolean getDebug() {
        return nativeGetDebug(mNativeMapViewPtr);
    }

    public void setReachability(boolean status) {
        nativeSetReachability(mNativeMapViewPtr, status);
    }

    //
    // Callbacks
    //

    protected void onMapChanged() {
        mMapView.onMapChanged();
    }

    protected void onFpsChanged(double fps) {
        mMapView.onFpsChanged(fps);
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

    private native long nativeCreate(String cachePath, String dataPath, String apkPath);

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

    private native void nativePause(long nativeMapViewPtr);

    private native void nativeResume(long nativeMapViewPtr);

    private native void nativeRun(long nativeMapViewPtr);

    private native void nativeRerender(long nativeMapViewPtr);

    private native void nativeUpdate(long nativeMapViewPtr);

    private native void nativeTerminate(long nativeMapViewPtr);

    private native boolean nativeNeedsSwap(long nativeMapViewPtr);

    private native void nativeSwapped(long nativeMapViewPtr);

    private native void nativeResize(long nativeMapViewPtr, int width,
            int height, float ratio);

    private native void nativeResize(long nativeMapViewPtr, int width,
            int height, float ratio, int fbWidth, int fbHeight);

    private native void nativeSetAppliedClasses(long nativeMapViewPtr,
            List<String> classes);

    private native List<String> nativeGetAppliedClasses(long nativeMapViewPtr);

    private native void nativeSetDefaultTransitionDuration(
            long nativeMapViewPtr, long milliseconds);

    private native long nativeGetDefaultTransitionDuration(long nativeMapViewPtr);

    private native void nativeSetStyleUrl(long nativeMapViewPtr, String url);

    private native void nativeSetStyleJson(long nativeMapViewPtr,
            String newStyleJson, String base);

    private native String nativeGetStyleJson(long nativeMapViewPtr);

    private native void nativeSetAccessToken(long nativeMapViewPtr, String accessToken);

    private native String nativeGetAccessToken(long nativeMapViewPtr);

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

    private native void nativeSetBearing(long nativeMapViewPtr, double degrees,
            double duration);

    private native void nativeSetBearing(long nativeMapViewPtr, double degrees,
            double cx, double cy);

    private native double nativeGetBearing(long nativeMapViewPtr);

    private native void nativeResetNorth(long nativeMapViewPtr);

    private native void nativeStartRotating(long nativeMapViewPtr);

    private native void nativeStopRotating(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);

    private native void nativeSetReachability(long nativeMapViewPtr, boolean status);
}
