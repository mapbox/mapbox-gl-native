package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.graphics.RectF;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.annotation.WorkerThread;
import android.support.v4.util.LongSparseArray;

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
import com.mapbox.mapboxsdk.geometry.LatLng;

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
class AnnotationManager extends MapThreadExecutor {

  private final MapView mapView;
  private final IconManager iconManager;
  private final InfoWindowManager infoWindowManager = new InfoWindowManager();
  private final MarkerViewManager markerViewManager;

  // FIXME: 08/02/2017 threadsafe collection
  private final LongSparseArray<Annotation> annotations = new LongSparseArray<>();
  private final List<Marker> selectedMarkers = new ArrayList<>();

  private MapboxMap mapboxMap;
  private MapboxMap.OnMarkerClickListener onMarkerClickListener;

  AnnotationManager(NativeMapView view, ThreadExecutor threadExecutor, MapView mapView,
                    MarkerViewManager markerViewManager) {
    super(view, threadExecutor);
    this.mapView = mapView;
    this.iconManager = new IconManager(getNativeMapView());
    this.markerViewManager = markerViewManager;
    if (view != null) {
      // null checking needed for unit tests
      getNativeMapView().addOnMapChangedListener(markerViewManager);
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

  @UiThread
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

  @UiThread
  void removeAnnotation(@NonNull Annotation annotation) {
    if (annotation instanceof Marker) {
      Marker marker = (Marker) annotation;
      marker.hideInfoWindow();
      if (marker instanceof MarkerView) {
        markerViewManager.removeMarkerView((MarkerView) marker);
      }
    }
    final long id = annotation.getId();
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.removeAnnotation(id);
      }
    });
    annotations.remove(id);
  }

  @UiThread
  void removeAnnotation(long id) {
    removeAnnotation(annotations.get(id));
  }

  @UiThread
  void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
    int count = annotationList.size();
    final long[] ids = new long[count];
    for (int i = 0; i < count; i++) {
      Annotation annotation = annotationList.get(i);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.hideInfoWindow();
        if (marker instanceof MarkerView) {
          markerViewManager.removeMarkerView((MarkerView) marker);
        }
      }
      ids[i] = annotationList.get(i).getId();
    }

    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.removeAnnotations(ids);
      }
    });

    for (long id : ids) {
      annotations.remove(id);
    }
  }

  void removeAnnotations() {
    Annotation annotation;
    int count = annotations.size();
    final long[] ids = new long[count];
    for (int i = 0; i < count; i++) {
      ids[i] = annotations.keyAt(i);
      annotation = annotations.get(ids[i]);
      if (annotation instanceof Marker) {
        Marker marker = (Marker) annotation;
        marker.hideInfoWindow();
        if (marker instanceof MarkerView) {
          markerViewManager.removeMarkerView((MarkerView) marker);
        }
      }
    }

    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.removeAnnotations(ids);
      }
    });

    annotations.clear();
  }

  //
  // Markers
  //

  void addMarker(@NonNull final BaseMarkerOptions markerOptions, @NonNull final MapboxMap mapboxMap,
                 final Callback<Marker> onMarkerResult) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final Marker marker = prepareMarker(markerOptions);
        long id = nativeMapView != null ? nativeMapView.addMarker(marker) : 0;
        marker.setMapboxMap(mapboxMap);
        marker.setId(id);
        annotations.put(id, marker);

        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            if (onMarkerResult != null) {
              onMarkerResult.onResult(marker);
            }
          }
        });
      }
    });
  }

  void addMarkers(@NonNull final List<? extends BaseMarkerOptions> markerOptionsList,
                  @NonNull final MapboxMap mapboxMap, final Callback<List<Marker>> onMarkersAddedResult) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        int count = markerOptionsList.size();
        final List<Marker> markers = new ArrayList<>(count);
        if (count > 0) {
          BaseMarkerOptions markerOptions;
          Marker marker;
          for (int i = 0; i < count; i++) {
            markerOptions = markerOptionsList.get(i);
            marker = prepareMarker(markerOptions);
            markers.add(marker);
          }

          if (markers.size() > 0) {
            long[] ids = null;
            if (nativeMapView != null) {
              ids = nativeMapView.addMarkers(markers);
            }

            long id = 0;
            Marker m;
            for (int i = 0; i < markers.size(); i++) {
              m = markers.get(i);
              m.setMapboxMap(mapboxMap);
              if (ids != null) {
                id = ids[i];
              } else {
                // unit test
                id++;
              }
              m.setId(id);
              annotations.put(id, m);
            }
          }
        }
        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            onMarkersAddedResult.onResult(markers);
          }
        });
      }
    });
  }

  @WorkerThread
  private Marker prepareMarker(BaseMarkerOptions markerOptions) {
    Marker marker = markerOptions.getMarker();
    Icon icon = iconManager.loadIconForMarker(marker);
    marker.setTopOffsetPixels(iconManager.getTopOffsetPixelsForIcon(icon));
    return marker;
  }

  @UiThread
  void addMarker(@NonNull final BaseMarkerViewOptions markerOptions, @NonNull final MapboxMap mapboxMap,
                 final Callback<MarkerView> callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final MarkerView marker = prepareViewMarker(markerOptions);

        // add marker to map
        marker.setMapboxMap(mapboxMap);
        long id = nativeMapView.addMarker(marker);
        marker.setId(id);
        annotations.put(id, marker);

        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            if (callback != null) {
              markerViewManager.addOnMarkerViewAddedListener(marker, callback);
            }
            markerViewManager.setEnabled(true);
            markerViewManager.setWaitingForRenderInvoke(true);
          }
        });
      }
    });
  }


  @UiThread
  void addMarkerViews(@NonNull final List<? extends BaseMarkerViewOptions> markerViewOptions,
                      @NonNull final MapboxMap mapboxMap, final Callback<List<MarkerView>> callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
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

        if (callback != null) {
          callback.onResult(markers);
        }
      }
    });
  }

  @WorkerThread
  private MarkerView prepareViewMarker(BaseMarkerViewOptions markerViewOptions) {
    MarkerView marker = markerViewOptions.getMarker();
    iconManager.loadIconForMarkerView(marker);
    return marker;
  }

  @UiThread
  void updateMarker(@NonNull final Marker updatedMarker, @NonNull final MapboxMap mapboxMap) {
    if (updatedMarker == null) {
      return;
    }

    if (updatedMarker.getId() == -1) {
      return;
    }

    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        if (!(updatedMarker instanceof MarkerView)) {
          iconManager.ensureIconLoaded(updatedMarker, mapboxMap);
        }

        nativeMapView.updateMarker(updatedMarker);

        int index = annotations.indexOfKey(updatedMarker.getId());
        if (index > -1) {
          annotations.setValueAt(index, updatedMarker);
        }
      }
    });
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

  @UiThread
  void setOnMarkerClickListener(@Nullable MapboxMap.OnMarkerClickListener listener) {
    onMarkerClickListener = listener;
  }

  @UiThread
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

  @UiThread
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

  @UiThread
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

  void getMarkersInRect(@NonNull final RectF rectangle, final Callback<List<Marker>> callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        // convert Rectangle to be density depedent
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

        final List<Marker> annotations = new ArrayList<>(ids.length);
        List<Annotation> annotationList = getAnnotations();
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
          Annotation annotation = annotationList.get(i);
          if (annotation instanceof Marker && idsList.contains(annotation.getId())) {
            annotations.add((Marker) annotation);
          }
        }

        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            if (callback != null) {
              callback.onResult(new ArrayList<>(annotations));
            }
          }
        });
      }
    });
  }

  void getMarkerViewsInRect(@NonNull final RectF rectangle, final Callback<List<MarkerView>> callback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
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

        final List<MarkerView> annotations = new ArrayList<>(ids.length);
        List<Annotation> annotationList = getAnnotations();
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
          Annotation annotation = annotationList.get(i);
          if (annotation instanceof MarkerView && idsList.contains(annotation.getId())) {
            annotations.add((MarkerView) annotation);
          }
        }

        if (callback != null) {
          queueUiEvent(new Runnable() {
            @Override
            public void run() {
              callback.onResult(annotations);
            }
          });
        }
      }
    });
  }

  //
  // Polygons
  //

  void addPolygon(@NonNull final PolygonOptions polygonOptions, @NonNull final MapboxMap mapboxMap,
                  final Callback<Polygon> listener) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final Polygon polygon = polygonOptions.getPolygon();
        if (!polygon.getPoints().isEmpty()) {
          long id = nativeMapView.addPolygon(polygon);
          polygon.setId(id);
          polygon.setMapboxMap(mapboxMap);
          annotations.put(id, polygon);
          queueUiEvent(new Runnable() {
            @Override
            public void run() {
              listener.onResult(polygon);
            }
          });
        }
      }
    });
  }

  void addPolygons(@NonNull final List<PolygonOptions> polygonOptionsList, @NonNull final MapboxMap mapboxMap,
                   final Callback<List<Polygon>> listCallback) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        int count = polygonOptionsList.size();

        Polygon polygon;
        final List<Polygon> polygons = new ArrayList<>(count);
        if (count > 0) {
          for (PolygonOptions polygonOptions : polygonOptionsList) {
            polygon = polygonOptions.getPolygon();
            if (!polygon.getPoints().isEmpty()) {
              polygons.add(polygon);
            }
          }

          long[] ids = null;
          if (nativeMapView != null) {
            ids = nativeMapView.addPolygons(polygons);
          }

          long id = 0;
          for (int i = 0; i < polygons.size(); i++) {
            polygon = polygons.get(i);
            polygon.setMapboxMap(mapboxMap);
            if (ids != null) {
              id = ids[i];
            } else {
              // unit test
              id++;
            }
            polygon.setId(id);
            annotations.put(id, polygon);
          }
        }

        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            if (listCallback != null) {
              listCallback.onResult(polygons);
            }
          }
        });
      }
    });
  }

  void updatePolygon(final Polygon polygon) {
    if (polygon == null) {
      return;
    }

    if (polygon.getId() == -1) {
      return;
    }

    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.updatePolygon(polygon);
        int index = annotations.indexOfKey(polygon.getId());
        if (index > -1) {
          annotations.setValueAt(index, polygon);
        }
      }
    });
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

  void addPolyline(@NonNull final PolylineOptions polylineOptions, @NonNull final MapboxMap mapboxMap,
                   final Callback<Polyline> listener) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final Polyline polyline = polylineOptions.getPolyline();
        if (!polyline.getPoints().isEmpty()) {
          long id = nativeMapView != null ? nativeMapView.addPolyline(polyline) : 0;
          polyline.setMapboxMap(mapboxMap);
          polyline.setId(id);
          annotations.put(id, polyline);
          queueUiEvent(new Runnable() {
            @Override
            public void run() {
              if (listener != null) {
                listener.onResult(polyline);
              }
            }
          });
        }
      }
    });
  }

  void addPolylines(@NonNull final List<PolylineOptions> polylineOptionsList, @NonNull final MapboxMap mapboxMap,
                    final Callback<List<Polyline>> listener) {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        int count = polylineOptionsList.size();
        Polyline polyline;
        final List<Polyline> polylines = new ArrayList<>(count);

        if (count > 0) {
          for (PolylineOptions options : polylineOptionsList) {
            polyline = options.getPolyline();
            if (!polyline.getPoints().isEmpty()) {
              polylines.add(polyline);
            }
          }

          long[] ids = nativeMapView.addPolylines(polylines);
          long id = 0;
          Polyline p;

          for (int i = 0; i < polylines.size(); i++) {
            p = polylines.get(i);
            p.setMapboxMap(mapboxMap);
            if (ids != null) {
              id = ids[i];
            } else {
              // unit test
              id++;
            }
            p.setId(id);
            annotations.put(id, p);
          }
        }
        queueUiEvent(new Runnable() {
          @Override
          public void run() {
            listener.onResult(polylines);
          }
        });
      }
    });
  }

  void updatePolyline(final Polyline polyline) {
    if (polyline == null) {
      return;
    }

    if (polyline.getId() == -1) {
      return;
    }

    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.updatePolyline(polyline);

        int index = annotations.indexOfKey(polyline.getId());
        if (index > -1) {
          annotations.setValueAt(index, polyline);
        }
      }
    });
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
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
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
    });
  }

  //
  // Click event
  //

  // FIXME: 09/02/2017 cleanup
  void onTap(final PointF tapPoint, final UiSettings uiSettings, final Projection projection,
             final MapboxMap.OnMapClickListener onMapClickListener) {
    float screenDensity = uiSettings.getPixelRatio();
    float toleranceSides = 4 * screenDensity;
    float toleranceTopBottom = 10 * screenDensity;

    RectF tapRect = new RectF(tapPoint.x - iconManager.getAverageIconWidth() / 2 - toleranceSides,
      tapPoint.y - iconManager.getAverageIconHeight() / 2 - toleranceTopBottom,
      tapPoint.x + iconManager.getAverageIconWidth() / 2 + toleranceSides,
      tapPoint.y + iconManager.getAverageIconHeight() / 2 + toleranceTopBottom);

    getMarkersInRect(tapRect, new Callback<List<Marker>>() {
      @Override
      public void onResult(List<Marker> nearbyMarkers) {
        boolean tapHandled = false;

        long newSelectedMarkerId = -1;
        boolean handledDefaultClick = false;

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

                if (annotation instanceof MarkerView) {
                  markerViewManager.onClickMarkerView((MarkerView) annotation);
                } else {
                  if (!handledDefaultClick) {
                    // only select marker if user didn't handle the click event themselves
                    selectMarker(marker);
                  }
                }
                tapHandled = true;
              }
            }
          }
        } else if (nearbyMarkers.size() > 0) {
          // we didn't find an unselected marker, check if we can close an already open markers
          for (Marker nearbyMarker : nearbyMarkers) {
            for (Marker selectedMarker : selectedMarkers) {
              if (nearbyMarker.equals(selectedMarker)) {
                if (onMarkerClickListener != null) {
                  // end developer has provided a custom click listener
                  handledDefaultClick = onMarkerClickListener.onMarkerClick(nearbyMarker);
                  if (!handledDefaultClick) {
                    deselectMarker(nearbyMarker);
                  }
                }
                tapHandled = true;
              }
            }
          }
        }

        if (!tapHandled) {
          if (uiSettings.isDeselectMarkersOnTap()) {
            // deselect any selected marker
            deselectMarkers();
          }

          // notify app of map click
          if (onMapClickListener != null) {
            projection.fromScreenLocation(tapPoint, new Callback<LatLng>() {
              @Override
              public void onResult(LatLng latLng) {
                onMapClickListener.onMapClick(latLng);
              }
            });
          }
        }
      }
    });
  }
}
