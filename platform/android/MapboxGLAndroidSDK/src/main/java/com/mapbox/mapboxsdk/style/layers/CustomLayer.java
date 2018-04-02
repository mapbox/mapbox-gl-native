package com.mapbox.mapboxsdk.style.layers;

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

  public CustomLayer(long nativePtr) {
    super(nativePtr);
  }

  public void update() {
    nativeUpdate();
  }

  protected native void initialize(String id, long host);

  protected native void nativeUpdate();

  @Override
  protected native void finalize() throws Throwable;

}
