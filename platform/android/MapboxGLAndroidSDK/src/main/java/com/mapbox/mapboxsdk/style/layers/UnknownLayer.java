package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.UiThread;

/**
 * An unknown type of layer
 */
@UiThread
public class UnknownLayer extends Layer {

  /**
   * Creates a UnknownLayer.
   *
   * @param nativePtr pointer used by core
   */
  UnknownLayer(long nativePtr) {
    super(nativePtr);
  }

  protected native void initialize();

  @Override
  protected native void finalize() throws Throwable;

}
