package com.mapbox.mapboxsdk.maps.renderer;

/**
 * Created by ivo on 11/09/2017.
 */

public interface RenderThread {

  void requestRender();

  void queueEvent(Runnable runnable);
}
