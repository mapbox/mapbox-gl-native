package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.AsyncTask;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.util.DisplayMetrics;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Geometry;
import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException;
import com.mapbox.mapboxsdk.style.layers.Filter;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.CannotAddSourceException;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import timber.log.Timber;

// Class that wraps the native methods for convenience
final class NativeMapView {

  // Flag to indicating destroy was called
  private boolean destroyed = false;

  // Holds the pointer to JNI NativeMapView
  private long nativePtr = 0;

  // Used for callbacks
  private MapView mapView;

  //Hold a reference to prevent it from being GC'd as long as it's used on the native side
  private final FileSource fileSource;

  // Used to schedule work on the MapRenderer Thread
  private MapRenderer mapRenderer;

  // Device density
  private final float pixelRatio;

  // Listener invoked to return a bitmap of the map
  private MapboxMap.SnapshotReadyCallback snapshotReadyCallback;

  static {
    LibraryLoader.load();
  }

  //
  // Constructors
  //

  public NativeMapView(final MapView mapView, MapRenderer mapRenderer) {
    this.mapRenderer = mapRenderer;
    this.mapView = mapView;

    Context context = mapView.getContext();
    fileSource = FileSource.getInstance(context);
    pixelRatio = context.getResources().getDisplayMetrics().density;

    nativeInitialize(this, fileSource, mapRenderer, pixelRatio);
  }

  //
  // Methods
  //

  private boolean isDestroyedOn(String callingMethod) {
    if (destroyed && !TextUtils.isEmpty(callingMethod)) {
      Timber.e(
        "You're calling `%s` after the `MapView` was destroyed, were you invoking it after `onDestroy()`?",
        callingMethod
      );
    }
    return destroyed;
  }

  public void destroy() {
    nativeDestroy();
    mapView = null;
    destroyed = true;
  }

  public void update() {
    if (isDestroyedOn("update")) {
      return;
    }

    mapRenderer.requestRender();
  }

  public void resizeView(int width, int height) {
    if (isDestroyedOn("resizeView")) {
      return;
    }
    width = (int) (width / pixelRatio);
    height = (int) (height / pixelRatio);

    if (width < 0) {
      throw new IllegalArgumentException("width cannot be negative.");
    }

    if (height < 0) {
      throw new IllegalArgumentException("height cannot be negative.");
    }

    if (width > 65535) {
      // we have seen edge cases where devices return incorrect values #6111
      Timber.e("Device returned an out of range width size, "
        + "capping value at 65535 instead of %s", width);
      width = 65535;
    }

    if (height > 65535) {
      // we have seen edge cases where devices return incorrect values #6111
      Timber.e("Device returned an out of range height size, "
        + "capping value at 65535 instead of %s", height);
      height = 65535;
    }

    nativeResizeView(width, height);
  }

  public void setStyleUrl(String url) {
    if (isDestroyedOn("setStyleUrl")) {
      return;
    }
    nativeSetStyleUrl(url);
  }

  public String getStyleUrl() {
    if (isDestroyedOn("getStyleUrl")) {
      return null;
    }
    return nativeGetStyleUrl();
  }

  public void setStyleJson(String newStyleJson) {
    if (isDestroyedOn("setStyleJson")) {
      return;
    }
    nativeSetStyleJson(newStyleJson);
  }

  public String getStyleJson() {
    if (isDestroyedOn("getStyleJson")) {
      return null;
    }
    return nativeGetStyleJson();
  }

  public void setLatLngBounds(LatLngBounds latLngBounds) {
    if (isDestroyedOn("setLatLngBounds")) {
      return;
    }
    nativeSetLatLngBounds(latLngBounds);
  }

  public void cancelTransitions() {
    if (isDestroyedOn("cancelTransitions")) {
      return;
    }
    nativeCancelTransitions();
  }

  public void setGestureInProgress(boolean inProgress) {
    if (isDestroyedOn("setGestureInProgress")) {
      return;
    }
    nativeSetGestureInProgress(inProgress);
  }

