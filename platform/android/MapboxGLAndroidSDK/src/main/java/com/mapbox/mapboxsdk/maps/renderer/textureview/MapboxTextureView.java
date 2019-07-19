package com.mapbox.mapboxsdk.maps.renderer.textureview;

import android.content.Context;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.TextureView;

/**
 * {@link android.view.TextureView} extension that notifies a listener when the view is attached to the window,
 * which is the point we can create the GL thread. Refs #13601
 */
public class MapboxTextureView extends TextureView {

  private OnViewAttachedListener listener;

  public MapboxTextureView(Context context) {
    super(context);
  }

  public MapboxTextureView(Context context, AttributeSet attrs) {
    super(context, attrs);
  }

  public void setOnViewAttachedListener(@Nullable OnViewAttachedListener listener) {
    this.listener = listener;
  }

  @Override
  protected void onAttachedToWindow() {
    super.onAttachedToWindow();
    if (listener != null) {
      listener.onViewAttached();
    }
  }

  public interface OnViewAttachedListener {
    void onViewAttached();
  }
}
