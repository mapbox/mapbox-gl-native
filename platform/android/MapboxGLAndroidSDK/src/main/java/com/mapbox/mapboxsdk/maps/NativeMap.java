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
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.util.HashMap;
import java.util.List;

interface NativeMap {
  void destroy();

  void update();

  void resizeView(int width, int height);

  void setStyleUrl(String url);

  @NonNull
  String getStyleUrl();

  void setStyleJson(String newStyleJson);

  @NonNull
  String getStyleJson();

  void setLatLngBounds(LatLngBounds latLngBounds);

  void cancelTransitions();

  void setGestureInProgress(boolean inProgress);

  void moveBy(double dx, double dy);

  void moveBy(double dx, double dy, long duration);

  void setLatLng(@NonNull LatLng latLng);

  void setLatLng(@NonNull LatLng latLng, long duration);

  LatLng getLatLng();

  CameraPosition getCameraForLatLngBounds(LatLngBounds bounds, int[] padding, double bearing, double tilt);

  CameraPosition getCameraForGeometry(Geometry geometry, int[] padding, double bearing, double tilt);

  void resetPosition();

  double getPitch();

  void setPitch(double pitch);

  void setPitch(double pitch, long duration);

  void setZoom(double zoom, @NonNull PointF focalPoint, long duration);

  double getZoom();

  void resetZoom();

  void setMinZoom(double zoom);

  double getMinZoom();

  void setMaxZoom(double zoom);

  double getMaxZoom();

  void rotateBy(double sx, double sy, double ex, double ey);

  void rotateBy(double sx, double sy, double ex, double ey,
                long duration);

  void setContentPadding(float[] padding);

  float[] getContentPadding();

  void setBearing(double degrees);

  void setBearing(double degrees, long duration);

  void setBearing(double degrees, double cx, double cy);

  void setBearing(double degrees, double fx, double fy, long duration);

  double getBearing();

  void resetNorth();

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

  @NonNull
  long[] queryPointAnnotations(RectF rect);

  @NonNull
  long[] queryShapeAnnotations(RectF rectF);

  void addAnnotationIcon(String symbol, int width, int height, float scale, byte[] pixels);

  void removeAnnotationIcon(String symbol);

  void setVisibleCoordinateBounds(LatLng[] coordinates, RectF padding, double direction, long duration);

  void onLowMemory();

  void setDebug(boolean debug);

  void cycleDebugOptions();

  boolean getDebug();

  boolean isFullyLoaded();

  void setReachability(boolean status);

  double getMetersPerPixelAtLatitude(double lat);

  ProjectedMeters projectedMetersForLatLng(@NonNull LatLng latLng);

  LatLng latLngForProjectedMeters(@NonNull ProjectedMeters projectedMeters);

  @NonNull
  PointF pixelForLatLng(@NonNull LatLng latLng);

  LatLng latLngForPixel(@NonNull PointF pixel);

  double getTopOffsetPixelsForAnnotationSymbol(String symbolName);

  void jumpTo(double angle, @NonNull LatLng center, double pitch, double zoom);

  void easeTo(double angle, @NonNull LatLng center, long duration, double pitch, double zoom,
              boolean easingInterpolator);

  void flyTo(double angle, @NonNull LatLng center, long duration, double pitch, double zoom);

  @NonNull
  CameraPosition getCameraPosition();

  void setPrefetchesTiles(boolean enable);

  boolean getPrefetchesTiles();

  long getTransitionDuration();

  void setTransitionDuration(long duration);

  long getTransitionDelay();

  void setTransitionDelay(long delay);

  @NonNull
  List<Layer> getLayers();

  Layer getLayer(String layerId);

  void addLayer(@NonNull Layer layer);

  void addLayerBelow(@NonNull Layer layer, @NonNull String below);

  void addLayerAbove(@NonNull Layer layer, @NonNull String above);

  void addLayerAt(@NonNull Layer layer, @IntRange(from = 0) int index);

  boolean removeLayer(@NonNull String layerId);

  boolean removeLayer(@NonNull Layer layer);

  boolean removeLayerAt(@IntRange(from = 0) int index);

  @NonNull
  List<Source> getSources();

  Source getSource(@NonNull String sourceId);

  void addSource(@NonNull Source source);

  boolean removeSource(@NonNull String sourceId);

  boolean removeSource(@NonNull Source source);

  void addImage(@NonNull String name, @NonNull Bitmap image, boolean sdf);

  void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap);

  void addImages(@NonNull HashMap<String, Bitmap> bitmapHashMap, boolean sdf);

  void removeImage(String name);

  Bitmap getImage(String name);

  @NonNull
  List<Feature> queryRenderedFeatures(@NonNull PointF coordinates,
                                      @Nullable String[] layerIds,
                                      @Nullable Expression filter);

  @NonNull
  List<Feature> queryRenderedFeatures(@NonNull RectF coordinates,
                                      @Nullable String[] layerIds,
                                      @Nullable Expression filter);

  void setApiBaseUrl(String baseUrl);

  Light getLight();

  float getPixelRatio();

  void setOnFpsChangedListener(@NonNull MapboxMap.OnFpsChangedListener listener);
}
