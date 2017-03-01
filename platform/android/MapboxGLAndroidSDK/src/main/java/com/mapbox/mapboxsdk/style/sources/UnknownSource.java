package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.UiThread;

/**
 * An unknown type of source
 */
@UiThread
public class UnknownSource extends Source {

  /**
   * Creates a UnknownSource.
   *
   * @param nativePtr pointer used by core
   */
  UnknownSource(long nativePtr) {
    super(nativePtr);
  }

  protected native void initialize();

  @Override
  protected native void finalize() throws Throwable;

}
