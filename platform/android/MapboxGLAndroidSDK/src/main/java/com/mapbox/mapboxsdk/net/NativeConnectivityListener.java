package com.mapbox.mapboxsdk.net;

import android.support.annotation.Keep;

import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.log.Logger;

/**
 * Updates the native library's connectivity state
 */
class NativeConnectivityListener implements ConnectivityListener {

  private static final String TAG = "NativeConnectivity";

  static {
    LibraryLoader libraryLoader = Mapbox.getLibraryLoader();
    if (libraryLoader != null) {
      libraryLoader.load();
    }
  }

  @Keep
  private long nativePtr;
  @Keep
  private boolean invalidated;

  @Keep
  NativeConnectivityListener(long nativePtr) {
    this.nativePtr = nativePtr;
  }

  NativeConnectivityListener() {
    try {
      initialize();
    } catch (UnsatisfiedLinkError error) {
      Logger.e(TAG, error.getMessage());
      MapStrictMode.strictModeViolation(error);
    }
  }

  @Override
  public void onNetworkStateChanged(boolean connected) {
    nativeOnConnectivityStateChanged(connected);
  }

  @Keep
  protected native void nativeOnConnectivityStateChanged(boolean connected);

  @Keep
  protected native void initialize();

  @Override
  @Keep
  protected native void finalize() throws Throwable;
}
