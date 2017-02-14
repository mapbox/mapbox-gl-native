package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.support.annotation.WorkerThread;

interface MapRunnable {
  @WorkerThread
  void execute(@NonNull NativeMapView nativeMapView);
}
