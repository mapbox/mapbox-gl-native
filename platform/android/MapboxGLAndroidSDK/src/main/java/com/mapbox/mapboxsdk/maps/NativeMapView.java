package com.mapbox.mapboxsdk.maps;

import android.app.ActivityManager;
import android.content.Context;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.Surface;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import com.mapbox.mapboxsdk.offline.OfflineManager;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.style.sources.NoSuchSourceException;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.services.commons.geojson.Feature;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

// Class that wraps the native methods for convenience
final class NativeMapView {

    //
    // Static members
    //

    //
    // Instance members
    //

    boolean mDestroyed = false;

    // Holds the pointer to JNI NativeMapView
    private long mNativeMapViewPtr = 0;

    // Used for callbacks
    private MapView mMapView;

    private final float pixelRatio;

    //
    // Static methods
    //

    static {
        System.loadLibrary("mapbox-gl");
    }

    //
    // Constructors
    //

    public NativeMapView(MapView mapView) {
        Context context = mapView.getContext();
        String dataPath = OfflineManager.getDatabasePath(context);

        // With the availability of offline, we're unifying the ambient (cache) and the offline
        // databases to be in the same folder, outside cache, to avoid automatic deletion from
        // the system
        String cachePath = dataPath;

        pixelRatio = context.getResources().getDisplayMetrics().density;
        String apkPath = context.getPackageCodePath();
        int availableProcessors = Runtime.getRuntime().availableProcessors();
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        ActivityManager activityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        activityManager.getMemoryInfo(memoryInfo);
        long totalMemory = memoryInfo.availMem;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            totalMemory = memoryInfo.totalMem;
        }

        if (availableProcessors < 0) {
            throw new IllegalArgumentException("availableProcessors cannot be negative.");
        }

        if (totalMemory < 0) {
            throw new IllegalArgumentException("totalMemory cannot be negative.");
        }

