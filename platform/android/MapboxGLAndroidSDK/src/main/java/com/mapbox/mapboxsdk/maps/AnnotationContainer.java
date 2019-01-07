package com.mapbox.mapboxsdk.maps;


import android.support.annotation.NonNull;
import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;

import java.util.ArrayList;
import java.util.List;

/**
 * Encapsulates {@link Annotation}'s functionality..
 */
class AnnotationContainer implements Annotations {

  private final NativeMap nativeMap;
  private final LongSparseArray<Annotation> annotations;

  AnnotationContainer(NativeMap nativeMap, LongSparseArray<Annotation> annotations) {
    this.nativeMap = nativeMap;
    this.annotations = annotations;
  }

  @Override
  public Annotation obtainBy(long id) {
    return annotations.get(id);
  }

  @NonNull
  @Override
  public List<Annotation> obtainAll() {
    List<Annotation> annotations = new ArrayList<>();
    for (int i = 0; i < this.annotations.size(); i++) {
      annotations.add(this.annotations.get(this.annotations.keyAt(i)));
    }
    return annotations;
  }

  @Override
  public void removeBy(long id) {
    if (nativeMap != null) {
      nativeMap.removeAnnotation(id);
    }
    annotations.remove(id);
  }

  @Override
  public void removeBy(@NonNull Annotation annotation) {
    long id = annotation.getId();
    removeBy(id);
  }

  @Override
  public void removeBy(@NonNull List<? extends Annotation> annotationList) {
    int count = annotationList.size();
    long[] ids = new long[count];
    for (int i = 0; i < count; i++) {
      ids[i] = annotationList.get(i).getId();
    }

    removeNativeAnnotations(ids);

    for (long id : ids) {
      annotations.remove(id);
    }
  }

  @Override
  public void removeAll() {
    int count = annotations.size();
    long[] ids = new long[count];
    for (int i = 0; i < count; i++) {
      ids[i] = annotations.keyAt(i);
    }

    removeNativeAnnotations(ids);

    annotations.clear();
  }

  private void removeNativeAnnotations(long[] ids) {
    if (nativeMap != null) {
      nativeMap.removeAnnotations(ids);
    }
  }
}