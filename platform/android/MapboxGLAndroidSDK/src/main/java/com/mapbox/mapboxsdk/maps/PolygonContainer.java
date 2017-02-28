package com.mapbox.mapboxsdk.maps;


import android.support.annotation.NonNull;
import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;

import java.util.ArrayList;
import java.util.List;

import timber.log.Timber;

/**
 * Encapsulates {@link Polygon}'s functionality.
 */
class PolygonContainer implements Polygons {

  private final NativeMapView nativeMapView;
  private final LongSparseArray<Annotation> annotations;

  PolygonContainer(NativeMapView nativeMapView, LongSparseArray<Annotation> annotations) {
    this.nativeMapView = nativeMapView;
    this.annotations = annotations;
  }

  @Override
  public Polygon addBy(@NonNull PolygonOptions polygonOptions, @NonNull MapboxMap mapboxMap) {
    Polygon polygon = polygonOptions.getPolygon();
    if (!polygon.getPoints().isEmpty()) {
      long id = nativeMapView != null ? nativeMapView.addPolygon(polygon) : 0;
      polygon.setId(id);
      polygon.setMapboxMap(mapboxMap);
      annotations.put(id, polygon);
    }
    return polygon;
  }

  @Override
  public List<Polygon> addBy(@NonNull List<PolygonOptions> polygonOptionsList, @NonNull MapboxMap mapboxMap) {
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
    return polygons;
  }

  @Override
  public void update(Polygon polygon) {
    if (!isAddedToMap(polygon)) {
      Timber.w("Attempting to update non-added Polygon with value %s", polygon);
      return;
    }

    nativeMapView.updatePolygon(polygon);
    annotations.setValueAt(annotations.indexOfKey(polygon.getId()), polygon);
  }

  @Override
  public List<Polygon> obtainAll() {
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

  private boolean isAddedToMap(Annotation annotation) {
    return annotation != null && annotation.getId() != -1 && annotations.indexOfKey(annotation.getId()) != -1;
  }
}
