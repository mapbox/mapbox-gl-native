package com.mapbox.mapboxsdk.maps;

import android.support.annotation.MainThread;
import android.support.annotation.NonNull;
import android.support.annotation.WorkerThread;

abstract class MapThreadExecutor implements ThreadExecutor {

  private final NativeMapView nativeMapView;
  private final ThreadExecutor threadExecutor;

  MapThreadExecutor(@NonNull NativeMapView nativeMapView, @NonNull ThreadExecutor threadExecutor) {
    this.nativeMapView = nativeMapView;
    this.threadExecutor = threadExecutor;
  }

  @MainThread
  public void queueRenderEvent(final MapRunnable mapRunnable) {
    threadExecutor.queueRenderEvent(mapRunnable);
  }

  @WorkerThread
  public void queueUiEvent(Runnable runnable) {
    threadExecutor.queueUiEvent(runnable);
  }

  @NonNull
  NativeMapView getNativeMapView() {
    return nativeMapView;
  }

}
