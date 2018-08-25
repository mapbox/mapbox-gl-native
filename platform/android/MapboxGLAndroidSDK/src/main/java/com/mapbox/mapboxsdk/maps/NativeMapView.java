package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.AsyncTask;
import android.os.Handler;
import android.support.annotation.IntRange;
import android.support.annotation.Keep;
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
import com.mapbox.mapboxsdk.exceptions.CalledFromWorkerThreadException;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CannotAddLayerException;
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
import java.util.concurrent.CopyOnWriteArrayList;

// Class that wraps the native methods for convenience
final class NativeMapView {

  private static final String TAG = "Mbgl-NativeMapView";

  //Hold a reference to prevent it from being GC'd as long as it's used on the native side
  private final FileSource fileSource;

  // Used to schedule work on the MapRenderer Thread
  private final MapRenderer mapRenderer;

  // Used to validate if methods are called from the correct thread
  private final Thread thread;

  // Used for callbacks
  private ViewCallback viewCallback;

  // Device density
  private final float pixelRatio;

  // Flag to indicating destroy was called
  private boolean destroyed = false;

  // Holds the pointer to JNI NativeMapView
  @Keep
  long nativePtr = 0;

  // Listener invoked to return a bitmap of the map
  private MapboxMap.SnapshotReadyCallback snapshotReadyCallback;

  private final CopyOnWriteArrayList<MapView.OnMapChangedListener> onMapChangedListeners = new CopyOnWriteArrayList<>();

  static {
    LibraryLoader.load();
  }

  //
  // Constructors
  //

  public NativeMapView(final Context context, final ViewCallback viewCallback, final MapRenderer mapRenderer) {
    this(context, context.getResources().getDisplayMetrics().density, viewCallback, mapRenderer);
  }

  public NativeMapView(final Context context, float pixelRatio,
                       final ViewCallback viewCallback, final MapRenderer mapRenderer) {
    this.mapRenderer = mapRenderer;
    this.viewCallback = viewCallback;
    this.fileSource = FileSource.getInstance(context);
    this.pixelRatio = pixelRatio;
    this.thread = Thread.currentThread();
    nativeInitialize(this, fileSource, mapRenderer, pixelRatio);
  }

  //
  // Methods
  //

  private boolean checkState(String callingMethod) {
    // validate if invocation has occurred on the main thread
    if (thread != Thread.currentThread()) {
      throw new CalledFromWorkerThreadException(
        String.format(
          "Map interactions should happen on the UI thread. Method invoked from wrong thread is %s.",
          callingMethod)
      );
    }

    // validate if map has already been destroyed
    if (destroyed && !TextUtils.isEmpty(callingMethod)) {
      Logger.e(TAG, String.format(
        "You're calling `%s` after the `MapView` was destroyed, were you invoking it after `onDestroy()`?",
        callingMethod)
      );
    }
    return destroyed;
  }

  public void destroy() {
    destroyed = true;
    onMapChangedListeners.clear();
    viewCallback = null;
    nativeDestroy();
  }

  public void update() {
    if (checkState("update")) {
      return;
    }

    mapRenderer.requestRender();
  }

  public void resizeView(int width, int height) {
    if (checkState("resizeView")) {
      return;
    }
    width = (int) Math.ceil(width / pixelRatio);
    height = (int) Math.ceil(height / pixelRatio);

    if (width < 0) {
      throw new IllegalArgumentException("width cannot be negative.");
    }

    if (height < 0) {
      throw new IllegalArgumentException("height cannot be negative.");
    }

    if (width > 65535) {
      // we have seen edge cases where devices return incorrect values #6111
      Logger.e(TAG, String.format("Device returned an out of range width size, "
        + "capping value at 65535 instead of %s", width)
      );
      width = 65535;
    }

    if (height > 65535) {
      // we have seen edge cases where devices return incorrect values #6111
      Logger.e(TAG, String.format("Device returned an out of range height size, "
        + "capping value at 65535 instead of %s", height)
      );
      height = 65535;
    }

    nativeResizeView(width, height);
  }

