package com.mapbox.mapboxsdk.net;

import com.mapbox.mapboxsdk.LibraryLoader;

/**
 * Updates the native library's connectivity state
 */
class NativeConnectivityListener implements ConnectivityListener {

  static {
    LibraryLoader.load();
  }

  private long nativePtr;
  private boolean invalidated;

  NativeConnectivityListener(long nativePtr) {
    this.nativePtr = nativePtr;
  }

  NativeConnectivityListener() {
    initialize();
  }

  @Override
  public void onNetworkStateChanged(boolean connected) {
    nativeOnConnectivityStateChanged(connected);
  }

  protected native void nativeOnConnectivityStateChanged(boolean connected);

  protected native void initialize();

  @Override
  protected native void finalize() throws Throwable;
}
