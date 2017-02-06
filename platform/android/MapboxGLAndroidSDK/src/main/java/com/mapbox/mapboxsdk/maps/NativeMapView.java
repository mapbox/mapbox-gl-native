package com.mapbox.mapboxsdk.maps;

import android.app.ActivityManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.DisplayMetrics;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import com.mapbox.mapboxsdk.offline.OfflineManager;
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.style.sources.CannotAddSourceException;
import com.mapbox.mapboxsdk.style.sources.NoSuchSourceException;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.services.commons.geojson.Feature;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import timber.log.Timber;

// Class that wraps the native methods for convenience
final class NativeMapView {

  // Holds the pointer to JNI NativeMapView
  private long nativePtr = 0;

  // Used for callbacks
  private MapView mapView;

  // Device density
  private final float pixelRatio;

  // Listeners for Map change events
  private CopyOnWriteArrayList<MapView.OnMapChangedListener> onMapChangedListeners = new CopyOnWriteArrayList<>();

  // Listener invoked to return a bitmap of the map
  private MapboxMap.SnapshotReadyCallback snapshotReadyCallback;

  //
  // Static methods
  //

  static {
    System.loadLibrary("mapbox-gl");
  }

  //
  // Constructors
  //

  NativeMapView(MapView mapView) {
    this.mapView = mapView;

    Context context = mapView.getContext();
    String cachePath = OfflineManager.getDatabasePath(context);

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

    initialize(this, cachePath, apkPath, pixelRatio, availableProcessors, totalMemory);
  }

  //
  // Methods
  //

  private native void initialize(NativeMapView nativeMapView, String cachePath, String apkPath, float pixelRatio,
                                 int availableProcessors, long totalMemory);

  native void destroy();

  native void render();

  native void setStyleUrl(String url);

  native String getStyleUrl();

  native void setStyleJson(String styleJson);

  native String getStyleJson();

  native void setAccessToken(String accessToken);

  native String getAccessToken();

  native void cancelTransitions();

  native void setGestureInProgress(boolean inProgress);

  native void setLatLng(double latitude, double longitude);

  native void resetPosition();

  native double getPitch();

  native void setPitch(double pitch);

  @Deprecated
  void setPitch(double pitch, int duration) {
    setPitch(pitch);
  }

  native double getScale();

  native void setZoom(double zoom);

  native double getZoom();

  native void resetZoom();

  native void setMinZoom(double zoom);

  native double getMinZoom();

  native void setMaxZoom(double zoom);

  native double getMaxZoom();

  void onViewportChanged(int width, int height) {
    if (width < 0) {
      throw new IllegalArgumentException("width cannot be negative.");
    }

    if (height < 0) {
      throw new IllegalArgumentException("height cannot be negative.");
    }

    if (width > 65535) {
      // we have seen edge cases where devices return incorrect values #6111
      Timber.e("Device returned an out of range width size, "
        + "capping value at 65535 instead of " + width);
      width = 65535;
    }

    if (height > 65535) {
      // we have seen edge cases where devices return incorrect values #6111
      Timber.e("Device returned an out of range height size, "
        + "capping value at 65535 instead of " + height);
      height = 65535;
    }

    _onViewportChanged(width, height);
  }

  private native void _onViewportChanged(int width, int height);

  void update() {
    //TODO, this circle makes little sense atm...
    Timber.w("TODO; Implement update()");
    onInvalidate();
  }


  void moveBy(double dx, double dy) {
    Timber.i("Move by %sx%s", dx, dy);
    _moveBy(dx / pixelRatio, dy / pixelRatio);
  }

  private native void _moveBy(double dx, double dy);

  @Deprecated
  void moveBy(double dx, double dy, long duration) {
    moveBy(dx, dy);
  }

  void setLatLng(LatLng latLng) {
    Timber.i("setLatLng %sx%s - %s", latLng.getLatitude(), latLng.getLongitude());
    setLatLng(latLng.getLatitude(), latLng.getLongitude());
  }

  @Deprecated
  void setLatLng(LatLng latLng, long duration) {
    setLatLng(latLng);
  }

  LatLng getLatLng() {
    // wrap longitude values coming from core
    return _getLatLng().wrap();
  }

  private native LatLng _getLatLng();

  void scaleBy(double ds) {
    scaleBy(ds, Double.NaN, Double.NaN);
  }

  void scaleBy(double ds, double cx, double cy) {
    _scaleBy(ds, cx / pixelRatio, cy / pixelRatio);
  }

  private native void _scaleBy(double ds, double cx, double cy);

  @Deprecated
  void scaleBy(double ds, double cx, double cy, long duration) {
    scaleBy(ds, cx, cy);
  }

  void setScale(double scale) {
    setScale(scale, Double.NaN, Double.NaN);
  }

  void setScale(double scale, double cx, double cy) {
    _setScale(scale, cx / pixelRatio, cy / pixelRatio);
  }

  private native void _setScale(double scale, double cx, double cy);

  @Deprecated
  void setScale(double scale, double cx, double cy, long duration) {
    setScale(scale, cx, cy);
  }

