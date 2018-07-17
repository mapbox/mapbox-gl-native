package com.mapbox.mapboxsdk.maps;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.LongSparseArray;
import android.view.View;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.log.Logger;

import java.util.ArrayList;
import java.util.List;

/**
 * Responsible for managing and tracking state of Annotations linked to Map. All events related to
 * annotations that occur on {@link MapboxMap} are forwarded to this class.
 * <p>
 * Responsible for referencing {@link InfoWindowManager} and {@link MarkerViewManager}.
 * </p>
 * <p>
 * Exposes convenience methods to add/remove/update all subtypes of annotations found in
 * com.mapbox.mapboxsdk.annotations.
 * </p>
 */
class AnnotationManager {

  private static final String TAG = "Mbgl-AnnotationManager";

  private static final long NO_ANNOTATION_ID = -1;

  private final MapView mapView;
  private final IconManager iconManager;
  private final InfoWindowManager infoWindowManager = new InfoWindowManager();
  private final MarkerViewManager markerViewManager;
  private final LongSparseArray<Annotation> annotationsArray;
  private final List<Marker> selectedMarkers = new ArrayList<>();

  private MapboxMap mapboxMap;
  private MapboxMap.OnMarkerClickListener onMarkerClickListener;
  private MapboxMap.OnPolygonClickListener onPolygonClickListener;
  private MapboxMap.OnPolylineClickListener onPolylineClickListener;

  private Annotations annotations;
  private ShapeAnnotations shapeAnnotations;
  private Markers markers;
  private Polygons polygons;
  private Polylines polylines;

  AnnotationManager(NativeMapView view, MapView mapView, LongSparseArray<Annotation> annotationsArray,
                    MarkerViewManager markerViewManager, IconManager iconManager, Annotations annotations,
                    Markers markers, Polygons polygons, Polylines polylines, ShapeAnnotations shapeAnnotations) {
    this.mapView = mapView;
    this.annotationsArray = annotationsArray;
    this.markerViewManager = markerViewManager;
    this.iconManager = iconManager;
    this.annotations = annotations;
    this.markers = markers;
    this.polygons = polygons;
    this.polylines = polylines;
    this.shapeAnnotations = shapeAnnotations;
    if (view != null) {
      // null checking needed for unit tests
      view.addOnMapChangedListener(markerViewManager);
    }
  }

  // TODO refactor MapboxMap out for Projection and Transform
  // Requires removing MapboxMap from Annotations by using Peer model from #6912
  AnnotationManager bind(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    this.markerViewManager.bind(mapboxMap);
    return this;
  }

  void update() {
    markerViewManager.update();
    infoWindowManager.update();
  }

  //
  // Annotations
  //

  Annotation getAnnotation(long id) {
    return annotations.obtainBy(id);
  }

  List<Annotation> getAnnotations() {
    return annotations.obtainAll();
  }

  void removeAnnotation(long id) {
    annotations.removeBy(id);
  }

  void removeAnnotation(@NonNull Annotation annotation) {
    if (annotation instanceof Marker) {
      Marker marker = (Marker) annotation;
      marker.hideInfoWindow();
      if (selectedMarkers.contains(marker)) {
        selectedMarkers.remove(marker);
      }

      if (marker instanceof MarkerView) {
        markerViewManager.removeMarkerView((MarkerView) marker);
      } else {
        // do icon cleanup
        iconManager.iconCleanup(marker.getIcon());
      }
    }
    annotations.removeBy(annotation);
  }

