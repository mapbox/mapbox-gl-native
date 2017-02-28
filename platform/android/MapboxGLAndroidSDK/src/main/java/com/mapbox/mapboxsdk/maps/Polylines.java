package com.mapbox.mapboxsdk.maps;


import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;

import java.util.List;

/**
 * Interface that defines convenient methods for working with a {@link Polyline}'s collection.
 */
interface Polylines {
  Polyline addBy(@NonNull PolylineOptions polylineOptions, @NonNull MapboxMap mapboxMap);

  List<Polyline> addBy(@NonNull List<PolylineOptions> polylineOptionsList, @NonNull MapboxMap mapboxMap);

  void update(Polyline polyline);

  List<Polyline> obtainAll();
}
