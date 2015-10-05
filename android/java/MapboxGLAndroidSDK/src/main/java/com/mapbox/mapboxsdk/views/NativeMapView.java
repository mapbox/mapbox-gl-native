package com.mapbox.mapboxsdk.views;

import android.graphics.PointF;
import android.view.Surface;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import java.util.List;

// Class that wraps the native methods for convenience
final class NativeMapView {

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

    public NativeMapView(MapView mapView, String cachePath, String dataPath, String apkPath, float pixelRatio, int availableProcessors, long totalMemory) {
        if (availableProcessors < 0) {
            throw new IllegalArgumentException("availableProcessors cannot be negative.");
        }

        if (totalMemory < 0) {
            throw new IllegalArgumentException("totalMemory cannot be negative.");
        }

        mMapView = mapView;

        // Create the NativeMapView
        mNativeMapViewPtr = nativeCreate(cachePath, dataPath, apkPath, pixelRatio, availableProcessors, totalMemory);
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

    public void renderSync() {
        nativeRenderSync(mNativeMapViewPtr);
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
                    "milliseconds cannot be negative.");
        }

        nativeSetDefaultTransitionDuration(mNativeMapViewPtr,
                milliseconds);
    }

    public void setStyleUrl(String url) {
        nativeSetStyleUrl(mNativeMapViewPtr, url);
    }

    public void setStyleJson(String newStyleJson) {
        setStyleJson(newStyleJson, "");
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

    public void setGestureInProgress(boolean inProgress) {
        nativeSetGestureInProgress(mNativeMapViewPtr, inProgress);
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

    public long addMarker(Marker marker) {
        return nativeAddMarker(mNativeMapViewPtr, marker);
    }

    public long[] addMarkers(List<Marker> markers) {
        return nativeAddMarkers(mNativeMapViewPtr, markers);
    }

    public long addPolyline(Polyline polyline) {
        return nativeAddPolyline(mNativeMapViewPtr, polyline);
    }

    public long addPolygon(Polygon polygon) {
        return nativeAddPolygon(mNativeMapViewPtr, polygon);
    }

    public long[] addPolygons(List<Polygon> polygon) {
        return nativeAddPolygons(mNativeMapViewPtr, polygon);
    }

    public void removeAnnotation(long id) {
        nativeRemoveAnnotation(mNativeMapViewPtr, id);
    }

    public void removeAnnotations(long[] ids) {
        nativeRemoveAnnotations(mNativeMapViewPtr, ids);
    }

    public long[] getAnnotationsInBounds(BoundingBox bbox) {
        return nativeGetAnnotationsInBounds(mNativeMapViewPtr, bbox);
    }

    public void setSprite(String symbol, int width, int height, float scale, byte[] pixels) {
        nativeSetSprite(mNativeMapViewPtr, symbol, width, height, scale, pixels);
    }

    public void onLowMemory() {
        nativeOnLowMemory(mNativeMapViewPtr);
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

    public void setCollisionDebug(boolean debug) {
        nativeSetCollisionDebug(mNativeMapViewPtr, debug);
    }

    public void toggleCollisionDebug() {
        nativeToggleCollisionDebug(mNativeMapViewPtr);
    }

    public boolean getCollisionDebug() {
        return nativeGetCollisionDebug(mNativeMapViewPtr);
    }

    public boolean isFullyLoaded() {
        return nativeIsFullyLoaded(mNativeMapViewPtr);
    }

    public void setReachability(boolean status) {
        nativeSetReachability(mNativeMapViewPtr, status);
    }

    //public void                            ();

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

    public double getTopOffsetPixelsForAnnotationSymbol(String symbolName) {
        return nativeGetTopOffsetPixelsForAnnotationSymbol(mNativeMapViewPtr, symbolName);
    }

    //
    // Callbacks
    //

    protected void onInvalidate() {
        mMapView.onInvalidate();
    }

    protected void onMapChanged(int rawChange) {
        mMapView.onMapChanged(rawChange);
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

    private native long nativeCreate(String cachePath, String dataPath, String apkPath, float pixelRatio, int availableProcessors, long totalMemory);

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

    private native void nativeRenderSync(long nativeMapViewPtr);

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

    private native void nativeSetGestureInProgress(long nativeMapViewPtr, boolean inProgress);

    private native void nativeMoveBy(long nativeMapViewPtr, double dx,
            double dy, long duration);

    private native void nativeSetLatLng(long nativeMapViewPtr, LatLng latLng,
                                        long duration);

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

    private native long nativeAddMarker(long nativeMapViewPtr, Marker marker);

    private native long[] nativeAddMarkers(long nativeMapViewPtr, List<Marker> markers);

    private native long nativeAddPolyline(long nativeMapViewPtr, Polyline polyline);

    private native long nativeAddPolygon(long mNativeMapViewPtr, Polygon polygon);

    private native long[] nativeAddPolygons(long mNativeMapViewPtr, List<Polygon> polygon);

    private native void nativeRemoveAnnotation(long nativeMapViewPtr, long id);

    private native void nativeRemoveAnnotations(long nativeMapViewPtr, long[] id);

    private native long[] nativeGetAnnotationsInBounds(long mNativeMapViewPtr, BoundingBox bbox);

    private native void nativeSetSprite(long nativeMapViewPtr, String symbol,
                                        int width, int height, float scale, byte[] pixels);

    private native void nativeOnLowMemory(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);

    private native void nativeSetCollisionDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleCollisionDebug(long nativeMapViewPtr);

    private native boolean nativeGetCollisionDebug(long nativeMapViewPtr);

    private native boolean nativeIsFullyLoaded(long nativeMapViewPtr);

    private native void nativeSetReachability(long nativeMapViewPtr, boolean status);

    //private native void nativeGetWorldBoundsMeters(long nativeMapViewPtr);

    //private native void nativeGetWorldBoundsLatLng(long nativeMapViewPtr);

    private native double nativeGetMetersPerPixelAtLatitude(long nativeMapViewPtr, double lat, double zoom);

    private native ProjectedMeters nativeProjectedMetersForLatLng(long nativeMapViewPtr, LatLng latLng);

    private native LatLng nativeLatLngForProjectedMeters(long nativeMapViewPtr, ProjectedMeters projectedMeters);

    private native PointF nativePixelForLatLng(long nativeMapViewPtr, LatLng latLng);

    private native LatLng nativeLatLngForPixel(long nativeMapViewPtr, PointF pixel);

    private native double nativeGetTopOffsetPixelsForAnnotationSymbol(long nativeMapViewPtr, String symbolName);
}