  public void moveBy(double dx, double dy) {
    if (isDestroyedOn("moveBy")) {
      return;
    }
    moveBy(dx, dy, 0);
  }

  public void moveBy(double dx, double dy, long duration) {
    if (isDestroyedOn("moveBy")) {
      return;
    }
    nativeMoveBy(dx / pixelRatio, dy / pixelRatio, duration);
  }

  public void setLatLng(LatLng latLng) {
    if (isDestroyedOn("setLatLng")) {
      return;
    }
    setLatLng(latLng, 0);
  }

  public void setLatLng(LatLng latLng, long duration) {
    if (isDestroyedOn("setLatLng")) {
      return;
    }
    nativeSetLatLng(latLng.getLatitude(), latLng.getLongitude(), duration);
  }

  public LatLng getLatLng() {
    if (isDestroyedOn("")) {
      return new LatLng();
    }
    // wrap longitude values coming from core
    return nativeGetLatLng().wrap();
  }

  public CameraPosition getCameraForLatLngBounds(LatLngBounds latLngBounds) {
    if (isDestroyedOn("getCameraForLatLngBounds")) {
      return null;
    }
    return nativeGetCameraForLatLngBounds(latLngBounds);
  }

  public CameraPosition getCameraForGeometry(Geometry geometry, double bearing) {
    if (isDestroyedOn("getCameraForGeometry")) {
      return null;
    }
    return nativeGetCameraForGeometry(geometry, bearing);
  }

  public void resetPosition() {
    if (isDestroyedOn("resetPosition")) {
      return;
    }
    nativeResetPosition();
  }

  public double getPitch() {
    if (isDestroyedOn("getPitch")) {
      return 0;
    }
    return nativeGetPitch();
  }

  public void setPitch(double pitch, long duration) {
    if (isDestroyedOn("setPitch")) {
      return;
    }
    nativeSetPitch(pitch, duration);
  }

  public void setZoom(double zoom, PointF focalPoint, long duration) {
    if (isDestroyedOn("setZoom")) {
      return;
    }
    nativeSetZoom(zoom, focalPoint.x / pixelRatio, focalPoint.y / pixelRatio, duration);
  }

  public double getZoom() {
    if (isDestroyedOn("getZoom")) {
      return 0;
    }
    return nativeGetZoom();
  }

  public void resetZoom() {
    if (isDestroyedOn("resetZoom")) {
      return;
    }
    nativeResetZoom();
  }

  public void setMinZoom(double zoom) {
    if (isDestroyedOn("setMinZoom")) {
      return;
    }
    nativeSetMinZoom(zoom);
  }

  public double getMinZoom() {
    if (isDestroyedOn("getMinZoom")) {
      return 0;
    }
    return nativeGetMinZoom();
  }

  public void setMaxZoom(double zoom) {
    if (isDestroyedOn("setMaxZoom")) {
      return;
    }
    nativeSetMaxZoom(zoom);
  }

  public double getMaxZoom() {
    if (isDestroyedOn("getMaxZoom")) {
      return 0;
    }
    return nativeGetMaxZoom();
  }

  public void rotateBy(double sx, double sy, double ex, double ey) {
    if (isDestroyedOn("rotateBy")) {
      return;
    }
    rotateBy(sx, sy, ex, ey, 0);
  }

  public void rotateBy(double sx, double sy, double ex, double ey,
                       long duration) {
    if (isDestroyedOn("rotateBy")) {
      return;
    }
    nativeRotateBy(sx / pixelRatio, sy / pixelRatio, ex, ey, duration);
  }

  public void setContentPadding(int[] padding) {
    if (isDestroyedOn("setContentPadding")) {
      return;
    }
    nativeSetContentPadding(
      padding[1] / pixelRatio,
      padding[0] / pixelRatio,
      padding[3] / pixelRatio,
      padding[2] / pixelRatio);
  }

  public void setBearing(double degrees) {
    if (isDestroyedOn("setBearing")) {
      return;
    }
    setBearing(degrees, 0);
  }

