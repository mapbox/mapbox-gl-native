package com.mapbox.mapboxsdk.maps;


import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.annotations.Annotation;

import java.util.List;

/**
 * Interface that defines convenient methods for working with a {@link Annotation}'s collection.
 */
interface Annotations {
  Annotation obtainBy(long id);

  List<Annotation> obtainAll();

  void removeBy(long id);

  void removeBy(@NonNull Annotation annotation);

  void removeBy(@NonNull List<? extends Annotation> annotationList);

  void removeAll();
}