  public void setStyleUrl(String url) {
    if (checkState("setStyleUrl")) {
      return;
    }
    nativeSetStyleUrl(url);
  }

  public String getStyleUrl() {
    if (checkState("getStyleUrl")) {
      return null;
    }
    return nativeGetStyleUrl();
  }

  public void setStyleJson(String newStyleJson) {
    if (checkState("setStyleJson")) {
      return;
    }
    nativeSetStyleJson(newStyleJson);
  }

  public String getStyleJson() {
    if (checkState("getStyleJson")) {
      return null;
    }
    return nativeGetStyleJson();
  }

  public void setLatLngBounds(LatLngBounds latLngBounds) {
    if (checkState("setLatLngBounds")) {
      return;
    }
    nativeSetLatLngBounds(latLngBounds);
  }

  public void cancelTransitions() {
    if (checkState("cancelTransitions")) {
      return;
    }
    nativeCancelTransitions();
  }

  public void setGestureInProgress(boolean inProgress) {
    if (checkState("setGestureInProgress")) {
      return;
    }
    nativeSetGestureInProgress(inProgress);
  }

  public void moveBy(double dx, double dy) {
    if (checkState("moveBy")) {
      return;
    }
    moveBy(dx, dy, 0);
  }

  public void moveBy(double dx, double dy, long duration) {
    if (checkState("moveBy")) {
      return;
    }
    nativeMoveBy(dx / pixelRatio, dy / pixelRatio, duration);
  }

  public void setLatLng(LatLng latLng) {
    if (checkState("setLatLng")) {
      return;
    }
    setLatLng(latLng, 0);
  }

  public void setLatLng(LatLng latLng, long duration) {
    if (checkState("setLatLng")) {
      return;
    }
    nativeSetLatLng(latLng.getLatitude(), latLng.getLongitude(), duration);
  }

  public LatLng getLatLng() {
    if (checkState("")) {
      return new LatLng();
    }
    // wrap longitude values coming from core
    return nativeGetLatLng().wrap();
  }

  public CameraPosition getCameraForLatLngBounds(LatLngBounds bounds, int[] padding, double bearing, double tilt) {
    if (checkState("getCameraForLatLngBounds")) {
      return null;
    }
    return nativeGetCameraForLatLngBounds(
      bounds,
      padding[1] / pixelRatio,
      padding[0] / pixelRatio,
      padding[3] / pixelRatio,
      padding[2] / pixelRatio,
      bearing,
      tilt
    );
  }

  public CameraPosition getCameraForGeometry(Geometry geometry, int[] padding, double bearing, double tilt) {
    if (checkState("getCameraForGeometry")) {
      return null;
    }
    return nativeGetCameraForGeometry(
      geometry,
      padding[1] / pixelRatio,
      padding[0] / pixelRatio,
      padding[3] / pixelRatio,
      padding[2] / pixelRatio,
      bearing,
      tilt
    );
  }

  public void resetPosition() {
    if (checkState("resetPosition")) {
      return;
    }
    nativeResetPosition();
  }

  public double getPitch() {
    if (checkState("getPitch")) {
      return 0;
    }
    return nativeGetPitch();
  }

  public void setPitch(double pitch, long duration) {
    if (checkState("setPitch")) {
      return;
    }
    nativeSetPitch(pitch, duration);
  }

  public void setZoom(double zoom, PointF focalPoint, long duration) {
    if (checkState("setZoom")) {
      return;
    }
    nativeSetZoom(zoom, focalPoint.x / pixelRatio, focalPoint.y / pixelRatio, duration);
  }

  public double getZoom() {
    if (checkState("getZoom")) {
      return 0;
    }
    return nativeGetZoom();
  }

  public void resetZoom() {
    if (checkState("resetZoom")) {
      return;
    }
    nativeResetZoom();
  }

