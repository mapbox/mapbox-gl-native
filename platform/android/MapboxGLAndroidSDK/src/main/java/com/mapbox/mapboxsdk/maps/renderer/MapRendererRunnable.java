package com.mapbox.mapboxsdk.maps.renderer;

/**
 * Peer class for {@link Runnable}s to be scheduled on the {@link MapRenderer} thread.
 * The actual work is performed in the native peer.
 */
class MapRendererRunnable implements Runnable {

  // Holds the pointer to the native peer after initialisation
  private final long nativePtr;

  /**
   * Constructed from the native peer constructor
   *
   * @param nativePtr the native peer's memory address
   */
  MapRendererRunnable(long nativePtr) {
    this.nativePtr = nativePtr;
  }

  @Override
  public native void run();

  @Override
  protected native void finalize() throws Throwable;

  private native void nativeInitialize();

}
