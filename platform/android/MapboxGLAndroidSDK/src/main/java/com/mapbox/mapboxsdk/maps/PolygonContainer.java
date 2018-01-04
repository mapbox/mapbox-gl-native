package com.mapbox.mapboxsdk.maps;


import android.support.annotation.NonNull;
import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;

import java.util.ArrayList;
import java.util.List;

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
    if (nativeMapView != null && count > 0) {
      for (PolygonOptions polygonOptions : polygonOptionsList) {
        polygon = polygonOptions.getPolygon();
        if (!polygon.getPoints().isEmpty()) {
          polygons.add(polygon);
        }
      }

      long[] ids = nativeMapView.addPolygons(polygons);
      for (int i = 0; i < ids.length; i++) {
        polygon = polygons.get(i);
        polygon.setMapboxMap(mapboxMap);
        polygon.setId(ids[i]);
        annotations.put(ids[i], polygon);
      }
    }
    return polygons;
  }

  @Override
  public void update(Polygon polygon) {
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
}
