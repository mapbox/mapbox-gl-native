package com.mapbox.mapboxsdk.maps.renderer;

import android.content.Context;
import android.opengl.GLSurfaceView;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.storage.FileSource;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * The {@link MapRenderer} encapsulates the GL thread.
 * <p>
 * Performs actions on the GL thread to manage the GL resources and
 * render on the one end and acts as a scheduler to request work to
 * be performed on the GL thread on the other.
 */
public class MapRenderer implements GLSurfaceView.Renderer, MapRendererScheduler {

  // Holds the pointer to the native peer after initialisation
  private long nativePtr = 0;

  private final GLSurfaceView glSurfaceView;

  private MapboxMap.OnFpsChangedListener onFpsChangedListener;

  public MapRenderer(Context context, GLSurfaceView glSurfaceView) {
    this.glSurfaceView = glSurfaceView;

    FileSource fileSource = FileSource.getInstance(context);
    float pixelRatio = context.getResources().getDisplayMetrics().density;
    String programCacheDir = context.getCacheDir().getAbsolutePath();

    // Initialise native peer
    nativeInitialize(this, fileSource, pixelRatio, programCacheDir);
  }

  public void setOnFpsChangedListener(MapboxMap.OnFpsChangedListener listener) {
    onFpsChangedListener = listener;
  }

  @Override
  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    nativeOnSurfaceCreated();
  }

  @Override
  public void onSurfaceChanged(GL10 gl, int width, int height) {
    if (width < 0) {
      throw new IllegalArgumentException("fbWidth cannot be negative.");
    }

    if (height < 0) {
      throw new IllegalArgumentException("fbHeight cannot be negative.");
    }

    if (width > 65535) {
      throw new IllegalArgumentException(
        "fbWidth cannot be greater than 65535.");
    }

    if (height > 65535) {
      throw new IllegalArgumentException(
        "fbHeight cannot be greater than 65535.");
    }

    gl.glViewport(0, 0, width, height);
    nativeOnSurfaceChanged(width, height);
  }

  @Override
  public void onDrawFrame(GL10 gl) {
    nativeRender();

    if (onFpsChangedListener != null) {
      updateFps();
    }
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

  /**
   * May be called from any thread.
   * <p>
   * Called from the native peer to schedule work on the GL
   * thread. Explicit override for easier to read jni code.
   *
   * @param runnable the runnable to execute
   * @see MapRendererRunnable
   */
  void queueEvent(MapRendererRunnable runnable) {
    this.queueEvent((Runnable) runnable);
  }

  private native void nativeInitialize(MapRenderer self,
                                       FileSource fileSource,
                                       float pixelRatio,
                                       String programCacheDir);

  @Override
  protected native void finalize() throws Throwable;

  private native void nativeOnSurfaceCreated();

  private native void nativeOnSurfaceChanged(int width, int height);

  private native void nativeRender();

  private long frames;
  private long timeElapsed;

  private void updateFps() {
    frames++;
    long currentTime = System.nanoTime();
    double fps = 0;
    if (currentTime - timeElapsed >= 1) {
      fps = frames / ((currentTime - timeElapsed) / 1E9);
      onFpsChangedListener.onFpsChanged(fps);
      timeElapsed = currentTime;
      frames = 0;
    }
  }

}
