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
import com.mapbox.mapboxsdk.MapStrictMode;
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

// Class that wraps the native methods for convenience
final class NativeMapView implements NativeMap {

  private static final String TAG = "Mbgl-NativeMapView";

  // Hold a reference to prevent it from being GC'd as long as it's used on the native side
  private final FileSource fileSource;

  // Used to schedule work on the MapRenderer Thread
  private final MapRenderer mapRenderer;

  // Used to validate if methods are called from the correct thread
  @NonNull
  private final Thread thread;

  // Used for view callbacks
  @Nullable
  private ViewCallback viewCallback;

  // Used for map change callbacks
  @Nullable
  private StateCallback stateCallback;

  // Device density
  private final float pixelRatio;

  // Flag to indicating destroy was called
  private boolean destroyed = false;

  // Holds the pointer to JNI NativeMapView
  @Keep
  long nativePtr = 0;

  // Listener invoked to return a bitmap of the map
  private MapboxMap.SnapshotReadyCallback snapshotReadyCallback;

  static {
    LibraryLoader.load();
  }

  //
  // Constructors
  //

  public NativeMapView(@NonNull final Context context, final boolean crossSourceCollisions,
                       final ViewCallback viewCallback, final StateCallback stateCallback,
                       final MapRenderer mapRenderer) {
    this(context, context.getResources().getDisplayMetrics().density, crossSourceCollisions, viewCallback,
      stateCallback, mapRenderer);
  }

