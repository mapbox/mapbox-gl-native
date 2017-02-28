package com.mapbox.mapboxsdk;

/**
 * Centralises the knowledge about "mapbox-gl" library loading.
 */
public class LibraryLoader {

  /**
   * Loads "libmapbox-gl.so" native shared library.
   */
  public static void load() {
    System.loadLibrary("mapbox-gl");
  }

}
