package com.mapbox.mapboxsdk.style.sources;

/**
 * Base Peer class for sources. see source.hpp for the other half of the peer.
 */
public abstract class Source {
  private long nativePtr;

  /**
   * Internal use
   *
   * @param nativePtr - pointer to native peer
   */
  public Source(long nativePtr) {
    this.nativePtr = nativePtr;
  }

  public Source() {
  }

  /**
   * Retrieve the source id
   *
   * @return the source id
   */
  public String getId() {
    return nativeGetId();
  }

  /**
   * Internal use
   *
   * @return the native peer pointer
   */
  public long getNativePtr() {
    return nativePtr;
  }

  protected native String nativeGetId();

}
