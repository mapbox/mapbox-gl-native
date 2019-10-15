package com.mapbox.mapboxsdk.maps.renderer.glsurfaceview;

import android.content.Context;
import android.opengl.GLSurfaceView;
import androidx.annotation.NonNull;

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

  @NonNull
  private final MapboxGLSurfaceView glSurfaceView;

  public GLSurfaceViewMapRenderer(Context context,
                                  MapboxGLSurfaceView glSurfaceView,
                                  String localIdeographFontFamily) {
    super(context, localIdeographFontFamily);
    this.glSurfaceView = glSurfaceView;
    glSurfaceView.setEGLContextClientVersion(2);
    glSurfaceView.setEGLConfigChooser(new EGLConfigChooser());
    glSurfaceView.setRenderer(this);
    glSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);
    glSurfaceView.setPreserveEGLContextOnPause(true);
    glSurfaceView.setDetachedListener(new MapboxGLSurfaceView.OnGLSurfaceViewDetachedListener() {
      @Override
      public void onGLSurfaceViewDetached() {
        // because the GL thread is destroyed when the view is detached from window,
        // we need to ensure releasing the native renderer as well.
        // This avoids releasing it only when the view is being recreated, which is already on a new GL thread,
        // and leads to JNI crashes like https://github.com/mapbox/mapbox-gl-native/issues/14618
        nativeReset();
      }
    });
  }

  @Override
  public void onStop() {
    glSurfaceView.onPause();
  }

  @Override
  public void onPause() {
    super.onPause();
  }

  @Override
  public void onDestroy() {
    super.onDestroy();
  }

  @Override
  public void onStart() {
    glSurfaceView.onResume();
  }

  @Override
  public void onResume() {
    super.onResume();
  }

  @Override
  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    super.onSurfaceCreated(gl, config);
  }

  @Override
  protected void onSurfaceDestroyed() {
    super.onSurfaceDestroyed();
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