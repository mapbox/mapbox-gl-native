package com.mapbox.mapboxsdk.net;

/**
 * Updates the native library's connectivity state
 */
class NativeConnectivityListener implements ConnectivityListener {

  static {
    System.loadLibrary("mapbox-gl");
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
