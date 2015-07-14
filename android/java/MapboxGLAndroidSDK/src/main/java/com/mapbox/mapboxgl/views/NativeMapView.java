package com.mapbox.mapboxgl.views;

import android.graphics.PointF;
import android.view.Surface;

import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.geometry.LatLngZoom;
import com.mapbox.mapboxgl.geometry.ProjectedMeters;

import java.util.List;

// Class that wraps the native methods for convenience
class NativeMapView {

    //
    // Static members
    //

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

    public NativeMapView(MapView mapView, String cachePath, String dataPath, String apkPath, float pixelRatio) {
        mMapView = mapView;

        // Create the NativeMapView
        mNativeMapViewPtr = nativeCreate(cachePath, dataPath, apkPath, pixelRatio);
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

    public void pause() {
        nativePause(mNativeMapViewPtr);
    }

    public void resume() {
        nativeResume(mNativeMapViewPtr);
    }

    public void update() {
        nativeUpdate(mNativeMapViewPtr);
    }

    public void invalidate() {
        nativeOnInvalidate(mNativeMapViewPtr);
    }

    public void resizeView(int width, int height) {
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
        nativeViewResize(mNativeMapViewPtr, width, height);
    }

    public void resizeFramebuffer(int fbWidth, int fbHeight) {
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
        nativeFramebufferResize(mNativeMapViewPtr, fbWidth, fbHeight);
    }

    public void addClass(String clazz) {
        nativeAddClass(mNativeMapViewPtr, clazz);
    }

    public void removeClass(String clazz) {
        nativeRemoveClass(mNativeMapViewPtr, clazz);
    }

    public boolean hasClass(String clazz) {
        return nativeHasClass(mNativeMapViewPtr, clazz);
    }

    public void setClasses(List<String> classes) {
        nativeSetClasses(mNativeMapViewPtr, classes);
    }

    public List<String> getClasses() {
        return nativeGetClasses(mNativeMapViewPtr);
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
        moveBy(dx, dy, 0);
    }

    public void moveBy(double dx, double dy, long duration) {
        nativeMoveBy(mNativeMapViewPtr, dx, dy, duration);
    }

    public void setLatLng(LatLng latLng) {
        setLatLng(latLng, 0);
    }

    public void setLatLng(LatLng latLng, long duration) {
        nativeSetLatLng(mNativeMapViewPtr, latLng, duration);
    }

    public void addMarker(LatLng latLng) {
        nativeAddMarker(mNativeMapViewPtr, latLng);
    }

    public LatLng getLatLng() {
        return nativeGetLatLng(mNativeMapViewPtr);
    }

    public void resetPosition() {
        nativeResetPosition(mNativeMapViewPtr);
    }

    public void scaleBy(double ds) {
        scaleBy(ds, -1.0, -1.0);
    }

    public void scaleBy(double ds, double cx, double cy) {
        scaleBy(ds, cx, cy, 0);
    }

    public void scaleBy(double ds, double cx, double cy, long duration) {
        nativeScaleBy(mNativeMapViewPtr, ds, cx, cy, duration);
    }

    public void setScale(double scale) {
        setScale(scale, -1.0, -1.0);
    }

    public void setScale(double scale, double cx, double cy) {
        setScale(scale, cx, cy, 0);
    }

    public void setScale(double scale, double cx, double cy, long duration) {
        nativeSetScale(mNativeMapViewPtr, scale, cx, cy, duration);
    }

    public double getScale() {
        return nativeGetScale(mNativeMapViewPtr);
    }

    public void setZoom(double zoom) {
        setZoom(zoom, 0);
    }

    public void setZoom(double zoom, long duration) {
        nativeSetZoom(mNativeMapViewPtr, zoom, duration);
    }

    public double getZoom() {
        return nativeGetZoom(mNativeMapViewPtr);
    }

    public void setLatLngZoom(LatLngZoom latLngZoom) {
        setLatLngZoom(latLngZoom, 0);
    }

    public void setLatLngZoom(LatLngZoom latLngZoom, long duration) {
        nativeSetLatLngZoom(mNativeMapViewPtr, latLngZoom, duration);
    }

    public LatLngZoom getLatLngZoom() {
        return nativeGetLatLngZoom(mNativeMapViewPtr);
    }

    public void resetZoom() {
        nativeResetZoom(mNativeMapViewPtr);
    }

    public double getMinZoom() {
        return nativeGetMinZoom(mNativeMapViewPtr);
    }

    public double getMaxZoom() {
        return nativeGetMaxZoom(mNativeMapViewPtr);
    }

    public void rotateBy(double sx, double sy, double ex, double ey) {
        rotateBy(sx, sy, ex, ey, 0);
    }

    public void rotateBy(double sx, double sy, double ex, double ey,
                         long duration) {
        nativeRotateBy(mNativeMapViewPtr, sx, sy, ex, ey, duration);
    }

    public void setBearing(double degrees) {
        setBearing(degrees, 0);
    }

    public void setBearing(double degrees, long duration) {
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

    //public void getWorldBoundsMeters();

    //public void getWorldBoundsLatLng();

    public double getMetersPerPixelAtLatitude(double lat, double zoom) {
        return nativeGetMetersPerPixelAtLatitude(mNativeMapViewPtr, lat, zoom);
    }

    public ProjectedMeters projectedMetersForLatLng(LatLng latLng) {
        return nativeProjectedMetersForLatLng(mNativeMapViewPtr, latLng);
    }

    public LatLng latLngForProjectedMeters(ProjectedMeters projectedMeters) {
        return nativeLatLngForProjectedMeters(mNativeMapViewPtr, projectedMeters);
    }

    public PointF pixelForLatLng(LatLng latLng) {
        return nativePixelForLatLng(mNativeMapViewPtr, latLng);
    }

    public LatLng latLngForPixel(PointF pixel) {
        return nativeLatLngForPixel(mNativeMapViewPtr, pixel);
    }

    //
    // Callbacks
    //

    protected void onInvalidate() {
        mMapView.onInvalidate();
    }

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

    private native long nativeCreate(String cachePath, String dataPath, String apkPath, float pixelRatio);

    private native void nativeDestroy(long nativeMapViewPtr);

    private native void nativeInitializeDisplay(long nativeMapViewPtr);

    private native void nativeTerminateDisplay(long nativeMapViewPtr);

    private native void nativeInitializeContext(long nativeMapViewPtr);

    private native void nativeTerminateContext(long nativeMapViewPtr);

    private native void nativeCreateSurface(long nativeMapViewPtr,
            Surface surface);

    private native void nativeDestroySurface(long nativeMapViewPtr);

    private native void nativePause(long nativeMapViewPtr);

    private native void nativeResume(long nativeMapViewPtr);

    private native void nativeUpdate(long nativeMapViewPtr);

    private native void nativeOnInvalidate(long nativeMapViewPtr);

    private native void nativeViewResize(long nativeMapViewPtr, int width, int height);

    private native void nativeFramebufferResize(long nativeMapViewPtr, int fbWidth, int fbHeight);

    private native void nativeAddClass(long nativeMapViewPtr, String clazz);

    private native void nativeRemoveClass(long nativeMapViewPtr, String clazz);

    private native boolean nativeHasClass(long nativeMapViewPtr, String clazz);

    private native void nativeSetClasses(long nativeMapViewPtr,
            List<String> classes);

    private native List<String> nativeGetClasses(long nativeMapViewPtr);

    private native void nativeSetDefaultTransitionDuration(
            long nativeMapViewPtr, long duration);

    private native long nativeGetDefaultTransitionDuration(long nativeMapViewPtr);

    private native void nativeSetStyleUrl(long nativeMapViewPtr, String url);

    private native void nativeSetStyleJson(long nativeMapViewPtr,
            String newStyleJson, String base);

    private native String nativeGetStyleJson(long nativeMapViewPtr);

    private native void nativeSetAccessToken(long nativeMapViewPtr, String accessToken);

    private native String nativeGetAccessToken(long nativeMapViewPtr);

    private native void nativeCancelTransitions(long nativeMapViewPtr);

    private native void nativeMoveBy(long nativeMapViewPtr, double dx,
            double dy, long duration);

    private native void nativeSetLatLng(long nativeMapViewPtr, LatLng latLng,
                                        long duration);

    private native void nativeAddMarker(long nativeMapViewPtr, LatLng latLng);

    private native LatLng nativeGetLatLng(long nativeMapViewPtr);

    private native void nativeResetPosition(long nativeMapViewPtr);

    private native void nativeScaleBy(long nativeMapViewPtr, double ds,
            double cx, double cy, long duration);

    private native void nativeSetScale(long nativeMapViewPtr, double scale,
            double cx, double cy, long duration);

    private native double nativeGetScale(long nativeMapViewPtr);

    private native void nativeSetZoom(long nativeMapViewPtr, double zoom,
                                      long duration);

    private native double nativeGetZoom(long nativeMapViewPtr);

    private native void nativeSetLatLngZoom(long nativeMapViewPtr,
            LatLngZoom lonLatZoom, long duration);

    private native LatLngZoom nativeGetLatLngZoom(long nativeMapViewPtr);

    private native void nativeResetZoom(long nativeMapViewPtr);

    private native double nativeGetMinZoom(long nativeMapViewPtr);

    private native double nativeGetMaxZoom(long nativeMapViewPtr);

    private native void nativeRotateBy(long nativeMapViewPtr, double sx,
            double sy, double ex, double ey, long duration);

    private native void nativeSetBearing(long nativeMapViewPtr, double degrees,
            long duration);

    private native void nativeSetBearing(long nativeMapViewPtr, double degrees,
            double cx, double cy);

    private native double nativeGetBearing(long nativeMapViewPtr);

    private native void nativeResetNorth(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);

    private native void nativeSetReachability(long nativeMapViewPtr, boolean status);

    //private native void nativeGetWorldBoundsMeters(long nativeMapViewPtr);

    //private native void nativeGetWorldBoundsLatLng(long nativeMapViewPtr);

    private native double nativeGetMetersPerPixelAtLatitude(long nativeMapViewPtr, double lat, double zoom);

    private native ProjectedMeters nativeProjectedMetersForLatLng(long nativeMapViewPtr, LatLng latLng);

    private native LatLng nativeLatLngForProjectedMeters(long nativeMapViewPtr, ProjectedMeters projectedMeters);

    private native PointF nativePixelForLatLng(long nativeMapViewPtr, LatLng latLng);

    private native LatLng nativeLatLngForPixel(long nativeMapViewPtr, PointF pixel);
}
