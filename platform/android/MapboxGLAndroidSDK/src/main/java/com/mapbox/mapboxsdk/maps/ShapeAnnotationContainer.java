package com.mapbox.mapboxsdk.maps;

import android.graphics.RectF;
import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;

import java.util.ArrayList;
import java.util.List;

class ShapeAnnotationContainer implements ShapeAnnotations {

  private final NativeMapView nativeMapView;
  private final LongSparseArray<Annotation> annotations;

  ShapeAnnotationContainer(NativeMapView nativeMapView, LongSparseArray<Annotation> annotations) {
    this.nativeMapView = nativeMapView;
    this.annotations = annotations;
  }

  @Override
  public List<Annotation> obtainAllIn(RectF rectangle) {
    RectF rect = nativeMapView.getDensityDependantRectangle(rectangle);
    long[] annotationIds = nativeMapView.queryShapeAnnotations(rect);
    return getAnnotationsFromIds(annotationIds);
  }

  private List<Annotation> getAnnotationsFromIds(long[] annotationIds) {
    List<Annotation> shapeAnnotations = new ArrayList<>();
    for (long annotationId : annotationIds) {
      Annotation annotation = annotations.get(annotationId);
      if (annotation != null) {
        shapeAnnotations.add(annotation);
      }
    }
    return shapeAnnotations;
  }
}
