package com.mapbox.mapboxsdk.maps;

import android.graphics.RectF;
import androidx.annotation.NonNull;
import androidx.collection.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;

import java.util.ArrayList;
import java.util.List;

class ShapeAnnotationContainer implements ShapeAnnotations {

  private final NativeMap nativeMapView;
  private final LongSparseArray<Annotation> annotations;

  ShapeAnnotationContainer(NativeMap nativeMapView, LongSparseArray<Annotation> annotations) {
    this.nativeMapView = nativeMapView;
    this.annotations = annotations;
  }

  @NonNull
  @Override
  public List<Annotation> obtainAllIn(@NonNull RectF rectangle) {
    RectF rect = nativeMapView.getDensityDependantRectangle(rectangle);
    long[] annotationIds = nativeMapView.queryShapeAnnotations(rect);
    return getAnnotationsFromIds(annotationIds);
  }

  @NonNull
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