  public void setBearing(double degrees, long duration) {
    if (isDestroyedOn("setBearing")) {
      return;
    }
    nativeSetBearing(degrees, duration);
  }

  public void setBearing(double degrees, double cx, double cy) {
    if (isDestroyedOn("setBearing")) {
      return;
    }
    setBearing(degrees, cx, cy, 0);
  }

  public void setBearing(double degrees, double fx, double fy, long duration) {
    if (isDestroyedOn("setBearing")) {
      return;
    }
    nativeSetBearingXY(degrees, fx / pixelRatio, fy / pixelRatio, duration);
  }

  public double getBearing() {
    if (isDestroyedOn("getBearing")) {
      return 0;
    }
    return nativeGetBearing();
  }

  public void resetNorth() {
    if (isDestroyedOn("resetNorth")) {
      return;
    }
    nativeResetNorth();
  }

  public long addMarker(Marker marker) {
    if (isDestroyedOn("addMarker")) {
      return 0;
    }
    Marker[] markers = {marker};
    return nativeAddMarkers(markers)[0];
  }

  public long[] addMarkers(List<Marker> markers) {
    if (isDestroyedOn("addMarkers")) {
      return new long[] {};
    }
    return nativeAddMarkers(markers.toArray(new Marker[markers.size()]));
  }

  public long addPolyline(Polyline polyline) {
    if (isDestroyedOn("addPolyline")) {
      return 0;
    }
    Polyline[] polylines = {polyline};
    return nativeAddPolylines(polylines)[0];
  }

  public long[] addPolylines(List<Polyline> polylines) {
    if (isDestroyedOn("addPolylines")) {
      return new long[] {};
    }
    return nativeAddPolylines(polylines.toArray(new Polyline[polylines.size()]));
  }

  public long addPolygon(Polygon polygon) {
    if (isDestroyedOn("addPolygon")) {
      return 0;
    }
    Polygon[] polygons = {polygon};
    return nativeAddPolygons(polygons)[0];
  }

  public long[] addPolygons(List<Polygon> polygons) {
    if (isDestroyedOn("addPolygons")) {
      return new long[] {};
    }
    return nativeAddPolygons(polygons.toArray(new Polygon[polygons.size()]));
  }

  public void updateMarker(Marker marker) {
    if (isDestroyedOn("updateMarker")) {
      return;
    }
    LatLng position = marker.getPosition();
    Icon icon = marker.getIcon();
    nativeUpdateMarker(marker.getId(), position.getLatitude(), position.getLongitude(), icon.getId());
  }

  public void updatePolygon(Polygon polygon) {
    if (isDestroyedOn("updatePolygon")) {
      return;
    }
    nativeUpdatePolygon(polygon.getId(), polygon);
  }

  public void updatePolyline(Polyline polyline) {
    if (isDestroyedOn("updatePolyline")) {
      return;
    }
    nativeUpdatePolyline(polyline.getId(), polyline);
  }

  public void removeAnnotation(long id) {
    if (isDestroyedOn("removeAnnotation")) {
      return;
    }
    long[] ids = {id};
    removeAnnotations(ids);
  }

  public void removeAnnotations(long[] ids) {
    if (isDestroyedOn("removeAnnotations")) {
      return;
    }
    nativeRemoveAnnotations(ids);
  }

  public long[] queryPointAnnotations(RectF rect) {
    if (isDestroyedOn("queryPointAnnotations")) {
      return new long[] {};
    }
    return nativeQueryPointAnnotations(rect);
  }

  public long[] queryShapeAnnotations(RectF rectF) {
    if (isDestroyedOn("queryShapeAnnotations")) {
      return new long[] {};
    }
    return nativeQueryShapeAnnotations(rectF);
  }

