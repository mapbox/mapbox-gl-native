package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.graphics.RectF;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.LongSparseArray;

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

import java.util.ArrayList;
import java.util.Collections;
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

  private final NativeMapView nativeMapView;
  private final MapView mapView;
  private final IconManager iconManager;
  private final InfoWindowManager infoWindowManager = new InfoWindowManager();
  private final MarkerViewManager markerViewManager;
  private final LongSparseArray<Annotation> annotationsArray;
  private final List<Marker> selectedMarkers = new ArrayList<>();

  private MapboxMap mapboxMap;
  private MapboxMap.OnMarkerClickListener onMarkerClickListener;
  private Annotations annotations;
  private Markers markers;
  private Polygons polygons;
  private Polylines polylines;

  AnnotationManager(NativeMapView view, MapView mapView, LongSparseArray<Annotation> annotationsArray,
                    MarkerViewManager markerViewManager, IconManager iconManager, Annotations annotations,
                    Markers markers, Polygons polygons, Polylines polylines) {
    this.nativeMapView = view;
    this.mapView = mapView;
    this.annotationsArray = annotationsArray;
    this.markerViewManager = markerViewManager;
    this.iconManager = iconManager;
    this.annotations = annotations;
    this.markers = markers;
    this.polygons = polygons;
    this.polylines = polylines;
    if (view != null) {
      // null checking needed for unit tests
      nativeMapView.addOnMapChangedListener(markerViewManager);
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
    polylines.update(polyline);
  }

  List<Polyline> getPolylines() {
    return polylines.obtainAll();
  }

  // TODO Refactor from here still in progress

  void setOnMarkerClickListener(@Nullable MapboxMap.OnMarkerClickListener listener) {
    onMarkerClickListener = listener;
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

  //
  // Click event
  //

  boolean onTap(PointF tapPoint, float screenDensity) {
    float toleranceSides = 4 * screenDensity;
    float toleranceTopBottom = 10 * screenDensity;
    boolean handledDefaultClick = false;

    RectF tapRect = new RectF(tapPoint.x - iconManager.getAverageIconWidth() / 2 - toleranceSides,
      tapPoint.y - iconManager.getAverageIconHeight() / 2 - toleranceTopBottom,
      tapPoint.x + iconManager.getAverageIconWidth() / 2 + toleranceSides,
      tapPoint.y + iconManager.getAverageIconHeight() / 2 + toleranceTopBottom);

    List<Marker> nearbyMarkers = getMarkersInRect(tapRect);
    long newSelectedMarkerId = -1;

    // find a Marker that isn't selected yet
    if (nearbyMarkers.size() > 0) {
      Collections.sort(nearbyMarkers);
      for (Marker nearbyMarker : nearbyMarkers) {
        boolean found = false;
        for (Marker selectedMarker : selectedMarkers) {
          if (selectedMarker.equals(nearbyMarker)) {
            found = true;
          }
        }
        if (!found) {
          newSelectedMarkerId = nearbyMarker.getId();
          break;
        }
      }
    }

    // if unselected marker found
    if (newSelectedMarkerId >= 0) {
      List<Annotation> annotations = getAnnotations();
      int count = annotations.size();
      for (int i = 0; i < count; i++) {
        Annotation annotation = annotations.get(i);
        if (annotation instanceof Marker) {
          if (annotation.getId() == newSelectedMarkerId) {
            Marker marker = (Marker) annotation;

            if (marker instanceof MarkerView) {
              handledDefaultClick = markerViewManager.onClickMarkerView((MarkerView) marker);
            } else {
              if (onMarkerClickListener != null) {
                // end developer has provided a custom click listener
                handledDefaultClick = onMarkerClickListener.onMarkerClick(marker);
              }
            }

            if (!handledDefaultClick) {
              // only select marker if user didn't handle the click event themselves
              selectMarker(marker);
            }

            return true;
          }
        }
      }
    } else if (nearbyMarkers.size() > 0) {
      // we didn't find an unselected marker, check if we can close an already open markers
      for (Marker nearbyMarker : nearbyMarkers) {
        for (Marker selectedMarker : selectedMarkers) {
          if (nearbyMarker.equals(selectedMarker)) {
            if (nearbyMarker instanceof MarkerView) {
              handledDefaultClick = markerViewManager.onClickMarkerView((MarkerView) nearbyMarker);
            } else if (onMarkerClickListener != null) {
              handledDefaultClick = onMarkerClickListener.onMarkerClick(nearbyMarker);
            }

            if (!handledDefaultClick) {
              // only deselect marker if user didn't handle the click event themselves
              deselectMarker(nearbyMarker);
            }
            return true;
          }
        }
      }
    }
    return false;
  }
}