  public void setMinZoom(double zoom) {
    if (checkState("setMinZoom")) {
      return;
    }
    nativeSetMinZoom(zoom);
  }

  public double getMinZoom() {
    if (checkState("getMinZoom")) {
      return 0;
    }
    return nativeGetMinZoom();
  }

  public void setMaxZoom(double zoom) {
    if (checkState("setMaxZoom")) {
      return;
    }
    nativeSetMaxZoom(zoom);
  }

  public double getMaxZoom() {
    if (checkState("getMaxZoom")) {
      return 0;
    }
    return nativeGetMaxZoom();
  }

  public void rotateBy(double sx, double sy, double ex, double ey) {
    if (checkState("rotateBy")) {
      return;
    }
    rotateBy(sx, sy, ex, ey, 0);
  }

  public void rotateBy(double sx, double sy, double ex, double ey,
                       long duration) {
    if (checkState("rotateBy")) {
      return;
    }
    nativeRotateBy(sx / pixelRatio, sy / pixelRatio, ex, ey, duration);
  }

  public void setContentPadding(int[] padding) {
    if (checkState("setContentPadding")) {
      return;
    }
    nativeSetContentPadding(
      padding[1] / pixelRatio,
      padding[0] / pixelRatio,
      padding[3] / pixelRatio,
      padding[2] / pixelRatio);
  }

  public void setBearing(double degrees) {
    if (checkState("setBearing")) {
      return;
    }
    setBearing(degrees, 0);
  }

  public void setBearing(double degrees, long duration) {
    if (checkState("setBearing")) {
      return;
    }
    nativeSetBearing(degrees, duration);
  }

  public void setBearing(double degrees, double cx, double cy) {
    if (checkState("setBearing")) {
      return;
    }
    setBearing(degrees, cx, cy, 0);
  }

  public void setBearing(double degrees, double fx, double fy, long duration) {
    if (checkState("setBearing")) {
      return;
    }
    nativeSetBearingXY(degrees, fx / pixelRatio, fy / pixelRatio, duration);
  }

  public double getBearing() {
    if (checkState("getBearing")) {
      return 0;
    }
    return nativeGetBearing();
  }

  public void resetNorth() {
    if (checkState("resetNorth")) {
      return;
    }
    nativeResetNorth();
  }

  public long addMarker(Marker marker) {
    if (checkState("addMarker")) {
      return 0;
    }
    Marker[] markers = {marker};
    return nativeAddMarkers(markers)[0];
  }

  public long[] addMarkers(List<Marker> markers) {
    if (checkState("addMarkers")) {
      return new long[] {};
    }
    return nativeAddMarkers(markers.toArray(new Marker[markers.size()]));
  }

  public long addPolyline(Polyline polyline) {
    if (checkState("addPolyline")) {
      return 0;
    }
    Polyline[] polylines = {polyline};
    return nativeAddPolylines(polylines)[0];
  }

  public long[] addPolylines(List<Polyline> polylines) {
    if (checkState("addPolylines")) {
      return new long[] {};
    }
    return nativeAddPolylines(polylines.toArray(new Polyline[polylines.size()]));
  }

  public long addPolygon(Polygon polygon) {
    if (checkState("addPolygon")) {
      return 0;
    }
    Polygon[] polygons = {polygon};
    return nativeAddPolygons(polygons)[0];
  }

  public long[] addPolygons(List<Polygon> polygons) {
    if (checkState("addPolygons")) {
      return new long[] {};
    }
    return nativeAddPolygons(polygons.toArray(new Polygon[polygons.size()]));
  }

  public void updateMarker(Marker marker) {
    if (checkState("updateMarker")) {
      return;
    }
    LatLng position = marker.getPosition();
    Icon icon = marker.getIcon();
    nativeUpdateMarker(marker.getId(), position.getLatitude(), position.getLongitude(), icon.getId());
  }

  public void updatePolygon(Polygon polygon) {
    if (checkState("updatePolygon")) {
      return;
    }
    nativeUpdatePolygon(polygon.getId(), polygon);
  }