  public void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels) {
    if (isDestroyedOn("addAnnotationIcon")) {
      return;
    }
    nativeAddAnnotationIcon(symbol, width, height, scale, pixels);
  }

  public void removeAnnotationIcon(String symbol) {
    if (isDestroyedOn("removeAnnotationIcon")) {
      return;
    }
    nativeRemoveAnnotationIcon(symbol);
  }

  public void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
    if (isDestroyedOn("setVisibleCoordinateBounds")) {
      return;
    }
    nativeSetVisibleCoordinateBounds(coordinates, padding, direction, duration);
  }

  public void onLowMemory() {
    if (isDestroyedOn("onLowMemory")) {
      return;
    }
    nativeOnLowMemory();
  }

  public void setDebug(boolean debug) {
    if (isDestroyedOn("setDebug")) {
      return;
    }
    nativeSetDebug(debug);
  }

  public void cycleDebugOptions() {
    if (isDestroyedOn("cycleDebugOptions")) {
      return;
    }
    nativeCycleDebugOptions();
  }

  public boolean getDebug() {
    if (isDestroyedOn("getDebug")) {
      return false;
    }
    return nativeGetDebug();
  }

  public boolean isFullyLoaded() {
    if (isDestroyedOn("isFullyLoaded")) {
      return false;
    }
    return nativeIsFullyLoaded();
  }

  public void setReachability(boolean status) {
    if (isDestroyedOn("setReachability")) {
      return;
    }
    nativeSetReachability(status);
  }

  public double getMetersPerPixelAtLatitude(double lat) {
    if (isDestroyedOn("getMetersPerPixelAtLatitude")) {
      return 0;
    }
    return nativeGetMetersPerPixelAtLatitude(lat, getZoom()) / pixelRatio;
  }

  public ProjectedMeters projectedMetersForLatLng(LatLng latLng) {
    if (isDestroyedOn("projectedMetersForLatLng")) {
      return null;
    }
    return nativeProjectedMetersForLatLng(latLng.getLatitude(), latLng.getLongitude());
  }

  public LatLng latLngForProjectedMeters(ProjectedMeters projectedMeters) {
    if (isDestroyedOn("latLngForProjectedMeters")) {
      return new LatLng();
    }
    return nativeLatLngForProjectedMeters(projectedMeters.getNorthing(),
      projectedMeters.getEasting()).wrap();
  }

  public PointF pixelForLatLng(LatLng latLng) {
    if (isDestroyedOn("pixelForLatLng")) {
      return new PointF();
    }
    PointF pointF = nativePixelForLatLng(latLng.getLatitude(), latLng.getLongitude());
    pointF.set(pointF.x * pixelRatio, pointF.y * pixelRatio);
    return pointF;
  }

  public LatLng latLngForPixel(PointF pixel) {
    if (isDestroyedOn("latLngForPixel")) {
      return new LatLng();
    }
    return nativeLatLngForPixel(pixel.x / pixelRatio, pixel.y / pixelRatio).wrap();
  }

  public double getTopOffsetPixelsForAnnotationSymbol(String symbolName) {
    if (isDestroyedOn("getTopOffsetPixelsForAnnotationSymbol")) {
      return 0;
    }
    return nativeGetTopOffsetPixelsForAnnotationSymbol(symbolName);
  }

  public void jumpTo(double angle, LatLng center, double pitch, double zoom) {
    if (isDestroyedOn("jumpTo")) {
      return;
    }
    nativeJumpTo(angle, center.getLatitude(), center.getLongitude(), pitch, zoom);
  }

  public void easeTo(double angle, LatLng center, long duration, double pitch, double zoom,
                     boolean easingInterpolator) {
    if (isDestroyedOn("easeTo")) {
      return;
    }
    nativeEaseTo(angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom,
      easingInterpolator);
  }

  public void flyTo(double angle, LatLng center, long duration, double pitch, double zoom) {
    if (isDestroyedOn("flyTo")) {
      return;
    }
    nativeFlyTo(angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom);
  }

  public CameraPosition getCameraPosition() {
    if (isDestroyedOn("getCameraValues")) {
      return new CameraPosition.Builder().build();
    }
    return nativeGetCameraPosition();
  }

  public void setPrefetchesTiles(boolean enable) {
    if (isDestroyedOn("setPrefetchesTiles")) {
      return;
    }
    nativeSetPrefetchesTiles(enable);
  }

  public boolean getPrefetchesTiles() {
    if (isDestroyedOn("getPrefetchesTiles")) {
      return false;
    }
    return nativeGetPrefetchesTiles();
  }

  // Runtime style Api

  public long getTransitionDuration() {
    return nativeGetTransitionDuration();
  }

  public void setTransitionDuration(long duration) {
    nativeSetTransitionDuration(duration);
  }

  public long getTransitionDelay() {
    return nativeGetTransitionDelay();
  }

  public void setTransitionDelay(long delay) {
    nativeSetTransitionDelay(delay);
  }

  public List<Layer> getLayers() {
    if (isDestroyedOn("getLayers")) {
      return null;
    }
    return Arrays.asList(nativeGetLayers());
  }

  public Layer getLayer(String layerId) {
    if (isDestroyedOn("getLayer")) {
      return null;
    }
    return nativeGetLayer(layerId);
  }

  public void addLayer(@NonNull Layer layer) {
    if (isDestroyedOn("addLayer")) {
      return;
    }
    nativeAddLayer(layer.getNativePtr(), null);
  }

  public void addLayerBelow(@NonNull Layer layer, @NonNull String below) {
    if (isDestroyedOn("addLayerBelow")) {
      return;
    }
    nativeAddLayer(layer.getNativePtr(), below);
  }

  public void addLayerAbove(@NonNull Layer layer, @NonNull String above) {
    if (isDestroyedOn("addLayerAbove")) {
      return;
    }
    nativeAddLayerAbove(layer.getNativePtr(), above);
  }

  public void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index) {
    if (isDestroyedOn("addLayerAt")) {
      return;
    }
    nativeAddLayerAt(layer.getNativePtr(), index);
  }

  @Nullable
  public Layer removeLayer(@NonNull String layerId) {
    if (isDestroyedOn("removeLayer")) {
      return null;
    }
    return nativeRemoveLayerById(layerId);
  }

  @Nullable
  public Layer removeLayer(@NonNull Layer layer) {
    if (isDestroyedOn("removeLayer")) {
      return null;
    }
    nativeRemoveLayer(layer.getNativePtr());
    return layer;
  }

  @Nullable
  public Layer removeLayerAt(@IntRange(from = 0) int index) {
    if (isDestroyedOn("removeLayerAt")) {
      return null;
    }
    return nativeRemoveLayerAt(index);
  }

  public List<Source> getSources() {
    if (isDestroyedOn("getSources")) {
      return null;
    }
    return Arrays.asList(nativeGetSources());
  }

  public Source getSource(@NonNull String sourceId) {
    if (isDestroyedOn("getSource")) {
      return null;
    }
    return nativeGetSource(sourceId);
  }

  public void addSource(@NonNull Source source) {
    if (isDestroyedOn("addSource")) {
      return;
    }
    nativeAddSource(source, source.getNativePtr());
  }

  @Nullable
  public Source removeSource(@NonNull String sourceId) {
    if (isDestroyedOn("removeSource")) {
      return null;
    }
    Source source = getSource(sourceId);
    return removeSource(source);
  }

  public Source removeSource(@NonNull Source source) {
    if (isDestroyedOn("removeSource")) {
      return null;
    }
    nativeRemoveSource(source, source.getNativePtr());
    return source;
  }

  public void addImage(@NonNull String name, @NonNull Bitmap image) {
    if (isDestroyedOn("addImage")) {
      return;
    }

    // Check/correct config
    if (image.getConfig() != Bitmap.Config.ARGB_8888) {
      image = image.copy(Bitmap.Config.ARGB_8888, false);
    }

    // Get pixels
    ByteBuffer buffer = ByteBuffer.allocate(image.getByteCount());
    image.copyPixelsToBuffer(buffer);

    // Determine pixel ratio
    float density = image.getDensity() == Bitmap.DENSITY_NONE ? Bitmap.DENSITY_NONE : image.getDensity();
    float pixelRatio = density / DisplayMetrics.DENSITY_DEFAULT;

    nativeAddImage(name, image.getWidth(), image.getHeight(), pixelRatio, buffer.array());
  }

  public void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap) {
    if (isDestroyedOn("addImages")) {
      return;
    }
    //noinspection unchecked
    new BitmapImageConversionTask(this).execute(bitmapHashMap);
  }

  public void removeImage(String name) {
    if (isDestroyedOn("removeImage")) {
      return;
    }
    nativeRemoveImage(name);
  }

  public Bitmap getImage(String name) {
    if (isDestroyedOn("getImage")) {
      return null;
    }
    return nativeGetImage(name);
  }

  // Feature querying

  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull PointF coordinates,
                                             @Nullable String[] layerIds,
                                             @Nullable Filter.Statement filter) {
    if (isDestroyedOn("queryRenderedFeatures")) {
      return new ArrayList<>();
    }
    Feature[] features = nativeQueryRenderedFeaturesForPoint(coordinates.x / pixelRatio,
      coordinates.y / pixelRatio, layerIds, filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull RectF coordinates,
                                             @Nullable String[] layerIds,
                                             @Nullable Filter.Statement filter) {
    if (isDestroyedOn("queryRenderedFeatures")) {
      return new ArrayList<>();
    }
    Feature[] features = nativeQueryRenderedFeaturesForBox(
      coordinates.left / pixelRatio,
      coordinates.top / pixelRatio,
      coordinates.right / pixelRatio,
      coordinates.bottom / pixelRatio,
      layerIds,
      filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  public void setApiBaseUrl(String baseUrl) {
    if (isDestroyedOn("setApiBaseUrl")) {
      return;
    }
    fileSource.setApiBaseUrl(baseUrl);
  }

  public Light getLight() {
    if (isDestroyedOn("getLight")) {
      return null;
    }
    return nativeGetLight();
  }

  public float getPixelRatio() {
    return pixelRatio;
  }

  RectF getDensityDependantRectangle(final RectF rectangle) {
    return new RectF(
      rectangle.left / pixelRatio,
      rectangle.top / pixelRatio,
      rectangle.right / pixelRatio,
      rectangle.bottom / pixelRatio
    );
  }

  //
  // Callbacks
  //

  protected void onMapChanged(int rawChange) {
    if (mapView != null) {
      mapView.onMapChange(rawChange);
    }
  }

  protected void onSnapshotReady(Bitmap mapContent) {
    if (isDestroyedOn("OnSnapshotReady")) {
      return;
    }

    Bitmap viewContent = BitmapUtils.createBitmapFromView(mapView);
    if (snapshotReadyCallback != null && mapContent != null && viewContent != null) {
      snapshotReadyCallback.onSnapshotReady(BitmapUtils.mergeBitmap(mapContent, viewContent));
    }
  }

  //
  // JNI methods
  //

  private native void nativeInitialize(NativeMapView nativeMapView,
                                       FileSource fileSource,
                                       MapRenderer mapRenderer,
                                       float pixelRatio);

  private native void nativeDestroy();

  private native void nativeResizeView(int width, int height);

  private native void nativeSetStyleUrl(String url);

  private native String nativeGetStyleUrl();

  private native void nativeSetStyleJson(String newStyleJson);

  private native String nativeGetStyleJson();

  private native void nativeSetLatLngBounds(LatLngBounds latLngBounds);

  private native void nativeCancelTransitions();

  private native void nativeSetGestureInProgress(boolean inProgress);

  private native void nativeMoveBy(double dx, double dy, long duration);

  private native void nativeSetLatLng(double latitude, double longitude, long duration);

  private native LatLng nativeGetLatLng();

  private native CameraPosition nativeGetCameraForLatLngBounds(LatLngBounds latLngBounds);

  private native CameraPosition nativeGetCameraForGeometry(Geometry geometry, double bearing);

  private native void nativeResetPosition();

  private native double nativeGetPitch();

  private native void nativeSetPitch(double pitch, long duration);

  private native void nativeSetZoom(double zoom, double cx, double cy, long duration);

  private native double nativeGetZoom();

  private native void nativeResetZoom();

  private native void nativeSetMinZoom(double zoom);

  private native double nativeGetMinZoom();

  private native void nativeSetMaxZoom(double zoom);

  private native double nativeGetMaxZoom();

  private native void nativeRotateBy(double sx, double sy, double ex, double ey, long duration);

  private native void nativeSetContentPadding(double top, double left, double bottom, double right);

  private native void nativeSetBearing(double degrees, long duration);

  private native void nativeSetBearingXY(double degrees, double fx, double fy, long duration);

  private native double nativeGetBearing();

  private native void nativeResetNorth();

  private native void nativeUpdateMarker(long markerId, double lat, double lon, String iconId);

  private native long[] nativeAddMarkers(Marker[] markers);

  private native long[] nativeAddPolylines(Polyline[] polylines);

  private native long[] nativeAddPolygons(Polygon[] polygons);

  private native void nativeRemoveAnnotations(long[] id);

  private native long[] nativeQueryPointAnnotations(RectF rect);

  private native long[] nativeQueryShapeAnnotations(RectF rect);

  private native void nativeAddAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels);

  private native void nativeRemoveAnnotationIcon(String symbol);

  private native void nativeSetVisibleCoordinateBounds(LatLng[] coordinates, RectF padding,
                                                       double direction, long duration);

  private native void nativeOnLowMemory();

  private native void nativeSetDebug(boolean debug);

  private native void nativeCycleDebugOptions();

  private native boolean nativeGetDebug();

  private native boolean nativeIsFullyLoaded();

  private native void nativeSetReachability(boolean status);

  private native double nativeGetMetersPerPixelAtLatitude(double lat, double zoom);

  private native ProjectedMeters nativeProjectedMetersForLatLng(double latitude, double longitude);

  private native LatLng nativeLatLngForProjectedMeters(double northing, double easting);

  private native PointF nativePixelForLatLng(double lat, double lon);

  private native LatLng nativeLatLngForPixel(float x, float y);

  private native double nativeGetTopOffsetPixelsForAnnotationSymbol(String symbolName);

  private native void nativeJumpTo(double angle, double latitude, double longitude, double pitch, double zoom);

  private native void nativeEaseTo(double angle, double latitude, double longitude,
                                   long duration, double pitch, double zoom,
                                   boolean easingInterpolator);

  private native void nativeFlyTo(double angle, double latitude, double longitude,
                                  long duration, double pitch, double zoom);

  private native CameraPosition nativeGetCameraPosition();

  private native long nativeGetTransitionDuration();

  private native void nativeSetTransitionDuration(long duration);

  private native long nativeGetTransitionDelay();

  private native void nativeSetTransitionDelay(long delay);

  private native Layer[] nativeGetLayers();

  private native Layer nativeGetLayer(String layerId);

  private native void nativeAddLayer(long layerPtr, String before) throws CannotAddLayerException;

  private native void nativeAddLayerAbove(long layerPtr, String above) throws CannotAddLayerException;

  private native void nativeAddLayerAt(long layerPtr, int index) throws CannotAddLayerException;

  private native Layer nativeRemoveLayerById(String layerId);

  private native void nativeRemoveLayer(long layerId);

  private native Layer nativeRemoveLayerAt(int index);

  private native Source[] nativeGetSources();

  private native Source nativeGetSource(String sourceId);

  private native void nativeAddSource(Source source, long sourcePtr) throws CannotAddSourceException;

  private native void nativeRemoveSource(Source source, long sourcePtr);

  private native void nativeAddImage(String name, int width, int height, float pixelRatio,
                                     byte[] array);

  private native void nativeAddImages(Image[] images);

  private native void nativeRemoveImage(String name);

  private native Bitmap nativeGetImage(String name);

  private native void nativeUpdatePolygon(long polygonId, Polygon polygon);

  private native void nativeUpdatePolyline(long polylineId, Polyline polyline);

  private native void nativeTakeSnapshot();

  private native Feature[] nativeQueryRenderedFeaturesForPoint(float x, float y,
                                                               String[] layerIds,
                                                               Object[] filter);

  private native Feature[] nativeQueryRenderedFeaturesForBox(float left, float top,
                                                             float right, float bottom,
                                                             String[] layerIds,
                                                             Object[] filter);

  private native Light nativeGetLight();

  private native void nativeSetPrefetchesTiles(boolean enable);

  private native boolean nativeGetPrefetchesTiles();

  int getWidth() {
    if (isDestroyedOn("")) {
      return 0;
    }
    return mapView.getWidth();
  }

  int getHeight() {
    if (isDestroyedOn("")) {
      return 0;
    }
    return mapView.getHeight();
  }

  //
  // MapChangeEvents
  //

  void addOnMapChangedListener(@NonNull MapView.OnMapChangedListener listener) {
    if (mapView != null) {
      mapView.addOnMapChangedListener(listener);
    }
  }

  void removeOnMapChangedListener(@NonNull MapView.OnMapChangedListener listener) {
    mapView.removeOnMapChangedListener(listener);
  }

  //
  // Snapshot
  //

  void addSnapshotCallback(@NonNull MapboxMap.SnapshotReadyCallback callback) {
    if (isDestroyedOn("addSnapshotCallback")) {
      return;
    }
    snapshotReadyCallback = callback;
    nativeTakeSnapshot();
  }

  public void setOnFpsChangedListener(final MapboxMap.OnFpsChangedListener listener) {
    mapRenderer.queueEvent(new Runnable() {

      @Override
      public void run() {
        mapRenderer.setOnFpsChangedListener(new MapboxMap.OnFpsChangedListener() {

          @Override
          public void onFpsChanged(final double fps) {
            mapView.post(new Runnable() {

              @Override
              public void run() {
                listener.onFpsChanged(fps);
              }

            });
          }

        });
      }

    });
  }


  //
  // Image conversion
  //

  private static class BitmapImageConversionTask extends AsyncTask<HashMap<String, Bitmap>, Void, List<Image>> {

    private NativeMapView nativeMapView;

    BitmapImageConversionTask(NativeMapView nativeMapView) {
      this.nativeMapView = nativeMapView;
    }

    @Override
    protected List<Image> doInBackground(HashMap<String, Bitmap>... params) {
      HashMap<String, Bitmap> bitmapHashMap = params[0];

      List<Image> images = new ArrayList<>();
      ByteBuffer buffer;
      String name;
      Bitmap bitmap;

      for (Map.Entry<String, Bitmap> stringBitmapEntry : bitmapHashMap.entrySet()) {
        name = stringBitmapEntry.getKey();
        bitmap = stringBitmapEntry.getValue();

        if (bitmap.getConfig() != Bitmap.Config.ARGB_8888) {
          bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, false);
        }

        buffer = ByteBuffer.allocate(bitmap.getByteCount());
        bitmap.copyPixelsToBuffer(buffer);

        float density = bitmap.getDensity() == Bitmap.DENSITY_NONE ? Bitmap.DENSITY_NONE : bitmap.getDensity();
        float pixelRatio = density / DisplayMetrics.DENSITY_DEFAULT;

        images.add(new Image(buffer.array(), pixelRatio, name, bitmap.getWidth(), bitmap.getHeight()));
      }

      return images;
    }

    @Override
    protected void onPostExecute(List<Image> images) {
      super.onPostExecute(images);
      if (nativeMapView != null && !nativeMapView.isDestroyedOn("nativeAddImages")) {
        nativeMapView.nativeAddImages(images.toArray(new Image[images.size()]));
      }
    }
  }
}
