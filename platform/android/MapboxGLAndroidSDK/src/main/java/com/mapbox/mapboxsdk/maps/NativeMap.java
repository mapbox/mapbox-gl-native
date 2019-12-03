package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Geometry;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.util.List;

interface NativeMap {

  //
  // Lifecycle API
  //

  void resizeView(int width, int height);

  void onLowMemory();

  void destroy();

  boolean isDestroyed();

  //
  // Camera API
  //

  void jumpTo(@NonNull LatLng center, double zoom, double pitch, double bearing, double[] padding);

  void easeTo(@NonNull LatLng center, double zoom, double bearing, double pitch, double[] padding, long duration,
              boolean easingInterpolator);

  void flyTo(@NonNull LatLng center, double zoom, double bearing, double pitch, double[] padding, long duration);

  void moveBy(double deltaX, double deltaY, long duration);

  @NonNull
  CameraPosition getCameraPosition();

  CameraPosition getCameraForLatLngBounds(@NonNull LatLngBounds bounds, int[] padding, double bearing, double pitch);

  CameraPosition getCameraForGeometry(@NonNull Geometry geometry, int[] padding, double bearing, double pitch);

  void resetPosition();

  void setLatLng(@NonNull LatLng latLng, long duration);

  LatLng getLatLng();

  void setLatLngBounds(@Nullable LatLngBounds latLngBounds);

  void setVisibleCoordinateBounds(@NonNull LatLng[] coordinates, @NonNull RectF padding,
                                  double direction, long duration);

  void setPitch(double pitch, long duration);

  double getPitch();

  void setZoom(double zoom, @NonNull PointF focalPoint, long duration);

  double getZoom();

  void setMinZoom(double zoom);

  double getMinZoom();

  void setMaxZoom(double zoom);

  double getMaxZoom();

  void resetZoom();

  void rotateBy(double sx, double sy, double ex, double ey, long duration);

  void setBearing(double degrees, long duration);

  void setBearing(double degrees, double fx, double fy, long duration);

  double getBearing();

  void resetNorth();

  void cancelTransitions();

  //
  // Style API
  //

  void setStyleUri(String url);

  @NonNull
  String getStyleUri();

  void setStyleJson(String newStyleJson);

  @NonNull
  String getStyleJson();

  boolean isFullyLoaded();

  void addLayer(@NonNull Layer layer);

  void addLayerBelow(@NonNull Layer layer, @NonNull String below);

  void addLayerAbove(@NonNull Layer layer, @NonNull String above);

  void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index);

  @NonNull
  List<Layer> getLayers();

  Layer getLayer(String layerId);

  boolean removeLayer(@NonNull String layerId);

  boolean removeLayer(@NonNull Layer layer);

  boolean removeLayerAt(@IntRange(from = 0) int index);

  void addSource(@NonNull Source source);

  @NonNull
  List<Source> getSources();

  Source getSource(@NonNull String sourceId);

  boolean removeSource(@NonNull String sourceId);

  boolean removeSource(@NonNull Source source);

  void setTransitionOptions(@NonNull TransitionOptions transitionOptions);

  @NonNull
  TransitionOptions getTransitionOptions();

  void addImages(Image[] images);

  Bitmap getImage(String name);

  void removeImage(String name);

  Light getLight();

  //
  // Content padding API
  //

  void setContentPadding(double[] padding);

  double[] getContentPadding();

  //
  // Query API
  //

  @NonNull
  List<Feature> queryRenderedFeatures(@NonNull PointF coordinates,
                                      @Nullable String[] layerIds,
                                      @Nullable Expression filter);

  @NonNull
  List<Feature> queryRenderedFeatures(@NonNull RectF coordinates,
                                      @Nullable String[] layerIds,
                                      @Nullable Expression filter);

  //
  // Projection API
  //

  double getMetersPerPixelAtLatitude(double lat);

  ProjectedMeters projectedMetersForLatLng(@NonNull LatLng latLng);

  LatLng latLngForProjectedMeters(@NonNull ProjectedMeters projectedMeters);

  @NonNull
  PointF pixelForLatLng(@NonNull LatLng latLng);

  LatLng latLngForPixel(@NonNull PointF pixel);

  //
  // Utils API
  //

  void setOnFpsChangedListener(@NonNull MapboxMap.OnFpsChangedListener listener);

  void setDebug(boolean debug);

  boolean getDebug();

  void cycleDebugOptions();

  void setReachability(boolean status);

  void setApiBaseUrl(String baseUrl);

  void setPrefetchTiles(boolean enable);

  boolean getPrefetchTiles();

  void setPrefetchZoomDelta(@IntRange(from = 0) int delta);

  @IntRange(from = 0)
  int getPrefetchZoomDelta();

  void setGestureInProgress(boolean inProgress);

  float getPixelRatio();

  //
  // Deprecated Annotations API
  //

  long addMarker(Marker marker);

  @NonNull
  long[] addMarkers(@NonNull List<Marker> markers);

  long addPolyline(Polyline polyline);

  @NonNull
  long[] addPolylines(@NonNull List<Polyline> polylines);

  long addPolygon(Polygon polygon);

  @NonNull
  long[] addPolygons(@NonNull List<Polygon> polygons);

  void updateMarker(@NonNull Marker marker);

  void updatePolygon(@NonNull Polygon polygon);

  void updatePolyline(@NonNull Polyline polyline);

  void removeAnnotation(long id);

  void removeAnnotations(long[] ids);

  double getTopOffsetPixelsForAnnotationSymbol(String symbolName);

  void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels);

  void removeAnnotationIcon(String symbol);

  @NonNull
  long[] queryPointAnnotations(RectF rectF);

  @NonNull
  long[] queryShapeAnnotations(RectF rectF);

  @NonNull
  RectF getDensityDependantRectangle(RectF rectangle);

  long getNativePtr();

  void addSnapshotCallback(@NonNull MapboxMap.SnapshotReadyCallback callback);
}