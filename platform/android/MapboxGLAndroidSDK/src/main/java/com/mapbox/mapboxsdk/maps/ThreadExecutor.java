package com.mapbox.mapboxsdk.maps;

import android.support.annotation.UiThread;
import android.support.annotation.WorkerThread;

interface ThreadExecutor {
  @UiThread
  void queueRenderEvent(MapRunnable runnable);

  @WorkerThread
  void queueUiEvent(Runnable runnable);
}