  @Deprecated
  void setZoom(double zoom, long duration) {
    setZoom(zoom);
  }

  void rotateBy(double sx, double sy, double ex, double ey) {
    _rotateBy(sx / pixelRatio, sy / pixelRatio, ex, ey);
  }

  private native void _rotateBy(double sx, double sy, double ex, double ey);

  @Deprecated
  void rotateBy(double sx, double sy, double ex, double ey, long duration) {
    rotateBy(sx, sy, ex, ey);
  }

  void setContentPadding(int[] padding) {
    setContentPadding(
      padding[1] / pixelRatio,
      padding[0] / pixelRatio,
      padding[3] / pixelRatio,
      padding[2] / pixelRatio);
  }

  native void setContentPadding(double top, double left, double bottom, double right);

  @Deprecated
  void setBearing(double degrees, long duration) {
    setBearing(degrees);
  }

  native void setBearing(double degrees);

  void setBearing(double degrees, double cx, double cy) {
    _setBearingXY(degrees, cx / pixelRatio, cy / pixelRatio);
  }

  @Deprecated
  void setBearing(double degrees, double cx, double cy, long duration) {
    setBearing(degrees, cx, cy);
  }

  private native void _setBearingXY(double degrees, double cx, double cy);

  native double getBearing();

  native void resetNorth();

  long addMarker(Marker marker) {
    Marker[] markers = {marker};
    return addMarkers(markers)[0];
  }

  long[] addMarkers(List<Marker> markers) {
    return addMarkers(markers.toArray(new Marker[markers.size()]));
  }

  native long[] addMarkers(Marker... markers);

  long addPolyline(Polyline polyline) {
    Polyline[] polylines = {polyline};
    return addPolylines(polylines)[0];
  }

  long[] addPolylines(List<Polyline> polylines) {
    return addPolylines(polylines.toArray(new Polyline[polylines.size()]));
  }

  native long[] addPolylines(Polyline[] polylines);

  long addPolygon(Polygon polygon) {
    Polygon[] polygons = {polygon};
    return addPolygons(polygons)[0];
  }

  long[] addPolygons(List<Polygon> polygons) {
    return addPolygons(polygons.toArray(new Polygon[polygons.size()]));
  }

  native long[] addPolygons(Polygon[] polygons);

  void updateMarker(Marker marker) {
    LatLng position = marker.getPosition();
    updateMarker(marker.getId(), position.getLatitude(), position.getLongitude(), marker.getIcon().getId());
  }

  private native void updateMarker(long markerId, double lat, double lon, String iconId);

  void updatePolygon(Polygon polygon) {
    updatePolygon(polygon.getId(), polygon);
  }

  private native void updatePolygon(long polygonId, Polygon polygon);

  void updatePolyline(Polyline polyline) {
    updatePolyline(polyline.getId(), polyline);
  }

  private native void updatePolyline(long polylineId, Polyline polyline);

  void removeAnnotation(long id) {
    long[] ids = {id};
    removeAnnotations(ids);
  }

  native void removeAnnotations(long[] id);

  native long[] queryPointAnnotations(RectF rect);

