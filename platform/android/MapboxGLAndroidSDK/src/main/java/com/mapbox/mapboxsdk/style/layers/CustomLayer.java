package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.Keep;

/**
 * Custom layer.
 * <p>
 * Experimental feature. Do not use.
 * </p>
 */
public class CustomLayer extends Layer {

  public CustomLayer(String id,
                     long host) {
    initialize(id, host);
  }

  @Keep
  public CustomLayer(long nativePtr) {
    super(nativePtr);
  }

  public void update() {
    nativeUpdate();
  }

  @Keep
  protected native void initialize(String id, long host);

  @Keep
  protected native void nativeUpdate();

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
