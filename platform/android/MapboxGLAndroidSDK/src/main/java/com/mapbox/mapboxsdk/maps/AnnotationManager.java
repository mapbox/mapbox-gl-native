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
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.services.commons.geojson.Feature;

import java.util.ArrayList;
import java.util.List;

import timber.log.Timber;

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

  private static final String LAYER_ID_SHAPE_ANNOTATIONS = "com.mapbox.annotations.shape.";
  private static final long NO_ANNOTATION_ID = -1;

  private final NativeMapView nativeMapView;
  private final MapView mapView;
  private final IconManager iconManager;
  private final InfoWindowManager infoWindowManager = new InfoWindowManager();
  private final MarkerViewManager markerViewManager;
  private final LongSparseArray<Annotation> annotations = new LongSparseArray<>();
  private final List<Marker> selectedMarkers = new ArrayList<>();
  private final List<String> shapeAnnotationIds = new ArrayList<>();

  private MapboxMap mapboxMap;
  private MapboxMap.OnMarkerClickListener onMarkerClickListener;
  private MapboxMap.OnPolygonClickListener onPolygonClickListener;
  private MapboxMap.OnPolylineClickListener onPolylineClickListener;

  AnnotationManager(NativeMapView view, MapView mapView, MarkerViewManager markerViewManager) {
    this.nativeMapView = view;
    this.mapView = mapView;
    this.iconManager = new IconManager(nativeMapView);
    this.markerViewManager = markerViewManager;
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
    return annotations.get(id);
  }

  List<Annotation> getAnnotations() {
    List<Annotation> annotations = new ArrayList<>();
    for (int i = 0; i < this.annotations.size(); i++) {
      annotations.add(this.annotations.get(this.annotations.keyAt(i)));
    }
    return annotations;
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
      }
    } else {
      // instanceOf Polygon/Polyline
      shapeAnnotationIds.remove(annotation.getId());
    }
    long id = annotation.getId();
    if (nativeMapView != null) {
      nativeMapView.removeAnnotation(id);
    }
    annotations.remove(id);
  }

  void removeAnnotation(long id) {
    if (nativeMapView != null) {
      nativeMapView.removeAnnotation(id);
    }
    annotations.remove(id);
  }

  void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
    int count = annotationList.size();
    long[] ids = new long[count];
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotationList.get(i);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.hideInfoWindow();
        if (selectedMarkers.contains(marker)) {
          selectedMarkers.remove(marker);
        }

        if (marker instanceof MarkerView) {
          markerViewManager.removeMarkerView((MarkerView) marker);
        }
      } else {
        // instanceOf Polygon/Polyline
        shapeAnnotationIds.remove(annotation.getId());
      }
      ids[i] = annotationList.get(i).getId();
    }

    if (nativeMapView != null) {
      nativeMapView.removeAnnotations(ids);
    }

    for (long id : ids) {
      annotations.remove(id);
    }
  }

  void removeAnnotations() {
    Annotation annotation;
    int count = annotations.size();
    long[] ids = new long[count];
    selectedMarkers.clear();
    for (int i = 0; i < count; i++) {
      ids[i] = annotations.keyAt(i);
      annotation = annotations.get(ids[i]);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.hideInfoWindow();
        if (marker instanceof MarkerView) {
          markerViewManager.removeMarkerView((MarkerView) marker);
        }
      } else {
        // instanceOf Polygon/Polyline
        shapeAnnotationIds.remove(annotation.getId());
      }
    }

    if (nativeMapView != null) {
      nativeMapView.removeAnnotations(ids);
    }

    annotations.clear();
  }

  //
  // Markers
  //

  Marker addMarker(@NonNull BaseMarkerOptions markerOptions, @NonNull MapboxMap mapboxMap) {
    Marker marker = prepareMarker(markerOptions);
    long id = nativeMapView != null ? nativeMapView.addMarker(marker) : 0;
    marker.setMapboxMap(mapboxMap);
    marker.setId(id);
    annotations.put(id, marker);
    return marker;
  }

  List<Marker> addMarkers(@NonNull List<? extends BaseMarkerOptions> markerOptionsList, @NonNull MapboxMap mapboxMap) {
    int count = markerOptionsList.size();
    List<Marker> markers = new ArrayList<>(count);
    if (count > 0) {
      BaseMarkerOptions markerOptions;
      Marker marker;
      for (int i = 0; i < count; i++) {
        markerOptions = markerOptionsList.get(i);
        marker = prepareMarker(markerOptions);
        markers.add(marker);
      }

      if (markers.size() > 0) {
        long[] ids;
        if (nativeMapView != null) {
          ids = nativeMapView.addMarkers(markers);
        } else {
          ids = new long[markers.size()];
        }

        long id;
        Marker m;
        for (int i = 0; i < ids.length; i++) {
          m = markers.get(i);
          m.setMapboxMap(mapboxMap);
          id = ids[i];
          m.setId(id);
          annotations.put(id, m);
        }

      }
    }
    return markers;
  }

  private Marker prepareMarker(BaseMarkerOptions markerOptions) {
    Marker marker = markerOptions.getMarker();
    Icon icon = iconManager.loadIconForMarker(marker);
    marker.setTopOffsetPixels(iconManager.getTopOffsetPixelsForIcon(icon));
    return marker;
  }

  MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions, @NonNull MapboxMap mapboxMap,
                       @Nullable MarkerViewManager.OnMarkerViewAddedListener onMarkerViewAddedListener) {
    final MarkerView marker = prepareViewMarker(markerOptions);

    // add marker to map
    marker.setMapboxMap(mapboxMap);
    long id = nativeMapView.addMarker(marker);
    marker.setId(id);
    annotations.put(id, marker);

    if (onMarkerViewAddedListener != null) {
      markerViewManager.addOnMarkerViewAddedListener(marker, onMarkerViewAddedListener);
    }
    markerViewManager.setEnabled(true);
    markerViewManager.setWaitingForRenderInvoke(true);
    return marker;
  }

  List<MarkerView> addMarkerViews(@NonNull List<? extends BaseMarkerViewOptions> markerViewOptions,
                                  @NonNull MapboxMap mapboxMap) {
    List<MarkerView> markers = new ArrayList<>();
    for (BaseMarkerViewOptions markerViewOption : markerViewOptions) {
      // if last marker
      if (markerViewOptions.indexOf(markerViewOption) == markerViewOptions.size() - 1) {
        // get notified when render occurs to invalidate and draw MarkerViews
        markerViewManager.setWaitingForRenderInvoke(true);
      }
      // add marker to map
      MarkerView marker = prepareViewMarker(markerViewOption);
      marker.setMapboxMap(mapboxMap);
      long id = nativeMapView.addMarker(marker);
      marker.setId(id);
      annotations.put(id, marker);
      markers.add(marker);
    }
    markerViewManager.setEnabled(true);
    markerViewManager.update();
    return markers;
  }

  private MarkerView prepareViewMarker(BaseMarkerViewOptions markerViewOptions) {
    MarkerView marker = markerViewOptions.getMarker();
    iconManager.loadIconForMarkerView(marker);
    return marker;
  }

  void updateMarker(@NonNull Marker updatedMarker) {
    if (!isAddedToMap(updatedMarker)) {
      Timber.w("Attempting to update non-added Marker with value %s", updatedMarker);
      return;
    }
    ensureIconLoaded(updatedMarker);
    nativeMapView.updateMarker(updatedMarker);
    annotations.setValueAt(annotations.indexOfKey(updatedMarker.getId()), updatedMarker);
  }

  private boolean isAddedToMap(Annotation annotation) {
    return annotation != null && annotation.getId() != -1 && annotations.indexOfKey(annotation.getId()) > -1;
  }

  private void ensureIconLoaded(Marker marker) {
    if (!(marker instanceof MarkerView)) {
      iconManager.ensureIconLoaded(marker, mapboxMap);
    }
  }

  List<Marker> getMarkers() {
    List<Marker> markers = new ArrayList<>();
    Annotation annotation;
    for (int i = 0; i < annotations.size(); i++) {
      annotation = annotations.get(annotations.keyAt(i));
      if (annotation instanceof Marker) {
        markers.add((Marker) annotation);
      }
    }
    return markers;
  }

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
      if (marker.isInfoWindowShown()) {
        marker.hideInfoWindow();
      }

      if (marker instanceof MarkerView) {
        markerViewManager.deselect((MarkerView) marker, false);
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

  @NonNull
  List<Marker> getMarkersInRect(@NonNull RectF rectangle) {
    // convert Rectangle to be density dependent
    float pixelRatio = nativeMapView.getPixelRatio();
    RectF rect = new RectF(rectangle.left / pixelRatio,
      rectangle.top / pixelRatio,
      rectangle.right / pixelRatio,
      rectangle.bottom / pixelRatio);

    long[] ids = nativeMapView.queryPointAnnotations(rect);

    List<Long> idsList = new ArrayList<>(ids.length);
    for (long id : ids) {
      idsList.add(id);
    }

    List<Marker> annotations = new ArrayList<>(ids.length);
    List<Annotation> annotationList = getAnnotations();
    int count = annotationList.size();
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotationList.get(i);
      if (annotation instanceof com.mapbox.mapboxsdk.annotations.Marker && idsList.contains(annotation.getId())) {
        annotations.add((com.mapbox.mapboxsdk.annotations.Marker) annotation);
      }
    }

    return new ArrayList<>(annotations);
  }

  List<MarkerView> getMarkerViewsInRect(@NonNull RectF rectangle) {
    float pixelRatio = nativeMapView.getPixelRatio();
    RectF rect = new RectF(rectangle.left / pixelRatio,
      rectangle.top / pixelRatio,
      rectangle.right / pixelRatio,
      rectangle.bottom / pixelRatio);

    long[] ids = nativeMapView.queryPointAnnotations(rect);

    List<Long> idsList = new ArrayList<>(ids.length);
    for (long id : ids) {
      idsList.add(id);
    }

    List<MarkerView> annotations = new ArrayList<>(ids.length);
    List<Annotation> annotationList = getAnnotations();
    int count = annotationList.size();
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotationList.get(i);
      if (annotation instanceof MarkerView && idsList.contains(annotation.getId())) {
        annotations.add((MarkerView) annotation);
      }
    }

    return new ArrayList<>(annotations);
  }

  //
  // Polygons
  //

  Polygon addPolygon(@NonNull PolygonOptions polygonOptions, @NonNull MapboxMap mapboxMap) {
    Polygon polygon = polygonOptions.getPolygon();
    if (!polygon.getPoints().isEmpty()) {
      long id = nativeMapView != null ? nativeMapView.addPolygon(polygon) : 0;
      polygon.setId(id);
      polygon.setMapboxMap(mapboxMap);
      shapeAnnotationIds.add(LAYER_ID_SHAPE_ANNOTATIONS + id);
      annotations.put(id, polygon);
    }
    return polygon;
  }

  List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList, @NonNull MapboxMap mapboxMap) {
    int count = polygonOptionsList.size();

    Polygon polygon;
    List<Polygon> polygons = new ArrayList<>(count);
    if (count > 0) {
      for (PolygonOptions polygonOptions : polygonOptionsList) {
        polygon = polygonOptions.getPolygon();
        if (!polygon.getPoints().isEmpty()) {
          polygons.add(polygon);
        }
      }

      long[] ids;
      if (nativeMapView != null) {
        ids = nativeMapView.addPolygons(polygons);
      } else {
        ids = new long[polygons.size()];
      }

      long id;
      for (int i = 0; i < ids.length; i++) {
        polygon = polygons.get(i);
        polygon.setMapboxMap(mapboxMap);
        id = ids[i];
        polygon.setId(id);
        shapeAnnotationIds.add(LAYER_ID_SHAPE_ANNOTATIONS + id);
        annotations.put(id, polygon);
      }
    }
    return polygons;
  }

  void updatePolygon(@NonNull Polygon polygon) {
    if (!isAddedToMap(polygon)) {
      Timber.w("Attempting to update non-added Polygon with value %s", polygon);
      return;
    }

    nativeMapView.updatePolygon(polygon);
    annotations.setValueAt(annotations.indexOfKey(polygon.getId()), polygon);
  }

  List<Polygon> getPolygons() {
    List<Polygon> polygons = new ArrayList<>();
    Annotation annotation;
    for (int i = 0; i < annotations.size(); i++) {
      annotation = annotations.get(annotations.keyAt(i));
      if (annotation instanceof Polygon) {
        polygons.add((Polygon) annotation);
      }
    }
    return polygons;
  }

  //
  // Polylines
  //

  Polyline addPolyline(@NonNull PolylineOptions polylineOptions, @NonNull MapboxMap mapboxMap) {
    Polyline polyline = polylineOptions.getPolyline();
    if (!polyline.getPoints().isEmpty()) {
      long id = nativeMapView != null ? nativeMapView.addPolyline(polyline) : 0;
      polyline.setMapboxMap(mapboxMap);
      polyline.setId(id);
      shapeAnnotationIds.add(LAYER_ID_SHAPE_ANNOTATIONS + id);
      annotations.put(id, polyline);
    }
    return polyline;
  }

  List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList, @NonNull MapboxMap mapboxMap) {
    int count = polylineOptionsList.size();
    Polyline polyline;
    List<Polyline> polylines = new ArrayList<>(count);

    if (count > 0) {
      for (PolylineOptions options : polylineOptionsList) {
        polyline = options.getPolyline();
        if (!polyline.getPoints().isEmpty()) {
          polylines.add(polyline);
        }
      }

      long[] ids;
      if (nativeMapView != null) {
        ids = nativeMapView.addPolylines(polylines);
      } else {
        ids = new long[polylines.size()];
      }

      long id;
      Polyline p;
      for (int i = 0; i < ids.length; i++) {
        p = polylines.get(i);
        p.setMapboxMap(mapboxMap);
        id = ids[i];
        p.setId(id);
        shapeAnnotationIds.add(LAYER_ID_SHAPE_ANNOTATIONS + id);
        annotations.put(id, p);
      }
    }
    return polylines;
  }

  void updatePolyline(@NonNull Polyline polyline) {
    if (!isAddedToMap(polyline)) {
      Timber.w("Attempting to update non-added Polyline with value %s", polyline);
      return;
    }

    nativeMapView.updatePolyline(polyline);
    annotations.setValueAt(annotations.indexOfKey(polyline.getId()), polyline);
  }

  List<Polyline> getPolylines() {
    List<Polyline> polylines = new ArrayList<>();
    Annotation annotation;
    for (int i = 0; i < annotations.size(); i++) {
      annotation = annotations.get(annotations.keyAt(i));
      if (annotation instanceof Polyline) {
        polylines.add((Polyline) annotation);
      }
    }
    return polylines;
  }

  InfoWindowManager getInfoWindowManager() {
    return infoWindowManager;
  }

  MarkerViewManager getMarkerViewManager() {
    return markerViewManager;
  }

  void adjustTopOffsetPixels(MapboxMap mapboxMap) {
    int count = annotations.size();
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotations.get(i);
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

  void reloadMarkers() {
    iconManager.reloadIcons();
    int count = annotations.size();
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotations.get(i);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        nativeMapView.removeAnnotation(annotation.getId());
        long newId = nativeMapView.addMarker(marker);
        marker.setId(newId);
      }
    }
  }

  //
  // Click event
  //

  boolean onTap(PointF tapPoint) {
    if (!shapeAnnotationIds.isEmpty()) {
      ShapeAnnotationHit shapeAnnotationHit = getShapeAnnotationHitFromTap(tapPoint);
      long shapeAnnotationId = new ShapeAnnotationHitResolver(mapboxMap).execute(shapeAnnotationHit);
      if (shapeAnnotationId != NO_ANNOTATION_ID) {
        handleClickForShapeAnnotation(shapeAnnotationId);
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
    return new ShapeAnnotationHit(tapRect, shapeAnnotationIds.toArray(new String[shapeAnnotationIds.size()]));
  }

  private void handleClickForShapeAnnotation(long shapeAnnotationId) {
    Annotation annotation = getAnnotation(shapeAnnotationId);
    if (annotation instanceof Polygon && onPolygonClickListener != null) {
      onPolygonClickListener.onPolygonClick((Polygon) annotation);
    } else if (annotation instanceof Polyline && onPolylineClickListener != null) {
      onPolylineClickListener.onPolylineClick((Polyline) annotation);
    }
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

    private MapboxMap mapboxMap;

    ShapeAnnotationHitResolver(MapboxMap mapboxMap) {
      this.mapboxMap = mapboxMap;
    }

    public long execute(ShapeAnnotationHit shapeHit) {
      long foundAnnotationId = NO_ANNOTATION_ID;
      List<Feature> features = mapboxMap.queryRenderedFeatures(shapeHit.tapPoint, shapeHit.layerIds);
      if (!features.isEmpty()) {
        foundAnnotationId = getIdFromFeature(features.get(0));
      }
      return foundAnnotationId;
    }

    private long getIdFromFeature(Feature feature) {
      try {
        return Long.valueOf(feature.getId());
      } catch (NumberFormatException exception) {
        Timber.e(exception, "Couldn't parse feature id to a long, with id: %s", feature.getId());
        return NO_ANNOTATION_ID;
      }
    }
  }

  private static class MarkerHitResolver {

    private final MarkerViewManager markerViewManager;
    private final Projection projection;

    private View view;
    private Bitmap bitmap;
    private PointF markerLocation;

    private Rect hitRectView = new Rect();
    private RectF hitRectMarker = new RectF();
    private RectF highestSurfaceIntersection = new RectF();

    private long closestMarkerId = NO_ANNOTATION_ID;

    MarkerHitResolver(@NonNull MapboxMap mapboxMap) {
      this.markerViewManager = mapboxMap.getMarkerViewManager();
      this.projection = mapboxMap.getProjection();
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
      hitRectMarker.set(0, 0, bitmap.getWidth(), bitmap.getHeight());
      hitRectMarker.offsetTo(
        markerLocation.x - bitmap.getWidth() / 2,
        markerLocation.y - bitmap.getHeight() / 2
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
    private final String[] layerIds;

    ShapeAnnotationHit(RectF tapRect, String[] layerIds) {
      this.tapPoint = tapRect;
      this.layerIds = layerIds;
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
