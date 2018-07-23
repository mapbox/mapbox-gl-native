package com.mapbox.mapboxsdk.maps.renderer;

import android.support.annotation.Keep;

/**
 * Can be used to schedule work on the map renderer
 * thread or request a render.
 */
public interface MapRendererScheduler {

  @Keep
  void requestRender();

  @Keep
  void queueEvent(Runnable runnable);

}
