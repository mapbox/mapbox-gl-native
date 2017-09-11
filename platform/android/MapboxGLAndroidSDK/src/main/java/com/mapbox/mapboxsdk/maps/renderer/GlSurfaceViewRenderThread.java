package com.mapbox.mapboxsdk.maps.renderer;

import android.opengl.GLSurfaceView;

/**
 * {@link RenderThread} implementation that schedules using the
 * {@link GLSurfaceView} thread.
 */
public class GlSurfaceViewRenderThread implements RenderThread {
  private final GLSurfaceView surfaceView;

  public GlSurfaceViewRenderThread(GLSurfaceView surfaceView) {
    this.surfaceView = surfaceView;
  }

  @Override
  public void requestRender() {
    surfaceView.requestRender();
  }

  @Override
  public void queueEvent(Runnable runnable) {
    surfaceView.queueEvent(runnable);
  }
}
