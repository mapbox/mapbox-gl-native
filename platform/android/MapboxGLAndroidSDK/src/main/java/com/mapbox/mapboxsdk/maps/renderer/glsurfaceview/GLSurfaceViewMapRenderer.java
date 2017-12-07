package com.mapbox.mapboxsdk.maps.renderer.glsurfaceview;

import android.content.Context;
import android.opengl.GLSurfaceView;

import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;
import com.mapbox.mapboxsdk.maps.renderer.egl.EGLConfigChooser;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

/**
 * The {@link GLSurfaceViewMapRenderer} encapsulates the GL thread and
 * {@link GLSurfaceView} specifics to render the map.
 *
 * @see MapRenderer
 */
public class GLSurfaceViewMapRenderer extends MapRenderer implements GLSurfaceView.Renderer {

  private final GLSurfaceView glSurfaceView;

  public GLSurfaceViewMapRenderer(Context context, GLSurfaceView glSurfaceView, String localIdeographFontFamily) {
    super(context, localIdeographFontFamily);
    this.glSurfaceView = glSurfaceView;
    glSurfaceView.setEGLContextClientVersion(2);
    glSurfaceView.setEGLConfigChooser(new EGLConfigChooser());
    glSurfaceView.setRenderer(this);
    glSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);
  }

  @Override
  public void onStop() {
    glSurfaceView.onPause();
  }

  @Override
  public void onStart() {
    glSurfaceView.onResume();
  }

  @Override
  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    super.onSurfaceCreated(gl, config);
  }

  @Override
  public void onSurfaceChanged(GL10 gl, int width, int height) {
    super.onSurfaceChanged(gl, width, height);
  }

  @Override
  public void onDrawFrame(GL10 gl) {
    super.onDrawFrame(gl);
  }

  /**
   * May be called from any thread.
   * <p>
   * Called from the renderer frontend to schedule a render.
   */
  @Override
  public void requestRender() {
    glSurfaceView.requestRender();
  }

  /**
   * May be called from any thread.
   * <p>
   * Schedules work to be performed on the MapRenderer thread.
   *
   * @param runnable the runnable to execute
   */
  @Override
  public void queueEvent(Runnable runnable) {
    glSurfaceView.queueEvent(runnable);
  }


}
