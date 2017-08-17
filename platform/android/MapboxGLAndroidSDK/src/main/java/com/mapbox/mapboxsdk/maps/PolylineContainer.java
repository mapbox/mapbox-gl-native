package com.mapbox.mapboxsdk.maps;


import android.support.annotation.NonNull;
import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;

import java.util.ArrayList;
import java.util.List;

/**
 * Encapsulates {@link Polyline}'s functionality.
 */
class PolylineContainer implements Polylines {

  private final NativeMapView nativeMapView;
  private final LongSparseArray<Annotation> annotations;

  PolylineContainer(NativeMapView nativeMapView, LongSparseArray<Annotation> annotations) {
    this.nativeMapView = nativeMapView;
    this.annotations = annotations;
  }

  @Override
  public Polyline addBy(@NonNull PolylineOptions polylineOptions, @NonNull MapboxMap mapboxMap) {
    Polyline polyline = polylineOptions.getPolyline();
    if (!polyline.getPoints().isEmpty()) {
      long id = nativeMapView != null ? nativeMapView.addPolyline(polyline) : 0;
      polyline.setMapboxMap(mapboxMap);
      polyline.setId(id);
      annotations.put(id, polyline);
    }
    return polyline;
  }

  @Override
  public List<Polyline> addBy(@NonNull List<PolylineOptions> polylineOptionsList, @NonNull MapboxMap mapboxMap) {
    int count = polylineOptionsList.size();
    Polyline polyline;
    List<Polyline> polylines = new ArrayList<>(count);
    if (nativeMapView != null && count > 0) {
      for (PolylineOptions options : polylineOptionsList) {
        polyline = options.getPolyline();
        if (!polyline.getPoints().isEmpty()) {
          polylines.add(polyline);
        }
      }

      long[] ids = nativeMapView.addPolylines(polylines);
      for (int i = 0; i < ids.length; i++) {
        Polyline polylineCreated = polylines.get(i);
        polylineCreated.setMapboxMap(mapboxMap);
        polylineCreated.setId(ids[i]);
        annotations.put(ids[i], polylineCreated);
      }
    }
    return polylines;
  }

  @Override
  public void update(Polyline polyline) {
    nativeMapView.updatePolyline(polyline);
    annotations.setValueAt(annotations.indexOfKey(polyline.getId()), polyline);
  }

  @Override
  public List<Polyline> obtainAll() {
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
}
