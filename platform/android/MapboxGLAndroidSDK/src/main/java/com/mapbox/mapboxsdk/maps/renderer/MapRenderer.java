package com.mapbox.mapboxsdk.maps.renderer;

import android.content.Context;
import androidx.annotation.CallSuper;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;

import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * The {@link MapRenderer} encapsulates the GL thread.
 * <p>
 * Performs actions on the GL thread to manage the GL resources and
 * render on the one end and acts as a scheduler to request work to
 * be performed on the GL thread on the other.
 */
@Keep
public abstract class MapRenderer implements MapRendererScheduler {

  static {
    LibraryLoader.load();
  }

  private static final String TAG = "Mbgl-MapRenderer";

  // Holds the pointer to the native peer after initialisation
  private long nativePtr = 0;
  private double expectedRenderTime = 0;
  private MapboxMap.OnFpsChangedListener onFpsChangedListener;

  public MapRenderer(@NonNull Context context, String localIdeographFontFamily) {
    float pixelRatio = context.getResources().getDisplayMetrics().density;

    // Initialise native peer
    nativeInitialize(this, pixelRatio, localIdeographFontFamily);
  }

  public void onStart() {
    // Implement if needed
  }

  public void onPause() {
    // Implement if needed
  }

  public void onResume() {
    // Implement if needed
  }

  public void onStop() {
    // Implement if needed
  }

  public void onDestroy() {
    // Implement if needed
  }

  public void setOnFpsChangedListener(MapboxMap.OnFpsChangedListener listener) {
    onFpsChangedListener = listener;
  }

  @CallSuper
  protected void onSurfaceCreated(GL10 gl, EGLConfig config) {
    nativeOnSurfaceCreated();
  }

  @CallSuper
  protected void onSurfaceChanged(@NonNull GL10 gl, int width, int height) {
    gl.glViewport(0, 0, width, height);
    nativeOnSurfaceChanged(width, height);
  }

  @CallSuper
  protected void onSurfaceDestroyed() {
    nativeOnSurfaceDestroyed();
  }

  @CallSuper
  protected void onDrawFrame(GL10 gl) {
    long startTime = System.nanoTime();
    try {
      nativeRender();
    } catch (java.lang.Error error) {
      Logger.e(TAG, error.getMessage());
    }
    long renderTime = System.nanoTime() - startTime;
    if (renderTime < expectedRenderTime) {
      try {
        Thread.sleep((long) ((expectedRenderTime - renderTime) / 1E6));
      } catch (InterruptedException ex) {
        Logger.e(TAG, ex.getMessage());
      }
    }
    if (onFpsChangedListener != null) {
      updateFps();
    }
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
  @CallSuper
  void queueEvent(MapRendererRunnable runnable) {
    this.queueEvent((Runnable) runnable);
  }

  private native void nativeInitialize(MapRenderer self,
                                       float pixelRatio,
                                       String localIdeographFontFamily);

  @CallSuper
  @Override
  protected native void finalize() throws Throwable;

  private native void nativeOnSurfaceCreated();

  private native void nativeOnSurfaceChanged(int width, int height);

  private native void nativeOnSurfaceDestroyed();

  protected native void nativeReset();

  private native void nativeRender();

  private long timeElapsed;

  private void updateFps() {
    long currentTime = System.nanoTime();
    double fps = 1E9 / ((currentTime - timeElapsed));
    onFpsChangedListener.onFpsChanged(fps);
    timeElapsed = currentTime;
  }

  /**
   * The max frame rate at which this render is rendered,
   * but it can't excess the ability of device hardware.
   *
   * @param maximumFps Can be set to arbitrary integer values.
   */
  public void setMaximumFps(int maximumFps) {
    if (maximumFps <= 0) {
      // Not valid, just return
      return;
    }
    expectedRenderTime = 1E9 / maximumFps;
  }
}
