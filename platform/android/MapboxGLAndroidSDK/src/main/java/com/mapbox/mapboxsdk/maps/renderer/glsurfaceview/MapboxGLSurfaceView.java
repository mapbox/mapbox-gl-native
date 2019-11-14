package com.mapbox.mapboxsdk.maps.renderer.glsurfaceview;

import android.content.Context;
import android.opengl.GLSurfaceView;
import androidx.annotation.NonNull;
import android.util.AttributeSet;

/**
 * {@link GLSurfaceView} extension that notifies a listener when the view is detached from window,
 * which is the point of destruction of the GL thread.
 */
public class MapboxGLSurfaceView extends GLSurfaceView {

  private OnGLSurfaceViewDetachedListener detachedListener;

  public MapboxGLSurfaceView(Context context) {
    super(context);
  }

  public MapboxGLSurfaceView(Context context, AttributeSet attrs) {
    super(context, attrs);
  }

  @Override
  protected void onDetachedFromWindow() {
    if (detachedListener != null) {
      detachedListener.onGLSurfaceViewDetached();
    }
    super.onDetachedFromWindow();
  }

  /**
   * Set a listener that gets notified when the view is detached from window.
   *
   * @param detachedListener listener
   */
  public void setDetachedListener(@NonNull OnGLSurfaceViewDetachedListener detachedListener) {
    if (this.detachedListener != null) {
      throw new IllegalArgumentException("Detached from window listener has been already set.");
    }
    this.detachedListener = detachedListener;
  }

  /**
   * Listener interface that notifies when a {@link MapboxGLSurfaceView} is detached from window.
   */
  public interface OnGLSurfaceViewDetachedListener {

    /**
     * Called when a {@link MapboxGLSurfaceView} is detached from window.
     */
    void onGLSurfaceViewDetached();
  }
}
