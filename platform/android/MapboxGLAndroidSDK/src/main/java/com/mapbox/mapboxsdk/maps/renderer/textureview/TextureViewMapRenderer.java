package com.mapbox.mapboxsdk.maps.renderer.textureview;

import android.content.Context;
import android.support.annotation.NonNull;
import android.view.TextureView;

import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * The {@link TextureViewMapRenderer} encapsulates the GL thread and
 * {@link TextureView} specifics to render the map.
 *
 * @see MapRenderer
 */
public class TextureViewMapRenderer extends MapRenderer {
  private TextureViewRenderThread renderThread;

  /**
   * Create a {@link MapRenderer} for the given {@link TextureView}
   *
   * @param context     the current Context
   * @param textureView the TextureView
   */
  public TextureViewMapRenderer(@NonNull Context context,
                                @NonNull TextureView textureView,
                                String localIdeographFontFamily) {
    super(context, localIdeographFontFamily);
    renderThread = new TextureViewRenderThread(textureView, this);
    renderThread.start();
  }

  /**
   * Overridden to provide package access
   */
  @Override
  protected void onSurfaceCreated(GL10 gl, EGLConfig config) {
    super.onSurfaceCreated(gl, config);
  }

  /**
   * Overridden to provide package access
   */
  @Override
  protected void onSurfaceChanged(GL10 gl, int width, int height) {
    super.onSurfaceChanged(gl, width, height);
  }

  /**
   * Overridden to provide package access
   */
  @Override
  protected void onDrawFrame(GL10 gl) {
    super.onDrawFrame(gl);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void requestRender() {
    renderThread.requestRender();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void queueEvent(Runnable runnable) {
    renderThread.queueEvent(runnable);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void onStop() {
    renderThread.onPause();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void onStart() {
    renderThread.onResume();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void onDestroy() {
    renderThread.onDestroy();
  }
}