  public void updatePolyline(Polyline polyline) {
    if (checkState("updatePolyline")) {
      return;
    }
    nativeUpdatePolyline(polyline.getId(), polyline);
  }

  public void removeAnnotation(long id) {
    if (checkState("removeAnnotation")) {
      return;
    }
    long[] ids = {id};
    removeAnnotations(ids);
  }

  public void removeAnnotations(long[] ids) {
    if (checkState("removeAnnotations")) {
      return;
    }
    nativeRemoveAnnotations(ids);
  }

  public long[] queryPointAnnotations(RectF rect) {
    if (checkState("queryPointAnnotations")) {
      return new long[] {};
    }
    return nativeQueryPointAnnotations(rect);
  }

  public long[] queryShapeAnnotations(RectF rectF) {
    if (checkState("queryShapeAnnotations")) {
      return new long[] {};
    }
    return nativeQueryShapeAnnotations(rectF);
  }

  public void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels) {
    if (checkState("addAnnotationIcon")) {
      return;
    }
    nativeAddAnnotationIcon(symbol, width, height, scale, pixels);
  }

  public void removeAnnotationIcon(String symbol) {
    if (checkState("removeAnnotationIcon")) {
      return;
    }
    nativeRemoveAnnotationIcon(symbol);
  }

  public void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
    if (checkState("setVisibleCoordinateBounds")) {
      return;
    }
    nativeSetVisibleCoordinateBounds(coordinates, padding, direction, duration);
  }

  public void onLowMemory() {
    if (checkState("onLowMemory")) {
      return;
    }
    nativeOnLowMemory();
  }

  public void setDebug(boolean debug) {
    if (checkState("setDebug")) {
      return;
    }
    nativeSetDebug(debug);
  }

  public void cycleDebugOptions() {
    if (checkState("cycleDebugOptions")) {
      return;
    }
    nativeCycleDebugOptions();
  }

  public boolean getDebug() {
    if (checkState("getDebug")) {
      return false;
    }
    return nativeGetDebug();
  }

  public boolean isFullyLoaded() {
    if (checkState("isFullyLoaded")) {
      return false;
    }
    return nativeIsFullyLoaded();
  }

  public void setReachability(boolean status) {
    if (checkState("setReachability")) {
      return;
    }
    nativeSetReachability(status);
  }

  public double getMetersPerPixelAtLatitude(double lat) {
    if (checkState("getMetersPerPixelAtLatitude")) {
      return 0;
    }
    return nativeGetMetersPerPixelAtLatitude(lat, getZoom()) / pixelRatio;
  }

  public ProjectedMeters projectedMetersForLatLng(LatLng latLng) {
    if (checkState("projectedMetersForLatLng")) {
      return null;
    }
    return nativeProjectedMetersForLatLng(latLng.getLatitude(), latLng.getLongitude());
  }

  public LatLng latLngForProjectedMeters(ProjectedMeters projectedMeters) {
    if (checkState("latLngForProjectedMeters")) {
      return new LatLng();
    }
    return nativeLatLngForProjectedMeters(projectedMeters.getNorthing(),
      projectedMeters.getEasting()).wrap();
  }

  public PointF pixelForLatLng(LatLng latLng) {
    if (checkState("pixelForLatLng")) {
      return new PointF();
    }
    PointF pointF = nativePixelForLatLng(latLng.getLatitude(), latLng.getLongitude());
    pointF.set(pointF.x * pixelRatio, pointF.y * pixelRatio);
    return pointF;
  }

  public LatLng latLngForPixel(PointF pixel) {
    if (checkState("latLngForPixel")) {
      return new LatLng();
    }
    return nativeLatLngForPixel(pixel.x / pixelRatio, pixel.y / pixelRatio).wrap();
  }

  public double getTopOffsetPixelsForAnnotationSymbol(String symbolName) {
    if (checkState("getTopOffsetPixelsForAnnotationSymbol")) {
      return 0;
    }
    return nativeGetTopOffsetPixelsForAnnotationSymbol(symbolName);
  }

  public void jumpTo(double angle, LatLng center, double pitch, double zoom) {
    if (checkState("jumpTo")) {
      return;
    }
    nativeJumpTo(angle, center.getLatitude(), center.getLongitude(), pitch, zoom);
  }

  public void easeTo(double angle, LatLng center, long duration, double pitch, double zoom,
                     boolean easingInterpolator) {
    if (checkState("easeTo")) {
      return;
    }
    nativeEaseTo(angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom,
      easingInterpolator);
  }

  public void flyTo(double angle, LatLng center, long duration, double pitch, double zoom) {
    if (checkState("flyTo")) {
      return;
    }
    nativeFlyTo(angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom);
  }

  public CameraPosition getCameraPosition() {
    if (checkState("getCameraValues")) {
      return new CameraPosition.Builder().build();
    }
    return nativeGetCameraPosition();
  }

  public void setPrefetchesTiles(boolean enable) {
    if (checkState("setPrefetchesTiles")) {
      return;
    }
    nativeSetPrefetchesTiles(enable);
  }

  public boolean getPrefetchesTiles() {
    if (checkState("getPrefetchesTiles")) {
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
    if (checkState("getLayers")) {
      return null;
    }
    return Arrays.asList(nativeGetLayers());
  }

  public Layer getLayer(String layerId) {
    if (checkState("getLayer")) {
      return null;
    }
    return nativeGetLayer(layerId);
  }

  public void addLayer(@NonNull Layer layer) {
    if (checkState("addLayer")) {
      return;
    }
    nativeAddLayer(layer.getNativePtr(), null);
  }

  public void addLayerBelow(@NonNull Layer layer, @NonNull String below) {
    if (checkState("addLayerBelow")) {
      return;
    }
    nativeAddLayer(layer.getNativePtr(), below);
  }

  public void addLayerAbove(@NonNull Layer layer, @NonNull String above) {
    if (checkState("addLayerAbove")) {
      return;
    }
    nativeAddLayerAbove(layer.getNativePtr(), above);
  }

  public void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index) {
    if (checkState("addLayerAt")) {
      return;
    }
    nativeAddLayerAt(layer.getNativePtr(), index);
  }

  @Nullable
  public Layer removeLayer(@NonNull String layerId) {
    if (checkState("removeLayer")) {
      return null;
    }
    return nativeRemoveLayerById(layerId);
  }

  @Nullable
  public Layer removeLayer(@NonNull Layer layer) {
    if (checkState("removeLayer")) {
      return null;
    }
    nativeRemoveLayer(layer.getNativePtr());
    return layer;
  }

  @Nullable
  public Layer removeLayerAt(@IntRange(from = 0) int index) {
    if (checkState("removeLayerAt")) {
      return null;
    }
    return nativeRemoveLayerAt(index);
  }

  public List<Source> getSources() {
    if (checkState("getSources")) {
      return null;
    }
    return Arrays.asList(nativeGetSources());
  }

  public Source getSource(@NonNull String sourceId) {
    if (checkState("getSource")) {
      return null;
    }
    return nativeGetSource(sourceId);
  }

  public void addSource(@NonNull Source source) {
    if (checkState("addSource")) {
      return;
    }
    nativeAddSource(source, source.getNativePtr());
  }

  @Nullable
  public Source removeSource(@NonNull String sourceId) {
    if (checkState("removeSource")) {
      return null;
    }
    Source source = getSource(sourceId);
    if (source != null) {
      return removeSource(source);
    }
    return null;
  }

  @Nullable
  public Source removeSource(@NonNull Source source) {
    if (checkState("removeSource")) {
      return null;
    }
    nativeRemoveSource(source, source.getNativePtr());
    return source;
  }

  public void addImage(@NonNull String name, @NonNull Bitmap image, boolean sdf) {
    if (checkState("addImage")) {
      return;
    }

    // Determine pixel ratio, cast to float to avoid rounding, see mapbox-gl-native/issues/11809
    float pixelRatio = (float) image.getDensity() / DisplayMetrics.DENSITY_DEFAULT;
    nativeAddImage(name, image, pixelRatio, sdf);
  }

  public void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap) {
    if (checkState("addImages")) {
      return;
    }
    this.addImages(bitmapHashMap, false);
  }

  public void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap, boolean sdf) {
    if (checkState("addImages")) {
      return;
    }
    //noinspection unchecked
    new BitmapImageConversionTask(this, sdf).execute(bitmapHashMap);
  }

  public void removeImage(String name) {
    if (checkState("removeImage")) {
      return;
    }
    nativeRemoveImage(name);
  }

  public Bitmap getImage(String name) {
    if (checkState("getImage")) {
      return null;
    }
    return nativeGetImage(name);
  }

  // Feature querying

  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull PointF coordinates,
                                             @Nullable String[] layerIds,
                                             @Nullable Expression filter) {
    if (checkState("queryRenderedFeatures")) {
      return new ArrayList<>();
    }
    Feature[] features = nativeQueryRenderedFeaturesForPoint(coordinates.x / pixelRatio,
      coordinates.y / pixelRatio, layerIds, filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  @NonNull
  public List<Feature> queryRenderedFeatures(@NonNull RectF coordinates,
                                             @Nullable String[] layerIds,
                                             @Nullable Expression filter) {
    if (checkState("queryRenderedFeatures")) {
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
    if (checkState("setApiBaseUrl")) {
      return;
    }
    fileSource.setApiBaseUrl(baseUrl);
  }

  public Light getLight() {
    if (checkState("getLight")) {
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

  @Keep
  protected void onMapChanged(int rawChange) {
    for (MapView.OnMapChangedListener onMapChangedListener : onMapChangedListeners) {
      try {
        onMapChangedListener.onMapChanged(rawChange);
      } catch (RuntimeException err) {
        Logger.e(TAG, "Exception in MapView.OnMapChangedListener", err);
      }
    }
  }

  @Keep
  protected void onSnapshotReady(Bitmap mapContent) {
    if (checkState("OnSnapshotReady")) {
      return;
    }

    Bitmap viewContent = viewCallback.getViewContent();
    if (snapshotReadyCallback != null && mapContent != null && viewContent != null) {
      snapshotReadyCallback.onSnapshotReady(BitmapUtils.mergeBitmap(mapContent, viewContent));
    }
  }

  //
  // JNI methods
  //

  @Keep
  private native void nativeInitialize(NativeMapView nativeMapView,
                                       FileSource fileSource,
                                       MapRenderer mapRenderer,
                                       float pixelRatio);

  @Keep
  private native void nativeDestroy();

  @Keep
  private native void nativeResizeView(int width, int height);

  @Keep
  private native void nativeSetStyleUrl(String url);

  @Keep
  private native String nativeGetStyleUrl();

  @Keep
  private native void nativeSetStyleJson(String newStyleJson);

  @Keep
  private native String nativeGetStyleJson();

  @Keep
  private native void nativeSetLatLngBounds(LatLngBounds latLngBounds);

  @Keep
  private native void nativeCancelTransitions();

  @Keep
  private native void nativeSetGestureInProgress(boolean inProgress);

  @Keep
  private native void nativeMoveBy(double dx, double dy, long duration);

  @Keep
  private native void nativeSetLatLng(double latitude, double longitude, long duration);

  @Keep
  private native LatLng nativeGetLatLng();

  @Keep
  private native CameraPosition nativeGetCameraForLatLngBounds(
    LatLngBounds latLngBounds, double top, double left, double bottom, double right, double bearing, double tilt);

  @Keep
  private native CameraPosition nativeGetCameraForGeometry(
    Geometry geometry, double top, double left, double bottom, double right, double bearing, double tilt);

  @Keep
  private native void nativeResetPosition();

  @Keep
  private native double nativeGetPitch();

  @Keep
  private native void nativeSetPitch(double pitch, long duration);

  @Keep
  private native void nativeSetZoom(double zoom, double cx, double cy, long duration);

  @Keep
  private native double nativeGetZoom();

  @Keep
  private native void nativeResetZoom();

  @Keep
  private native void nativeSetMinZoom(double zoom);

  @Keep
  private native double nativeGetMinZoom();

  @Keep
  private native void nativeSetMaxZoom(double zoom);

  @Keep
  private native double nativeGetMaxZoom();

  @Keep
  private native void nativeRotateBy(double sx, double sy, double ex, double ey, long duration);

  @Keep
  private native void nativeSetContentPadding(double top, double left, double bottom, double right);

  @Keep
  private native void nativeSetBearing(double degrees, long duration);

  @Keep
  private native void nativeSetBearingXY(double degrees, double fx, double fy, long duration);

  @Keep
  private native double nativeGetBearing();

  @Keep
  private native void nativeResetNorth();

  @Keep
  private native void nativeUpdateMarker(long markerId, double lat, double lon, String iconId);

  @Keep
  private native long[] nativeAddMarkers(Marker[] markers);

  @Keep
  private native long[] nativeAddPolylines(Polyline[] polylines);

  @Keep
  private native long[] nativeAddPolygons(Polygon[] polygons);

  @Keep
  private native void nativeRemoveAnnotations(long[] id);

  @Keep
  private native long[] nativeQueryPointAnnotations(RectF rect);

  @Keep
  private native long[] nativeQueryShapeAnnotations(RectF rect);

  @Keep
  private native void nativeAddAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels);

  @Keep
  private native void nativeRemoveAnnotationIcon(String symbol);

  @Keep
  private native void nativeSetVisibleCoordinateBounds(LatLng[] coordinates, RectF padding,
                                                       double direction, long duration);

  @Keep
  private native void nativeOnLowMemory();

  @Keep
  private native void nativeSetDebug(boolean debug);

  @Keep
  private native void nativeCycleDebugOptions();

  @Keep
  private native boolean nativeGetDebug();

  @Keep
  private native boolean nativeIsFullyLoaded();

  @Keep
  private native void nativeSetReachability(boolean status);

  @Keep
  private native double nativeGetMetersPerPixelAtLatitude(double lat, double zoom);

  @Keep
  private native ProjectedMeters nativeProjectedMetersForLatLng(double latitude, double longitude);

  @Keep
  private native LatLng nativeLatLngForProjectedMeters(double northing, double easting);

  @Keep
  private native PointF nativePixelForLatLng(double lat, double lon);

  @Keep
  private native LatLng nativeLatLngForPixel(float x, float y);

  @Keep
  private native double nativeGetTopOffsetPixelsForAnnotationSymbol(String symbolName);

  @Keep
  private native void nativeJumpTo(double angle, double latitude, double longitude, double pitch, double zoom);

  @Keep
  private native void nativeEaseTo(double angle, double latitude, double longitude,
                                   long duration, double pitch, double zoom,
                                   boolean easingInterpolator);

  @Keep
  private native void nativeFlyTo(double angle, double latitude, double longitude,
                                  long duration, double pitch, double zoom);

  @Keep
  private native CameraPosition nativeGetCameraPosition();

  @Keep
  private native long nativeGetTransitionDuration();

  @Keep
  private native void nativeSetTransitionDuration(long duration);

  @Keep
  private native long nativeGetTransitionDelay();

  @Keep
  private native void nativeSetTransitionDelay(long delay);

  @Keep
  private native Layer[] nativeGetLayers();

  @Keep
  private native Layer nativeGetLayer(String layerId);

  @Keep
  private native void nativeAddLayer(long layerPtr, String before) throws CannotAddLayerException;

  @Keep
  private native void nativeAddLayerAbove(long layerPtr, String above) throws CannotAddLayerException;

  @Keep
  private native void nativeAddLayerAt(long layerPtr, int index) throws CannotAddLayerException;

  @Keep
  private native Layer nativeRemoveLayerById(String layerId);

  @Keep
  private native void nativeRemoveLayer(long layerId);

  @Keep
  private native Layer nativeRemoveLayerAt(int index);

  @Keep
  private native Source[] nativeGetSources();

  @Keep
  private native Source nativeGetSource(String sourceId);

  @Keep
  private native void nativeAddSource(Source source, long sourcePtr) throws CannotAddSourceException;

  @Keep
  private native void nativeRemoveSource(Source source, long sourcePtr);

  @Keep
  private native void nativeAddImage(String name, Bitmap bitmap, float pixelRatio, boolean sdf);

  @Keep
  private native void nativeAddImages(Image[] images);

  @Keep
  private native void nativeRemoveImage(String name);

  @Keep
  private native Bitmap nativeGetImage(String name);

  @Keep
  private native void nativeUpdatePolygon(long polygonId, Polygon polygon);

  @Keep
  private native void nativeUpdatePolyline(long polylineId, Polyline polyline);

  @Keep
  private native void nativeTakeSnapshot();

  @Keep
  private native Feature[] nativeQueryRenderedFeaturesForPoint(float x, float y,
                                                               String[] layerIds,
                                                               Object[] filter);

  @Keep
  private native Feature[] nativeQueryRenderedFeaturesForBox(float left, float top,
                                                             float right, float bottom,
                                                             String[] layerIds,
                                                             Object[] filter);

  @Keep
  private native Light nativeGetLight();

  @Keep
  private native void nativeSetPrefetchesTiles(boolean enable);

  @Keep
  private native boolean nativeGetPrefetchesTiles();

  int getWidth() {
    if (checkState("")) {
      return 0;
    }
    return viewCallback.getWidth();
  }

  int getHeight() {
    if (checkState("")) {
      return 0;
    }
    return viewCallback.getHeight();
  }

  //
  // MapChangeEvents
  //

  void addOnMapChangedListener(@NonNull MapView.OnMapChangedListener listener) {
    onMapChangedListeners.add(listener);
  }

  void removeOnMapChangedListener(@NonNull MapView.OnMapChangedListener listener) {
    if (onMapChangedListeners.contains(listener)) {
      onMapChangedListeners.remove(listener);
    }
  }

  //
  // Snapshot
  //

  void addSnapshotCallback(@NonNull MapboxMap.SnapshotReadyCallback callback) {
    if (checkState("addSnapshotCallback")) {
      return;
    }
    snapshotReadyCallback = callback;
    nativeTakeSnapshot();
  }

  public void setOnFpsChangedListener(final MapboxMap.OnFpsChangedListener listener) {
    final Handler handler = new Handler();
    mapRenderer.queueEvent(new Runnable() {

      @Override
      public void run() {
        mapRenderer.setOnFpsChangedListener(new MapboxMap.OnFpsChangedListener() {
          @Override
          public void onFpsChanged(final double fps) {
            handler.post(new Runnable() {

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
    private boolean sdf;

    BitmapImageConversionTask(NativeMapView nativeMapView, boolean sdf) {
      this.nativeMapView = nativeMapView;
      this.sdf = sdf;
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

        float pixelRatio = (float) bitmap.getDensity() / DisplayMetrics.DENSITY_DEFAULT;

        images.add(new Image(buffer.array(), pixelRatio, name, bitmap.getWidth(), bitmap.getHeight(), sdf));
      }

      return images;
    }

    @Override
    protected void onPostExecute(List<Image> images) {
      super.onPostExecute(images);
      if (nativeMapView != null && !nativeMapView.checkState("nativeAddImages")) {
        nativeMapView.nativeAddImages(images.toArray(new Image[images.size()]));
      }
    }
  }

  public interface ViewCallback {
    int getWidth();

    int getHeight();

    Bitmap getViewContent();
  }
}