  native void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels);

  @Deprecated
  void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
    setVisibleCoordinateBounds(coordinates, padding, direction);
  }

  native void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction);

  native void onLowMemory();

  native void setDebug(boolean debug);

  native void cycleDebugOptions();

  native boolean getDebug();

  native boolean isFullyLoaded();

  double getMetersPerPixelAtLatitude(double lat) {
    return getMetersPerPixelAtLatitude(lat, getZoom());
  }

  private native double getMetersPerPixelAtLatitude(double lat, double zoom);

  ProjectedMeters projectedMetersForLatLng(LatLng latLng) {
    return projectedMetersForLatLng(latLng.getLatitude(), latLng.getLongitude());
  }

  native ProjectedMeters projectedMetersForLatLng(double latitude, double longitude);

  LatLng latLngForProjectedMeters(ProjectedMeters projectedMeters) {
    return latLngForProjectedMeters(projectedMeters.getNorthing(), projectedMeters.getEasting()).wrap();
  }

  private native LatLng latLngForProjectedMeters(double northing, double easting);

  LatLng latLngForPixel(PointF pixel) {
    return latLngForPixel(pixel.x / pixelRatio, pixel.y / pixelRatio).wrap();
  }

  private native LatLng latLngForPixel(float x, float y);

  PointF pixelForLatLng(LatLng latLng) {
    PointF pointF = pixelForLatLng(latLng.getLatitude(), latLng.getLongitude());
    pointF.set(pointF.x * pixelRatio, pointF.y * pixelRatio);
    return pointF;
  }

  private native PointF pixelForLatLng(double lat, double lon);

  native double getTopOffsetPixelsForAnnotationSymbol(String symbolName);

  @Deprecated
  void jumpTo(double angle, LatLng center, double pitch, double zoom) {
    Timber.w("Deprecated");
  }

  @Deprecated
  void easeTo(double angle, LatLng center, long duration, double pitch, double zoom,
              boolean easingInterpolator) {
    Timber.w("Deprecated");
  }

  @Deprecated
  void flyTo(double angle, LatLng center, long duration, double pitch, double zoom) {
    Timber.w("Deprecated");
  }

  // Runtime style Api

  native long getTransitionDuration();

  native void setTransitionDuration(long duration);

  native long getTransitionDelay();

  native void setTransitionDelay(long delay);

  native Layer getLayer(String layerId);

  void addLayer(@NonNull Layer layer, @Nullable String before) throws CannotAddLayerException {
    addLayer(layer.getNativePtr(), before);
  }

  private native void addLayer(long layerPtr, String before) throws CannotAddLayerException;

  void removeLayer(@NonNull String layerId) throws NoSuchLayerException {
    removeLayerById(layerId);
  }

  private native void removeLayerById(String layerId) throws NoSuchLayerException;

  void removeLayer(@NonNull Layer layer) throws NoSuchLayerException {
    removeLayer(layer.getNativePtr());
  }

  private native void removeLayer(long layerId) throws NoSuchLayerException;

  native Source getSource(String sourceId);

  void addSource(@NonNull Source source) throws CannotAddSourceException {
    addSource(source.getNativePtr());
  }

  private native void addSource(long nativeSourcePtr) throws CannotAddSourceException;

  void removeSource(@NonNull String sourceId) throws NoSuchSourceException {
    removeSourceById(sourceId);
  }

  private native void removeSourceById(String sourceId) throws NoSuchSourceException;

  void removeSource(@NonNull Source source) throws NoSuchSourceException {
    removeSource(source.getNativePtr());
  }

  private native void removeSource(long sourcePtr) throws NoSuchSourceException;

  void addImage(@NonNull String name, @NonNull Bitmap image) {
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

    addImage(name, image.getWidth(), image.getHeight(), pixelRatio, buffer.array());
  }

  private native void addImage(String name, int width, int height, float pixelRatio, byte[] array);

  native void removeImage(String name);

  // Feature querying //

  @NonNull
  List<Feature> queryRenderedFeatures(PointF coordinates, String... layerIds) {
    Feature[] features = queryRenderedFeaturesForPoint(coordinates.x / pixelRatio,
      coordinates.y / pixelRatio, layerIds);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  private native Feature[] queryRenderedFeaturesForPoint(float x, float y, String[] layerIds);

  @NonNull
  List<Feature> queryRenderedFeatures(RectF coordinates, String... layerIds) {
    Feature[] features = queryRenderedFeaturesForBox(
      coordinates.left / pixelRatio,
      coordinates.top / pixelRatio,
      coordinates.right / pixelRatio,
      coordinates.bottom / pixelRatio,
      layerIds);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  private native Feature[] queryRenderedFeaturesForBox(float left, float top, float right,
                                                       float bottom, String[] layerIds);

  float getPixelRatio() {
    return pixelRatio;
  }

  Context getContext() {
    return mapView.getContext();
  }

  //
  // Callbacks
  //

  /**
   * Called through JNI when the map needs to be re-rendered
   */
  protected void onInvalidate() {
    Timber.i("onInvalidate");
    mapView.onInvalidate();
  }

  /**
   * Called through JNI when the render thread needs to be woken up
   */
  protected void onWake() {
    Timber.i("wake!");
    mapView.requestRender();
  }

  /**
   * Called through JNI when the map state changed
   *
   * @param rawChange the mbgl::MapChange as an int
   */
  protected void onMapChanged(final int rawChange) {
    Timber.i("onMapChanged: %s", rawChange);
    if (onMapChangedListeners != null) {
      for (final MapView.OnMapChangedListener onMapChangedListener : onMapChangedListeners) {
        mapView.post(new Runnable() {
          @Override
          public void run() {
            onMapChangedListener.onMapChanged(rawChange);
          }
        });
      }
    }
  }

  /**
   * Called through JNI if fps is enabled and the fps changed
   *
   * @param fps the Frames Per Second
   */
  protected void onFpsChanged(double fps) {
    mapView.onFpsChanged(fps);
  }

  /**
   * Called through JNI when a requested snapshot is ready
   *
   * @param bitmap the snapshot as a bitmap
   */
  protected void onSnapshotReady(Bitmap bitmap) {
    if (snapshotReadyCallback != null && bitmap != null) {
      snapshotReadyCallback.onSnapshotReady(bitmap);
    }
  }

  native void setReachability(boolean status);

  native double[] getCameraValues();

  native void scheduleSnapshot();

  native void setApiBaseUrl(String baseUrl);

  native void enableFps(boolean enable);

  int getWidth() {
    return mapView.getWidth();
  }

  int getHeight() {
    return mapView.getHeight();
  }

  //
  // MapChangeEvents
  //

  void addOnMapChangedListener(@NonNull MapView.OnMapChangedListener listener) {
    onMapChangedListeners.add(listener);
  }

  void removeOnMapChangedListener(@NonNull MapView.OnMapChangedListener listener) {
    onMapChangedListeners.remove(listener);
  }

  //
  // Snapshot
  //

  void addSnapshotCallback(@NonNull MapboxMap.SnapshotReadyCallback callback) {
    snapshotReadyCallback = callback;
    scheduleSnapshot();
    render();
  }
}