  public NativeMapView(final Context context, final float pixelRatio, final boolean crossSourceCollisions,
                       final ViewCallback viewCallback, final StateCallback stateCallback,
                       final MapRenderer mapRenderer) {
    this.mapRenderer = mapRenderer;
    this.viewCallback = viewCallback;
    this.fileSource = FileSource.getInstance(context);
    this.pixelRatio = pixelRatio;
    this.thread = Thread.currentThread();
    this.stateCallback = stateCallback;
    nativeInitialize(this, fileSource, mapRenderer, pixelRatio, crossSourceCollisions);
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
      String message = String.format(
        "You're calling `%s` after the `MapView` was destroyed, were you invoking it after `onDestroy()`?",
        callingMethod);
      Logger.e(TAG, message);

      MapStrictMode.strictModeViolation(message);
    }
    return destroyed;
  }

  @Override
  public void destroy() {
    destroyed = true;
    viewCallback = null;
    nativeDestroy();
  }

  @Override
  public void update() {
    if (checkState("update")) {
      return;
    }

    mapRenderer.requestRender();
  }

  @Override
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

  @Override
  public void setStyleUrl(String url) {
    if (checkState("setStyleUrl")) {
      return;
    }
    nativeSetStyleUrl(url);
  }

  @Override
  @NonNull
  public String getStyleUrl() {
    if (checkState("getStyleUrl")) {
      return "";
    }
    return nativeGetStyleUrl();
  }

  @Override
  public void setStyleJson(String newStyleJson) {
    if (checkState("setStyleJson")) {
      return;
    }
    nativeSetStyleJson(newStyleJson);
  }

  @Override
  @NonNull
  public String getStyleJson() {
    if (checkState("getStyleJson")) {
      return "";
    }
    return nativeGetStyleJson();
  }

  @Override
  public void setLatLngBounds(LatLngBounds latLngBounds) {
    if (checkState("setLatLngBounds")) {
      return;
    }
    nativeSetLatLngBounds(latLngBounds);
  }

  @Override
  public void cancelTransitions() {
    if (checkState("cancelTransitions")) {
      return;
    }
    nativeCancelTransitions();
  }

  @Override
  public void setGestureInProgress(boolean inProgress) {
    if (checkState("setGestureInProgress")) {
      return;
    }
    nativeSetGestureInProgress(inProgress);
  }

  @Override
  public void moveBy(double dx, double dy) {
    if (checkState("moveBy")) {
      return;
    }
    moveBy(dx, dy, 0);
  }

  @Override
  public void moveBy(double dx, double dy, long duration) {
    if (checkState("moveBy")) {
      return;
    }
    nativeMoveBy(dx / pixelRatio, dy / pixelRatio, duration);
  }

  @Override
  public void setLatLng(@NonNull LatLng latLng) {
    if (checkState("setLatLng")) {
      return;
    }
    setLatLng(latLng, 0);
  }

  @Override
  public void setLatLng(@NonNull LatLng latLng, long duration) {
    if (checkState("setLatLng")) {
      return;
    }
    nativeSetLatLng(latLng.getLatitude(), latLng.getLongitude(), duration);
  }

  @Override
  public LatLng getLatLng() {
    if (checkState("")) {
      return new LatLng();
    }
    return nativeGetLatLng();
  }

  @Override
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

  @Override
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

  @Override
  public void resetPosition() {
    if (checkState("resetPosition")) {
      return;
    }
    nativeResetPosition();
  }

  @Override
  public double getPitch() {
    if (checkState("getPitch")) {
      return 0;
    }
    return nativeGetPitch();
  }

  @Override
  public void setPitch(double pitch) {
    setPitch(pitch, 0);
  }

  @Override
  public void setPitch(double pitch, long duration) {
    if (checkState("setPitch")) {
      return;
    }
    nativeSetPitch(pitch, duration);
  }

  @Override
  public void setZoom(double zoom, @NonNull PointF focalPoint, long duration) {
    if (checkState("setZoom")) {
      return;
    }
    nativeSetZoom(zoom, focalPoint.x / pixelRatio, focalPoint.y / pixelRatio, duration);
  }

  @Override
  public double getZoom() {
    if (checkState("getZoom")) {
      return 0;
    }
    return nativeGetZoom();
  }

  @Override
  public void resetZoom() {
    if (checkState("resetZoom")) {
      return;
    }
    nativeResetZoom();
  }

  @Override
  public void setMinZoom(double zoom) {
    if (checkState("setMinZoom")) {
      return;
    }
    nativeSetMinZoom(zoom);
  }

  @Override
  public double getMinZoom() {
    if (checkState("getMinZoom")) {
      return 0;
    }
    return nativeGetMinZoom();
  }

  @Override
  public void setMaxZoom(double zoom) {
    if (checkState("setMaxZoom")) {
      return;
    }
    nativeSetMaxZoom(zoom);
  }

  @Override
  public double getMaxZoom() {
    if (checkState("getMaxZoom")) {
      return 0;
    }
    return nativeGetMaxZoom();
  }

  @Override
  public void rotateBy(double sx, double sy, double ex, double ey) {
    if (checkState("rotateBy")) {
      return;
    }
    rotateBy(sx, sy, ex, ey, 0);
  }

  @Override
  public void rotateBy(double sx, double sy, double ex, double ey,
                       long duration) {
    if (checkState("rotateBy")) {
      return;
    }
    nativeRotateBy(sx / pixelRatio, sy / pixelRatio, ex, ey, duration);
  }

  @Override
  public void setContentPadding(float[] padding) {
    if (checkState("setContentPadding")) {
      return;
    }
    // TopLeftBottomRight
    nativeSetContentPadding(
      padding[1] / pixelRatio,
      padding[0] / pixelRatio,
      padding[3] / pixelRatio,
      padding[2] / pixelRatio);
  }

  @Override
  public float[] getContentPadding() {
    if (checkState("getContentPadding")) {
      return new float[] {0, 0, 0, 0};
    }
    float[] topLeftBottomRight = nativeGetContentPadding();
    return new float[]{
      topLeftBottomRight[1] * pixelRatio,
      topLeftBottomRight[0] * pixelRatio,
      topLeftBottomRight[3] * pixelRatio,
      topLeftBottomRight[2] * pixelRatio
    };
  }

  @Override
  public void setBearing(double degrees) {
    if (checkState("setBearing")) {
      return;
    }
    setBearing(degrees, 0);
  }

  @Override
  public void setBearing(double degrees, long duration) {
    if (checkState("setBearing")) {
      return;
    }
    nativeSetBearing(degrees, duration);
  }

  @Override
  public void setBearing(double degrees, double cx, double cy) {
    if (checkState("setBearing")) {
      return;
    }
    setBearing(degrees, cx, cy, 0);
  }

  @Override
  public void setBearing(double degrees, double fx, double fy, long duration) {
    if (checkState("setBearing")) {
      return;
    }
    nativeSetBearingXY(degrees, fx / pixelRatio, fy / pixelRatio, duration);
  }

  @Override
  public double getBearing() {
    if (checkState("getBearing")) {
      return 0;
    }
    return nativeGetBearing();
  }

  @Override
  public void resetNorth() {
    if (checkState("resetNorth")) {
      return;
    }
    nativeResetNorth();
  }

  @Override
  public long addMarker(Marker marker) {
    if (checkState("addMarker")) {
      return 0;
    }
    Marker[] markers = {marker};
    return nativeAddMarkers(markers)[0];
  }

  @Override
  @NonNull
  public long[] addMarkers(@NonNull List<Marker> markers) {
    if (checkState("addMarkers")) {
      return new long[] {};
    }
    return nativeAddMarkers(markers.toArray(new Marker[markers.size()]));
  }

  @Override
  public long addPolyline(Polyline polyline) {
    if (checkState("addPolyline")) {
      return 0;
    }
    Polyline[] polylines = {polyline};
    return nativeAddPolylines(polylines)[0];
  }

  @Override
  @NonNull
  public long[] addPolylines(@NonNull List<Polyline> polylines) {
    if (checkState("addPolylines")) {
      return new long[] {};
    }
    return nativeAddPolylines(polylines.toArray(new Polyline[polylines.size()]));
  }

  @Override
  public long addPolygon(Polygon polygon) {
    if (checkState("addPolygon")) {
      return 0;
    }
    Polygon[] polygons = {polygon};
    return nativeAddPolygons(polygons)[0];
  }

  @Override
  @NonNull
  public long[] addPolygons(@NonNull List<Polygon> polygons) {
    if (checkState("addPolygons")) {
      return new long[] {};
    }
    return nativeAddPolygons(polygons.toArray(new Polygon[polygons.size()]));
  }

  @Override
  public void updateMarker(@NonNull Marker marker) {
    if (checkState("updateMarker")) {
      return;
    }
    LatLng position = marker.getPosition();
    Icon icon = marker.getIcon();
    nativeUpdateMarker(marker.getId(), position.getLatitude(), position.getLongitude(), icon.getId());
  }

  @Override
  public void updatePolygon(@NonNull Polygon polygon) {
    if (checkState("updatePolygon")) {
      return;
    }
    nativeUpdatePolygon(polygon.getId(), polygon);
  }

  @Override
  public void updatePolyline(@NonNull Polyline polyline) {
    if (checkState("updatePolyline")) {
      return;
    }
    nativeUpdatePolyline(polyline.getId(), polyline);
  }

  @Override
  public void removeAnnotation(long id) {
    if (checkState("removeAnnotation")) {
      return;
    }
    long[] ids = {id};
    removeAnnotations(ids);
  }

  @Override
  public void removeAnnotations(long[] ids) {
    if (checkState("removeAnnotations")) {
      return;
    }
    nativeRemoveAnnotations(ids);
  }

  @Override
  @NonNull
  public long[] queryPointAnnotations(RectF rect) {
    if (checkState("queryPointAnnotations")) {
      return new long[] {};
    }
    return nativeQueryPointAnnotations(rect);
  }

  @Override
  @NonNull
  public long[] queryShapeAnnotations(RectF rectF) {
    if (checkState("queryShapeAnnotations")) {
      return new long[] {};
    }
    return nativeQueryShapeAnnotations(rectF);
  }

  @Override
  public void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels) {
    if (checkState("addAnnotationIcon")) {
      return;
    }
    nativeAddAnnotationIcon(symbol, width, height, scale, pixels);
  }

  @Override
  public void removeAnnotationIcon(String symbol) {
    if (checkState("removeAnnotationIcon")) {
      return;
    }
    nativeRemoveAnnotationIcon(symbol);
  }

  @Override
  public void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration) {
    if (checkState("setVisibleCoordinateBounds")) {
      return;
    }
    nativeSetVisibleCoordinateBounds(coordinates, padding, direction, duration);
  }

  @Override
  public void onLowMemory() {
    if (checkState("onLowMemory")) {
      return;
    }
    nativeOnLowMemory();
  }

  @Override
  public void setDebug(boolean debug) {
    if (checkState("setDebug")) {
      return;
    }
    nativeSetDebug(debug);
  }

  @Override
  public void cycleDebugOptions() {
    if (checkState("cycleDebugOptions")) {
      return;
    }
    nativeCycleDebugOptions();
  }

  @Override
  public boolean getDebug() {
    if (checkState("getDebug")) {
      return false;
    }
    return nativeGetDebug();
  }

  @Override
  public boolean isFullyLoaded() {
    if (checkState("isFullyLoaded")) {
      return false;
    }
    return nativeIsFullyLoaded();
  }

  @Override
  public void setReachability(boolean status) {
    if (checkState("setReachability")) {
      return;
    }
    nativeSetReachability(status);
  }

  @Override
  public double getMetersPerPixelAtLatitude(double lat) {
    if (checkState("getMetersPerPixelAtLatitude")) {
      return 0;
    }
    return nativeGetMetersPerPixelAtLatitude(lat, getZoom()) / pixelRatio;
  }

  @Override
  public ProjectedMeters projectedMetersForLatLng(@NonNull LatLng latLng) {
    if (checkState("projectedMetersForLatLng")) {
      return null;
    }
    return nativeProjectedMetersForLatLng(latLng.getLatitude(), latLng.getLongitude());
  }

  @Override
  public LatLng latLngForProjectedMeters(@NonNull ProjectedMeters projectedMeters) {
    if (checkState("latLngForProjectedMeters")) {
      return new LatLng();
    }
    return nativeLatLngForProjectedMeters(projectedMeters.getNorthing(),
      projectedMeters.getEasting());
  }

  @Override
  @NonNull
  public PointF pixelForLatLng(@NonNull LatLng latLng) {
    if (checkState("pixelForLatLng")) {
      return new PointF();
    }
    PointF pointF = nativePixelForLatLng(latLng.getLatitude(), latLng.getLongitude());
    pointF.set(pointF.x * pixelRatio, pointF.y * pixelRatio);
    return pointF;
  }

  @Override
  public LatLng latLngForPixel(@NonNull PointF pixel) {
    if (checkState("latLngForPixel")) {
      return new LatLng();
    }
    return nativeLatLngForPixel(pixel.x / pixelRatio, pixel.y / pixelRatio);
  }

  @Override
  public double getTopOffsetPixelsForAnnotationSymbol(String symbolName) {
    if (checkState("getTopOffsetPixelsForAnnotationSymbol")) {
      return 0;
    }
    return nativeGetTopOffsetPixelsForAnnotationSymbol(symbolName);
  }

  @Override
  public void jumpTo(double angle, @NonNull LatLng center, double pitch, double zoom) {
    if (checkState("jumpTo")) {
      return;
    }
    nativeJumpTo(angle, center.getLatitude(), center.getLongitude(), pitch, zoom);
  }

  @Override
  public void easeTo(double angle, @NonNull LatLng center, long duration, double pitch, double zoom,
                     boolean easingInterpolator) {
    if (checkState("easeTo")) {
      return;
    }
    nativeEaseTo(angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom,
      easingInterpolator);
  }

  @Override
  public void flyTo(double angle, @NonNull LatLng center, long duration, double pitch, double zoom) {
    if (checkState("flyTo")) {
      return;
    }
    nativeFlyTo(angle, center.getLatitude(), center.getLongitude(), duration, pitch, zoom);
  }

  @Override
  @NonNull
  public CameraPosition getCameraPosition() {
    if (checkState("getCameraValues")) {
      return new CameraPosition.Builder().build();
    }
    return nativeGetCameraPosition();
  }

  @Override
  public void setPrefetchesTiles(boolean enable) {
    if (checkState("setPrefetchesTiles")) {
      return;
    }
    nativeSetPrefetchesTiles(enable);
  }

  @Override
  public boolean getPrefetchesTiles() {
    if (checkState("getPrefetchesTiles")) {
      return false;
    }
    return nativeGetPrefetchesTiles();
  }

  // Runtime style Api

  @Override
  public long getTransitionDuration() {
    return nativeGetTransitionDuration();
  }

  @Override
  public void setTransitionDuration(long duration) {
    nativeSetTransitionDuration(duration);
  }

  @Override
  public long getTransitionDelay() {
    return nativeGetTransitionDelay();
  }

  @Override
  public void setTransitionDelay(long delay) {
    nativeSetTransitionDelay(delay);
  }

  @Override
  @NonNull
  public List<Layer> getLayers() {
    if (checkState("getLayers")) {
      return new ArrayList<>();
    }
    return Arrays.asList(nativeGetLayers());
  }

  @Override
  public Layer getLayer(String layerId) {
    if (checkState("getLayer")) {
      return null;
    }
    return nativeGetLayer(layerId);
  }

  @Override
  public void addLayer(@NonNull Layer layer) {
    if (checkState("addLayer")) {
      return;
    }
    nativeAddLayer(layer.getNativePtr(), null);
  }

  @Override
  public void addLayerBelow(@NonNull Layer layer, @NonNull String below) {
    if (checkState("addLayerBelow")) {
      return;
    }
    nativeAddLayer(layer.getNativePtr(), below);
  }

  @Override
  public void addLayerAbove(@NonNull Layer layer, @NonNull String above) {
    if (checkState("addLayerAbove")) {
      return;
    }
    nativeAddLayerAbove(layer.getNativePtr(), above);
  }

  @Override
  public void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index) {
    if (checkState("addLayerAt")) {
      return;
    }
    nativeAddLayerAt(layer.getNativePtr(), index);
  }

  @Override
  public boolean removeLayer(@NonNull String layerId) {
    if (checkState("removeLayer")) {
      return false;
    }

    Layer layer = getLayer(layerId);
    if (layer != null) {
      return removeLayer(layer);
    }
    return false;
  }


  @Override
  public boolean removeLayer(@NonNull Layer layer) {
    if (checkState("removeLayer")) {
      return false;
    }
    return nativeRemoveLayer(layer.getNativePtr());
  }

  @Override
  public boolean removeLayerAt(@IntRange(from = 0) int index) {
    if (checkState("removeLayerAt")) {
      return false;
    }
    return nativeRemoveLayerAt(index);
  }

  @Override
  @NonNull
  public List<Source> getSources() {
    if (checkState("getSources")) {
      return new ArrayList<>();
    }
    return Arrays.asList(nativeGetSources());
  }

  @Override
  public Source getSource(@NonNull String sourceId) {
    if (checkState("getSource")) {
      return null;
    }
    return nativeGetSource(sourceId);
  }

  @Override
  public void addSource(@NonNull Source source) {
    if (checkState("addSource")) {
      return;
    }
    nativeAddSource(source, source.getNativePtr());
  }

  @Override
  public boolean removeSource(@NonNull String sourceId) {
    if (checkState("removeSource")) {
      return false;
    }
    Source source = getSource(sourceId);
    if (source != null) {
      return removeSource(source);
    }
    return false;
  }

  @Override
  public boolean removeSource(@NonNull Source source) {
    if (checkState("removeSource")) {
      return false;
    }
    return nativeRemoveSource(source, source.getNativePtr());
  }

  @Override
  public void addImage(@NonNull String name, @NonNull Bitmap image, boolean sdf) {
    if (checkState("addImage")) {
      return;
    }

    // Determine pixel ratio, cast to float to avoid rounding, see mapbox-gl-native/issues/11809
    float pixelRatio = (float) image.getDensity() / DisplayMetrics.DENSITY_DEFAULT;
    nativeAddImage(name, image, pixelRatio, sdf);
  }

  @Override
  public void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap) {
    if (checkState("addImages")) {
      return;
    }
    this.addImages(bitmapHashMap, false);
  }

  @Override
  public void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap, boolean sdf) {
    if (checkState("addImages")) {
      return;
    }
    //noinspection unchecked
    new BitmapImageConversionTask(this, sdf).execute(bitmapHashMap);
  }

  @Override
  public void removeImage(String name) {
    if (checkState("removeImage")) {
      return;
    }
    nativeRemoveImage(name);
  }

  @Override
  public Bitmap getImage(String name) {
    if (checkState("getImage")) {
      return null;
    }
    return nativeGetImage(name);
  }

  // Feature querying

  @Override
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

  @Override
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

  @Override
  public void setApiBaseUrl(String baseUrl) {
    if (checkState("setApiBaseUrl")) {
      return;
    }
    fileSource.setApiBaseUrl(baseUrl);
  }

  @Override
  public Light getLight() {
    if (checkState("getLight")) {
      return null;
    }
    return nativeGetLight();
  }

  @Override
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
  private void onCameraWillChange(boolean animated) {
    if (stateCallback != null) {
      stateCallback.onCameraWillChange(animated);
    }
  }

  @Keep
  private void onCameraIsChanging() {
    if (stateCallback != null) {
      stateCallback.onCameraIsChanging();
    }
  }

  @Keep
  private void onCameraDidChange(boolean animated) {
    if (stateCallback != null) {
      stateCallback.onCameraDidChange(animated);
    }
  }

  @Keep
  private void onWillStartLoadingMap() {
    if (stateCallback != null) {
      stateCallback.onWillStartLoadingMap();
    }
  }

  @Keep
  private void onDidFinishLoadingMap() {
    if (stateCallback != null) {
      stateCallback.onDidFinishLoadingMap();
    }
  }

  @Keep
  private void onDidFailLoadingMap(String error) {
    if (stateCallback != null) {
      stateCallback.onDidFailLoadingMap(error);
    }
  }

  @Keep
  private void onWillStartRenderingFrame() {
    if (stateCallback != null) {
      stateCallback.onWillStartRenderingFrame();
    }
  }

  @Keep
  private void onDidFinishRenderingFrame(boolean fully) {
    if (stateCallback != null) {
      stateCallback.onDidFinishRenderingFrame(fully);
    }
  }

  @Keep
  private void onWillStartRenderingMap() {
    if (stateCallback != null) {
      stateCallback.onWillStartRenderingMap();
    }
  }

  @Keep
  private void onDidFinishRenderingMap(boolean fully) {
    if (stateCallback != null) {
      stateCallback.onDidFinishRenderingMap(fully);
    }
  }

  @Keep
  private void onDidBecomeIdle() {
    stateCallback.onDidBecomeIdle();
  }

  @Keep
  private void onDidFinishLoadingStyle() {
    if (stateCallback != null) {
      stateCallback.onDidFinishLoadingStyle();
    }
  }

  @Keep
  private void onSourceChanged(String sourceId) {
    if (stateCallback != null) {
      stateCallback.onSourceChanged(sourceId);
    }
  }

  @Keep
  protected void onSnapshotReady(@Nullable Bitmap mapContent) {
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
  private native void nativeInitialize(NativeMapView nativeMap,
                                       FileSource fileSource,
                                       MapRenderer mapRenderer,
                                       float pixelRatio,
                                       boolean crossSourceCollisions);

  @Keep
  private native void nativeDestroy();

  @Keep
  private native void nativeResizeView(int width, int height);

  @Keep
  private native void nativeSetStyleUrl(String url);

  @NonNull
  @Keep
  private native String nativeGetStyleUrl();

  @Keep
  private native void nativeSetStyleJson(String newStyleJson);

  @NonNull
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

  @NonNull
  @Keep
  private native LatLng nativeGetLatLng();

  @NonNull
  @Keep
  private native CameraPosition nativeGetCameraForLatLngBounds(
    LatLngBounds latLngBounds, double top, double left, double bottom, double right, double bearing, double tilt);

  @NonNull
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
  private native void nativeSetContentPadding(float top, float left, float bottom, float right);

  @Keep
  private native float[] nativeGetContentPadding();

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

  @NonNull
  @Keep
  private native long[] nativeAddMarkers(Marker[] markers);

  @NonNull
  @Keep
  private native long[] nativeAddPolylines(Polyline[] polylines);

  @NonNull
  @Keep
  private native long[] nativeAddPolygons(Polygon[] polygons);

  @Keep
  private native void nativeRemoveAnnotations(long[] id);

  @NonNull
  @Keep
  private native long[] nativeQueryPointAnnotations(RectF rect);

  @NonNull
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

  @NonNull
  @Keep
  private native ProjectedMeters nativeProjectedMetersForLatLng(double latitude, double longitude);

  @NonNull
  @Keep
  private native LatLng nativeLatLngForProjectedMeters(double northing, double easting);

  @NonNull
  @Keep
  private native PointF nativePixelForLatLng(double lat, double lon);

  @NonNull
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

  @NonNull
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

  @NonNull
  @Keep
  private native Layer[] nativeGetLayers();

  @NonNull
  @Keep
  private native Layer nativeGetLayer(String layerId);

  @Keep
  private native void nativeAddLayer(long layerPtr, String before) throws CannotAddLayerException;

  @Keep
  private native void nativeAddLayerAbove(long layerPtr, String above) throws CannotAddLayerException;

  @Keep
  private native void nativeAddLayerAt(long layerPtr, int index) throws CannotAddLayerException;

  @Keep
  private native boolean nativeRemoveLayer(long layerId);

  @Keep
  private native boolean nativeRemoveLayerAt(int index);

  @NonNull
  @Keep
  private native Source[] nativeGetSources();

  @NonNull
  @Keep
  private native Source nativeGetSource(String sourceId);

  @Keep
  private native void nativeAddSource(Source source, long sourcePtr) throws CannotAddSourceException;

  @Keep
  private native boolean nativeRemoveSource(Source source, long sourcePtr);

  @Keep
  private native void nativeAddImage(String name, Bitmap bitmap, float pixelRatio, boolean sdf);

  @Keep
  private native void nativeAddImages(Image[] images);

  @Keep
  private native void nativeRemoveImage(String name);

  @NonNull
  @Keep
  private native Bitmap nativeGetImage(String name);

  @Keep
  private native void nativeUpdatePolygon(long polygonId, Polygon polygon);

  @Keep
  private native void nativeUpdatePolyline(long polylineId, Polyline polyline);

  @Keep
  private native void nativeTakeSnapshot();

  @NonNull
  @Keep
  private native Feature[] nativeQueryRenderedFeaturesForPoint(float x, float y,
                                                               String[] layerIds,
                                                               Object[] filter);

  @NonNull
  @Keep
  private native Feature[] nativeQueryRenderedFeaturesForBox(float left, float top,
                                                             float right, float bottom,
                                                             String[] layerIds,
                                                             Object[] filter);

  @NonNull
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
  // Snapshot
  //

  void addSnapshotCallback(@NonNull MapboxMap.SnapshotReadyCallback callback) {
    if (checkState("addSnapshotCallback")) {
      return;
    }
    snapshotReadyCallback = callback;
    nativeTakeSnapshot();
  }

  @Override
  public void setOnFpsChangedListener(@NonNull final MapboxMap.OnFpsChangedListener listener) {
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

  boolean isDestroyed() {
    return destroyed;
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

    @NonNull
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
    protected void onPostExecute(@NonNull List<Image> images) {
      super.onPostExecute(images);
      if (nativeMapView != null && !nativeMapView.checkState("nativeAddImages")) {
        nativeMapView.nativeAddImages(images.toArray(new Image[images.size()]));
      }
    }
  }

  public interface ViewCallback {
    int getWidth();

    int getHeight();

    @Nullable
    Bitmap getViewContent();
  }

  interface StyleCallback {
    void onWillStartLoadingMap();

    void onDidFinishLoadingStyle();
  }

  interface StateCallback extends StyleCallback {
    void onCameraWillChange(boolean animated);

    void onCameraIsChanging();

    void onCameraDidChange(boolean animated);

    void onDidFinishLoadingMap();

    void onDidFailLoadingMap(String error);

    void onWillStartRenderingFrame();

    void onDidFinishRenderingFrame(boolean fully);

    void onWillStartRenderingMap();

    void onDidFinishRenderingMap(boolean fully);

    void onDidBecomeIdle();

    void onSourceChanged(String sourceId);
  }
}
