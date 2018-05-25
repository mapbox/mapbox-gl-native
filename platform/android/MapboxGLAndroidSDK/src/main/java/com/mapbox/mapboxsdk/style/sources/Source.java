package com.mapbox.mapboxsdk.style.sources;

import com.mapbox.mapboxsdk.utils.ThreadUtils;

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
    checkThread();
    this.nativePtr = nativePtr;
  }

  public Source() {
    checkThread();
  }

  /**
   * Validates if source interaction is happening on the UI thread
   */
  protected void checkThread() {
    ThreadUtils.checkThread("Source");
  }

  /**
   * Retrieve the source id
   *
   * @return the source id
   */
  public String getId() {
    checkThread();
    return nativeGetId();
  }

  /**
   * Retrieve the source attribution.
   * <p>
   * Will return an empty String if no attribution is available.
   * </p>
   *
   * @return the string representation of the attribution in html format
   */
  public String getAttribution() {
    checkThread();
    return nativeGetAttribution();
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

  protected native String nativeGetAttribution();
}
