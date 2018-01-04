package com.mapbox.mapboxsdk.maps.renderer;

/**
 * Can be used to schedule work on the map renderer
 * thread or request a render.
 */
public interface MapRendererScheduler {

  void requestRender();

  void queueEvent(Runnable runnable);

}