  void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
    for (Annotation annotation : annotationList) {
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.hideInfoWindow();
        if (selectedMarkers.contains(marker)) {
          selectedMarkers.remove(marker);
        }

        if (marker instanceof MarkerView) {
          markerViewManager.removeMarkerView((MarkerView) marker);
        } else {
          iconManager.iconCleanup(marker.getIcon());
        }
      }
    }
    annotations.removeBy(annotationList);
  }

  void removeAnnotations() {
    Annotation annotation;
    int count = annotationsArray.size();
    long[] ids = new long[count];
    selectedMarkers.clear();
    for (int i = 0; i < count; i++) {
      ids[i] = annotationsArray.keyAt(i);
      annotation = annotationsArray.get(ids[i]);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.hideInfoWindow();
        if (marker instanceof MarkerView) {
          markerViewManager.removeMarkerView((MarkerView) marker);
        } else {
          iconManager.iconCleanup(marker.getIcon());
        }
      }
    }
    annotations.removeAll();
  }

  //
  // Markers
  //

  Marker addMarker(@NonNull BaseMarkerOptions markerOptions, @NonNull MapboxMap mapboxMap) {
    return markers.addBy(markerOptions, mapboxMap);
  }

  List<Marker> addMarkers(@NonNull List<? extends BaseMarkerOptions> markerOptionsList, @NonNull MapboxMap mapboxMap) {
    return markers.addBy(markerOptionsList, mapboxMap);
  }

  void updateMarker(@NonNull Marker updatedMarker, @NonNull MapboxMap mapboxMap) {
    if (!isAddedToMap(updatedMarker)) {
      logNonAdded(updatedMarker);
      return;
    }
    markers.update(updatedMarker, mapboxMap);
  }

  List<Marker> getMarkers() {
    return markers.obtainAll();
  }

  @NonNull
  List<Marker> getMarkersInRect(@NonNull RectF rectangle) {
    return markers.obtainAllIn(rectangle);
  }

  MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions, @NonNull MapboxMap mapboxMap,
                       @Nullable MarkerViewManager.OnMarkerViewAddedListener onMarkerViewAddedListener) {
    return markers.addViewBy(markerOptions, mapboxMap, onMarkerViewAddedListener);
  }

  List<MarkerView> addMarkerViews(@NonNull List<? extends BaseMarkerViewOptions> markerViewOptions,
                                  @NonNull MapboxMap mapboxMap) {
    return markers.addViewsBy(markerViewOptions, mapboxMap);
  }

  List<MarkerView> getMarkerViewsInRect(@NonNull RectF rectangle) {
    return markers.obtainViewsIn(rectangle);
  }

  void reloadMarkers() {
    markers.reload();
  }

  //
  // Polygons
  //

  Polygon addPolygon(@NonNull PolygonOptions polygonOptions, @NonNull MapboxMap mapboxMap) {
    return polygons.addBy(polygonOptions, mapboxMap);
  }

  List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList, @NonNull MapboxMap mapboxMap) {
    return polygons.addBy(polygonOptionsList, mapboxMap);
  }

  void updatePolygon(Polygon polygon) {
    if (!isAddedToMap(polygon)) {
      logNonAdded(polygon);
      return;
    }
    polygons.update(polygon);
  }

  List<Polygon> getPolygons() {
    return polygons.obtainAll();
  }

  //
  // Polylines
  //

  Polyline addPolyline(@NonNull PolylineOptions polylineOptions, @NonNull MapboxMap mapboxMap) {
    return polylines.addBy(polylineOptions, mapboxMap);
  }

  List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList, @NonNull MapboxMap mapboxMap) {
    return polylines.addBy(polylineOptionsList, mapboxMap);
  }

  void updatePolyline(Polyline polyline) {
    if (!isAddedToMap(polyline)) {
      logNonAdded(polyline);
      return;
    }
    polylines.update(polyline);
  }

  List<Polyline> getPolylines() {
    return polylines.obtainAll();
  }

  // TODO Refactor from here still in progress
  void setOnMarkerClickListener(@Nullable MapboxMap.OnMarkerClickListener listener) {
    onMarkerClickListener = listener;
  }

  void setOnPolygonClickListener(@Nullable MapboxMap.OnPolygonClickListener listener) {
    onPolygonClickListener = listener;
  }

  void setOnPolylineClickListener(@Nullable MapboxMap.OnPolylineClickListener listener) {
    onPolylineClickListener = listener;
  }

  void selectMarker(@NonNull Marker marker) {
    if (selectedMarkers.contains(marker)) {
      return;
    }

    // Need to deselect any currently selected annotation first
    if (!infoWindowManager.isAllowConcurrentMultipleOpenInfoWindows()) {
      deselectMarkers();
    }

    if (marker instanceof MarkerView) {
      markerViewManager.select((MarkerView) marker, false);
      markerViewManager.ensureInfoWindowOffset((MarkerView) marker);
    }

    if (infoWindowManager.isInfoWindowValidForMarker(marker) || infoWindowManager.getInfoWindowAdapter() != null) {
      infoWindowManager.add(marker.showInfoWindow(mapboxMap, mapView));
    }

    // only add to selected markers if user didn't handle the click event themselves #3176
    selectedMarkers.add(marker);
  }

  void deselectMarkers() {
    if (selectedMarkers.isEmpty()) {
      return;
    }

    for (Marker marker : selectedMarkers) {
      if (marker != null) {
        if (marker.isInfoWindowShown()) {
          marker.hideInfoWindow();
        }

        if (marker instanceof MarkerView) {
          markerViewManager.deselect((MarkerView) marker, false);
        }
      }
    }

    // Removes all selected markers from the list
    selectedMarkers.clear();
  }

  void deselectMarker(@NonNull Marker marker) {
    if (!selectedMarkers.contains(marker)) {
      return;
    }

    if (marker.isInfoWindowShown()) {
      marker.hideInfoWindow();
    }

    if (marker instanceof MarkerView) {
      markerViewManager.deselect((MarkerView) marker, false);
    }

    selectedMarkers.remove(marker);
  }

  List<Marker> getSelectedMarkers() {
    return selectedMarkers;
  }

  InfoWindowManager getInfoWindowManager() {
    return infoWindowManager;
  }

  MarkerViewManager getMarkerViewManager() {
    return markerViewManager;
  }

  void adjustTopOffsetPixels(MapboxMap mapboxMap) {
    int count = annotationsArray.size();
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotationsArray.get(i);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.setTopOffsetPixels(
          iconManager.getTopOffsetPixelsForIcon(marker.getIcon()));
      }
    }

    for (Marker marker : selectedMarkers) {
      if (marker.isInfoWindowShown()) {
        marker.hideInfoWindow();
        marker.showInfoWindow(mapboxMap, mapView);
      }
    }
  }

  private boolean isAddedToMap(Annotation annotation) {
    return annotation != null && annotation.getId() != -1 && annotationsArray.indexOfKey(annotation.getId()) > -1;
  }

  private void logNonAdded(Annotation annotation) {
    Logger.w(TAG, String.format(
      "Attempting to update non-added %s with value %s", annotation.getClass().getCanonicalName(), annotation)
    );
  }

  //
  // Click event
  //

  boolean onTap(PointF tapPoint) {
    ShapeAnnotationHit shapeAnnotationHit = getShapeAnnotationHitFromTap(tapPoint);
    Annotation annotation = new ShapeAnnotationHitResolver(shapeAnnotations).execute(shapeAnnotationHit);
    if (annotation != null) {
      if (handleClickForShapeAnnotation(annotation)) {
        return true;
      }
    }

    MarkerHit markerHit = getMarkerHitFromTouchArea(tapPoint);
    long markerId = new MarkerHitResolver(mapboxMap).execute(markerHit);
    return markerId != NO_ANNOTATION_ID && isClickHandledForMarker(markerId);
  }

  private ShapeAnnotationHit getShapeAnnotationHitFromTap(PointF tapPoint) {
    float touchTargetSide = Mapbox.getApplicationContext().getResources().getDimension(R.dimen.mapbox_eight_dp);
    RectF tapRect = new RectF(
      tapPoint.x - touchTargetSide,
      tapPoint.y - touchTargetSide,
      tapPoint.x + touchTargetSide,
      tapPoint.y + touchTargetSide
    );
    return new ShapeAnnotationHit(tapRect);
  }

  private boolean handleClickForShapeAnnotation(Annotation annotation) {
    if (annotation instanceof Polygon && onPolygonClickListener != null) {
      onPolygonClickListener.onPolygonClick((Polygon) annotation);
      return true;
    } else if (annotation instanceof Polyline && onPolylineClickListener != null) {
      onPolylineClickListener.onPolylineClick((Polyline) annotation);
      return true;
    }
    return false;
  }

  private MarkerHit getMarkerHitFromTouchArea(PointF tapPoint) {
    int touchSurfaceWidth = (int) (iconManager.getHighestIconHeight() * 1.5);
    int touchSurfaceHeight = (int) (iconManager.getHighestIconWidth() * 1.5);
    final RectF tapRect = new RectF(tapPoint.x - touchSurfaceWidth,
      tapPoint.y - touchSurfaceHeight,
      tapPoint.x + touchSurfaceWidth,
      tapPoint.y + touchSurfaceHeight
    );
    return new MarkerHit(tapRect, getMarkersInRect(tapRect));
  }

  private boolean isClickHandledForMarker(long markerId) {
    boolean handledDefaultClick;
    Marker marker = (Marker) getAnnotation(markerId);
    if (marker instanceof MarkerView) {
      handledDefaultClick = markerViewManager.onClickMarkerView((MarkerView) marker);
    } else {
      handledDefaultClick = onClickMarker(marker);
    }

    if (!handledDefaultClick) {
      toggleMarkerSelectionState(marker);
    }
    return true;
  }

  private boolean onClickMarker(Marker marker) {
    return onMarkerClickListener != null && onMarkerClickListener.onMarkerClick(marker);
  }

  private void toggleMarkerSelectionState(Marker marker) {
    if (!selectedMarkers.contains(marker)) {
      selectMarker(marker);
    } else {
      deselectMarker(marker);
    }
  }

  private static class ShapeAnnotationHitResolver {

    private ShapeAnnotations shapeAnnotations;

    ShapeAnnotationHitResolver(ShapeAnnotations shapeAnnotations) {
      this.shapeAnnotations = shapeAnnotations;
    }

    public Annotation execute(ShapeAnnotationHit shapeHit) {
      Annotation foundAnnotation = null;
      List<Annotation> annotations = shapeAnnotations.obtainAllIn(shapeHit.tapPoint);
      if (annotations.size() > 0) {
        foundAnnotation = annotations.get(0);
      }
      return foundAnnotation;
    }
  }

  private static class MarkerHitResolver {

    private final MarkerViewManager markerViewManager;
    private final Projection projection;
    private final int minimalTouchSize;

    private View view;
    private Bitmap bitmap;
    private int bitmapWidth;
    private int bitmapHeight;
    private PointF markerLocation;

    private Rect hitRectView = new Rect();
    private RectF hitRectMarker = new RectF();
    private RectF highestSurfaceIntersection = new RectF();

    private long closestMarkerId = NO_ANNOTATION_ID;

    MarkerHitResolver(@NonNull MapboxMap mapboxMap) {
      this.markerViewManager = mapboxMap.getMarkerViewManager();
      this.projection = mapboxMap.getProjection();
      this.minimalTouchSize = (int) (32 * Mapbox.getApplicationContext().getResources().getDisplayMetrics().density);
    }

    public long execute(MarkerHit markerHit) {
      resolveForMarkers(markerHit);
      return closestMarkerId;
    }

    private void resolveForMarkers(MarkerHit markerHit) {
      for (Marker marker : markerHit.markers) {
        if (marker instanceof MarkerView) {
          resolveForMarkerView(markerHit, (MarkerView) marker);
        } else {
          resolveForMarker(markerHit, marker);
        }
      }
    }

    private void resolveForMarkerView(MarkerHit markerHit, MarkerView markerView) {
      view = markerViewManager.getView(markerView);
      if (view != null) {
        view.getHitRect(hitRectView);
        hitRectMarker = new RectF(hitRectView);
        hitTestMarker(markerHit, markerView, hitRectMarker);
      }
    }

    private void resolveForMarker(MarkerHit markerHit, Marker marker) {
      markerLocation = projection.toScreenLocation(marker.getPosition());
      bitmap = marker.getIcon().getBitmap();

      bitmapHeight = bitmap.getHeight();
      if (bitmapHeight < minimalTouchSize) {
        bitmapHeight = minimalTouchSize;
      }

      bitmapWidth = bitmap.getWidth();
      if (bitmapWidth < minimalTouchSize) {
        bitmapWidth = minimalTouchSize;
      }

      hitRectMarker.set(0, 0, bitmapWidth, bitmapHeight);
      hitRectMarker.offsetTo(
        markerLocation.x - bitmapWidth / 2,
        markerLocation.y - bitmapHeight / 2
      );
      hitTestMarker(markerHit, marker, hitRectMarker);
    }

    private void hitTestMarker(MarkerHit markerHit, Marker marker, RectF hitRectMarker) {
      if (hitRectMarker.contains(markerHit.getTapPointX(), markerHit.getTapPointY())) {
        hitRectMarker.intersect(markerHit.tapRect);
        if (isRectangleHighestSurfaceIntersection(hitRectMarker)) {
          highestSurfaceIntersection = new RectF(hitRectMarker);
          closestMarkerId = marker.getId();
        }
      }
    }

    private boolean isRectangleHighestSurfaceIntersection(RectF rectF) {
      return rectF.width() * rectF.height() > highestSurfaceIntersection.width() * highestSurfaceIntersection.height();
    }
  }

  private static class ShapeAnnotationHit {
    private final RectF tapPoint;

    ShapeAnnotationHit(RectF tapPoint) {
      this.tapPoint = tapPoint;
    }
  }

  private static class MarkerHit {
    private final RectF tapRect;
    private final List<Marker> markers;

    MarkerHit(RectF tapRect, List<Marker> markers) {
      this.tapRect = tapRect;
      this.markers = markers;
    }

    float getTapPointX() {
      return tapRect.centerX();
    }

    float getTapPointY() {
      return tapRect.centerY();
    }
  }
}
