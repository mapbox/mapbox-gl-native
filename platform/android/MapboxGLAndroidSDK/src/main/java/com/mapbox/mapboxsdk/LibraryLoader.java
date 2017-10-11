package com.mapbox.mapboxsdk;

import timber.log.Timber;

/**
 * Centralises the knowledge about "mapbox-gl" library loading.
 */
public class LibraryLoader {

  /**
   * Loads "libmapbox-gl.so" native shared library.
   */
  public static void load() {
    try {
      System.loadLibrary("mapbox-gl");
    } catch (UnsatisfiedLinkError error) {
      Timber.e(error, "Failed to load native shared library.");
    }
  }
}