        mMapView = mapView;
        mNativeMapViewPtr = nativeCreate(cachePath, dataPath, apkPath, pixelRatio, availableProcessors, totalMemory);
    }

    //
    // Methods
    //

    public void destroy() {
        nativeDestroy(mNativeMapViewPtr);
        mNativeMapViewPtr = 0;
        mMapView = null;
        mDestroyed = true;
    }

    public boolean wasDestroyed() {
        return mDestroyed;
    }

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

    public void update() {
        nativeUpdate(mNativeMapViewPtr);
    }

    public void render() {
        nativeRender(mNativeMapViewPtr);
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

    public void setStyleUrl(String url) {
        nativeSetStyleUrl(mNativeMapViewPtr, url);
    }

    public void setStyleJson(String newStyleJson) {
        nativeSetStyleJson(mNativeMapViewPtr, newStyleJson);
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
        nativeSetLatLng(mNativeMapViewPtr, latLng.getLatitude(), latLng.getLongitude(), duration);
    }

    public LatLng getLatLng() {
        return nativeGetLatLng(mNativeMapViewPtr);
    }

    public void resetPosition() {
        nativeResetPosition(mNativeMapViewPtr);
    }

    public double getPitch() {
        return nativeGetPitch(mNativeMapViewPtr);
    }

    public void setPitch(double pitch, long duration) {
        nativeSetPitch(mNativeMapViewPtr, pitch, duration);
    }

    public void scaleBy(double ds) {
        scaleBy(ds, Double.NaN, Double.NaN);
    }

    public void scaleBy(double ds, double cx, double cy) {
        scaleBy(ds, cx, cy, 0);
    }

    public void scaleBy(double ds, double cx, double cy, long duration) {
        nativeScaleBy(mNativeMapViewPtr, ds, cx, cy, duration);
    }

    public void setScale(double scale) {
        setScale(scale, Double.NaN, Double.NaN);
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

    public void resetZoom() {
        nativeResetZoom(mNativeMapViewPtr);
    }

    public void setMinZoom(double zoom) {
        nativeSetMinZoom(mNativeMapViewPtr, zoom);
    }

    public double getMinZoom() {
        return nativeGetMinZoom(mNativeMapViewPtr);
    }

    public void setMaxZoom(double zoom) {
        nativeSetMaxZoom(mNativeMapViewPtr, zoom);
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

    public void setContentPadding(double top, double left, double bottom, double right) {
        nativeSetContentPadding(mNativeMapViewPtr, top, left, bottom, right);
    }

    public void setBearing(double degrees) {
        setBearing(degrees, 0);
    }

    public void setBearing(double degrees, long duration) {
        nativeSetBearing(mNativeMapViewPtr, degrees, duration);
    }

    public void setBearing(double degrees, double cx, double cy) {
        nativeSetBearingXY(mNativeMapViewPtr, degrees, cx, cy);
    }

    public double getBearing() {
        return nativeGetBearing(mNativeMapViewPtr);
    }

    public void resetNorth() {
        nativeResetNorth(mNativeMapViewPtr);
    }

    public long addMarker(Marker marker) {
        Marker[] markers = {marker};
        return nativeAddMarkers(mNativeMapViewPtr, markers)[0];
    }

    public long[] addMarkers(List<Marker> markers) {
        return nativeAddMarkers(mNativeMapViewPtr, markers.toArray(new Marker[markers.size()]));
    }

    public long addPolyline(Polyline polyline) {
        Polyline[] polylines = {polyline};
        return nativeAddPolylines(mNativeMapViewPtr, polylines)[0];
    }

    public long[] addPolylines(List<Polyline> polylines) {
        return nativeAddPolylines(mNativeMapViewPtr, polylines.toArray(new Polyline[polylines.size()]));
    }

    public long addPolygon(Polygon polygon) {
        Polygon[] polygons = {polygon};
        return nativeAddPolygons(mNativeMapViewPtr, polygons)[0];
    }

    public long[] addPolygons(List<Polygon> polygons) {
        return nativeAddPolygons(mNativeMapViewPtr, polygons.toArray(new Polygon[polygons.size()]));
    }

    public void updateMarker(Marker marker) {
        LatLng position = marker.getPosition();
        Icon icon = marker.getIcon();
        nativeUpdateMarker(mNativeMapViewPtr, marker.getId(), position.getLatitude(), position.getLongitude(), icon.getId());
    }

    public void updatePolygon(Polygon polygon) {
        nativeUpdatePolygon(mNativeMapViewPtr, polygon.getId(), polygon);
    }

    public void updatePolyline(Polyline polyline) {
        nativeUpdatePolyline(mNativeMapViewPtr, polyline.getId(), polyline);
    }

    public void removeAnnotation(long id) {
        long[] ids = {id};
        removeAnnotations(ids);
    }

    public void removeAnnotations(long[] ids) {
        nativeRemoveAnnotations(mNativeMapViewPtr, ids);
    }

    public long[] queryPointAnnotations(RectF rect) {
        return nativeQueryPointAnnotations(mNativeMapViewPtr, rect);
    }

    public void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels) {
        nativeAddAnnotationIcon(mNativeMapViewPtr, symbol, width, height, scale, pixels);
    }

    public void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
        nativeSetVisibleCoordinateBounds(mNativeMapViewPtr, coordinates, padding, direction, duration);
    }

    public void onLowMemory() {
        nativeOnLowMemory(mNativeMapViewPtr);
    }

    public void setDebug(boolean debug) {
        nativeSetDebug(mNativeMapViewPtr, debug);
    }

    public void cycleDebugOptions() {
        nativeToggleDebug(mNativeMapViewPtr);
    }

    public boolean getDebug() {
        return nativeGetDebug(mNativeMapViewPtr);
    }

    public boolean isFullyLoaded() {
        return nativeIsFullyLoaded(mNativeMapViewPtr);
    }

    public void setReachability(boolean status) {
        nativeSetReachability(mNativeMapViewPtr, status);
    }

    public double getMetersPerPixelAtLatitude(double lat, double zoom) {
        return nativeGetMetersPerPixelAtLatitude(mNativeMapViewPtr, lat, zoom);
    }

    public ProjectedMeters projectedMetersForLatLng(LatLng latLng) {
        return nativeProjectedMetersForLatLng(mNativeMapViewPtr, latLng.getLatitude(), latLng.getLongitude());
    }

    public LatLng latLngForProjectedMeters(ProjectedMeters projectedMeters) {
        return nativeLatLngForProjectedMeters(mNativeMapViewPtr, projectedMeters.getNorthing(), projectedMeters.getEasting());
    }

    public PointF pixelForLatLng(LatLng latLng) {
        return nativePixelForLatLng(mNativeMapViewPtr, latLng.getLatitude(), latLng.getLongitude());
    }

    public LatLng latLngForPixel(PointF pixel) {
        return nativeLatLngForPixel(mNativeMapViewPtr, pixel.x, pixel.y);
    }

    public double getTopOffsetPixelsForAnnotationSymbol(String symbolName) {
        return nativeGetTopOffsetPixelsForAnnotationSymbol(mNativeMapViewPtr, symbolName);
    }

    public void jumpTo(double angle, LatLng center, double pitch, double zoom) {
        nativeJumpTo(mNativeMapViewPtr, angle, center.getLatitude(), center.getLongitude(), pitch, zoom);
    }

    public void easeTo(double angle, LatLng center, long duration, double pitch, double zoom, boolean easingInterpolator) {
        nativeEaseTo(mNativeMapViewPtr, angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom, easingInterpolator);
    }

    public void flyTo(double angle, LatLng center, long duration, double pitch, double zoom) {
        nativeFlyTo(mNativeMapViewPtr, angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom);
    }

    public double[] getCameraValues() {
        return nativeGetCameraValues(mNativeMapViewPtr);
    }

    // Runtime style Api

    public Layer getLayer(String layerId) {
        return nativeGetLayer(mNativeMapViewPtr, layerId);
    }

    public void addLayer(@NonNull Layer layer, @Nullable String before) {
        nativeAddLayer(mNativeMapViewPtr, layer.getNativePtr(), before);
        layer.invalidate();
    }

    public void removeLayer(@NonNull String layerId) throws NoSuchLayerException {
        nativeRemoveLayer(mNativeMapViewPtr, layerId);
    }

    public void addSource(@NonNull Source source) {
        nativeAddSource(mNativeMapViewPtr, source.getId(), source);
    }

    public void removeSource(@NonNull String sourceId) throws NoSuchSourceException {
        nativeRemoveSource(mNativeMapViewPtr, sourceId);
    }

    // Feature querying

    @NonNull
    public List<Feature> queryRenderedFeatures(PointF coordinates, String... layerIds) {
        Feature[] features = nativeQueryRenderedFeaturesForPoint(mNativeMapViewPtr, coordinates.x / pixelRatio, coordinates.y / pixelRatio, layerIds);
        return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
    }

    @NonNull
    public List<Feature> queryRenderedFeatures(RectF coordinates, String... layerIds) {
        Feature[] features = nativeQueryRenderedFeaturesForBox(
                mNativeMapViewPtr,
                coordinates.left / pixelRatio,
                coordinates.top / pixelRatio,
                coordinates.right / pixelRatio,
                coordinates.bottom / pixelRatio,
                layerIds);
        return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
    }

    public void scheduleTakeSnapshot() {
        nativeScheduleTakeSnapshot(mNativeMapViewPtr);
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

    protected void onSnapshotReady(byte[] bytes) {
        mMapView.onSnapshotReady(bytes);
    }

    //
    // JNI methods
    //

    private native long nativeCreate(String cachePath, String dataPath, String apkPath, float pixelRatio, int availableProcessors, long totalMemory);

    private native void nativeDestroy(long nativeMapViewPtr);

    private native void nativeInitializeDisplay(long nativeMapViewPtr);

    private native void nativeTerminateDisplay(long nativeMapViewPtr);

    private native void nativeInitializeContext(long nativeMapViewPtr);

    private native void nativeTerminateContext(long nativeMapViewPtr);

    private native void nativeCreateSurface(long nativeMapViewPtr,
                                            Surface surface);

    private native void nativeDestroySurface(long nativeMapViewPtr);

    private native void nativeUpdate(long nativeMapViewPtr);

    private native void nativeRender(long nativeMapViewPtr);

    private native void nativeViewResize(long nativeMapViewPtr, int width, int height);

    private native void nativeFramebufferResize(long nativeMapViewPtr, int fbWidth, int fbHeight);

    private native void nativeAddClass(long nativeMapViewPtr, String clazz);

    private native void nativeRemoveClass(long nativeMapViewPtr, String clazz);

    private native boolean nativeHasClass(long nativeMapViewPtr, String clazz);

    private native void nativeSetClasses(long nativeMapViewPtr,
                                         List<String> classes);

    private native List<String> nativeGetClasses(long nativeMapViewPtr);

    private native void nativeSetStyleUrl(long nativeMapViewPtr, String url);

    private native void nativeSetStyleJson(long nativeMapViewPtr, String newStyleJson);

    private native String nativeGetStyleJson(long nativeMapViewPtr);

    private native void nativeSetAccessToken(long nativeMapViewPtr, String accessToken);

    private native String nativeGetAccessToken(long nativeMapViewPtr);

    private native void nativeCancelTransitions(long nativeMapViewPtr);

    private native void nativeSetGestureInProgress(long nativeMapViewPtr, boolean inProgress);

    private native void nativeMoveBy(long nativeMapViewPtr, double dx,
                                     double dy, long duration);

    private native void nativeSetLatLng(long nativeMapViewPtr, double latitude, double longitude,
                                        long duration);

    private native LatLng nativeGetLatLng(long nativeMapViewPtr);

    private native void nativeResetPosition(long nativeMapViewPtr);

    private native double nativeGetPitch(long nativeMapViewPtr);

    private native void nativeSetPitch(long nativeMapViewPtr, double pitch, long duration);

    private native void nativeScaleBy(long nativeMapViewPtr, double ds,
                                      double cx, double cy, long duration);

    private native void nativeSetScale(long nativeMapViewPtr, double scale,
                                       double cx, double cy, long duration);

    private native double nativeGetScale(long nativeMapViewPtr);

    private native void nativeSetZoom(long nativeMapViewPtr, double zoom,
                                      long duration);

    private native double nativeGetZoom(long nativeMapViewPtr);

    private native void nativeResetZoom(long nativeMapViewPtr);

    private native void nativeSetMinZoom(long nativeMapViewPtr, double zoom);

    private native double nativeGetMinZoom(long nativeMapViewPtr);

    private native void nativeSetMaxZoom(long nativeMapViewPtr, double zoom);

    private native double nativeGetMaxZoom(long nativeMapViewPtr);

    private native void nativeRotateBy(long nativeMapViewPtr, double sx,
                                       double sy, double ex, double ey, long duration);

    private native void nativeSetContentPadding(long nativeMapViewPtr, double top, double left, double bottom, double right);

    private native void nativeSetBearing(long nativeMapViewPtr, double degrees,
                                         long duration);

    private native void nativeSetBearingXY(long nativeMapViewPtr, double degrees,
                                           double cx, double cy);

    private native double nativeGetBearing(long nativeMapViewPtr);

    private native void nativeResetNorth(long nativeMapViewPtr);

    private native void nativeUpdateMarker(long nativeMapViewPtr, long markerId, double lat, double lon, String iconId);

    private native long[] nativeAddMarkers(long nativeMapViewPtr, Marker[] markers);

    private native long[] nativeAddPolylines(long mNativeMapViewPtr, Polyline[] polylines);

    private native long[] nativeAddPolygons(long mNativeMapViewPtr, Polygon[] polygons);

    private native void nativeRemoveAnnotations(long nativeMapViewPtr, long[] id);

    private native long[] nativeQueryPointAnnotations(long mNativeMapViewPtr, RectF rect);

    private native void nativeAddAnnotationIcon(long nativeMapViewPtr, String symbol,
                                                int width, int height, float scale, byte[] pixels);

    private native void nativeSetVisibleCoordinateBounds(long mNativeMapViewPtr, LatLng[] coordinates,
                                                         RectF padding, double direction, long duration);

    private native void nativeOnLowMemory(long nativeMapViewPtr);

    private native void nativeSetDebug(long nativeMapViewPtr, boolean debug);

    private native void nativeToggleDebug(long nativeMapViewPtr);

    private native boolean nativeGetDebug(long nativeMapViewPtr);

    private native boolean nativeIsFullyLoaded(long nativeMapViewPtr);

    private native void nativeSetReachability(long nativeMapViewPtr, boolean status);

    private native double nativeGetMetersPerPixelAtLatitude(long nativeMapViewPtr, double lat, double zoom);

    private native ProjectedMeters nativeProjectedMetersForLatLng(long nativeMapViewPtr, double latitude, double longitude);

    private native LatLng nativeLatLngForProjectedMeters(long nativeMapViewPtr, double northing, double easting);

    private native PointF nativePixelForLatLng(long nativeMapViewPtr, double lat, double lon);

    private native LatLng nativeLatLngForPixel(long nativeMapViewPtr, float x, float y);

    private native double nativeGetTopOffsetPixelsForAnnotationSymbol(long nativeMapViewPtr, String symbolName);

    private native void nativeJumpTo(long nativeMapViewPtr, double angle, double latitude, double longitude, double pitch, double zoom);

    private native void nativeEaseTo(long nativeMapViewPtr, double angle, double latitude, double longitude, long duration, double pitch, double zoom, boolean easingInterpolator);

    private native void nativeFlyTo(long nativeMapViewPtr, double angle, double latitude, double longitude, long duration, double pitch, double zoom);

    private native double[] nativeGetCameraValues(long mNativeMapViewPtr);

    private native Layer nativeGetLayer(long nativeMapViewPtr, String layerId);

    private native void nativeAddLayer(long nativeMapViewPtr, long layerPtr, String before);

    private native void nativeRemoveLayer(long nativeMapViewPtr, String layerId) throws NoSuchLayerException;

    private native void nativeAddSource(long mNativeMapViewPtr, String id, Source source);

    private native void nativeRemoveSource(long mNativeMapViewPtr, String sourceId) throws NoSuchSourceException;

    private native void nativeUpdatePolygon(long nativeMapViewPtr, long polygonId, Polygon polygon);

    private native void nativeUpdatePolyline(long nativeMapviewPtr, long polylineId, Polyline polyline);

    private native void nativeScheduleTakeSnapshot(long nativeMapViewPtr);

    private native Feature[] nativeQueryRenderedFeaturesForPoint(long nativeMapViewPtr, float x, float y, String[] layerIds);

    private native Feature[] nativeQueryRenderedFeaturesForBox(long mNativeMapViewPtr, float left, float top, float right, float bottom, String[] layerIds);
}
