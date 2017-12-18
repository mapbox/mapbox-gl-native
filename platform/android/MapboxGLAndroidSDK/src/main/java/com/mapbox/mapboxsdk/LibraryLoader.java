package com.mapbox.mapboxsdk;

import timber.log.Timber;

/**
 * Centralises the knowledge about "mapbox-gl" library loading.
 */
public abstract class LibraryLoader {

  private static final LibraryLoader DEFAULT = new LibraryLoader() {
    @Override
    public void load(String name) {
      try {
        System.loadLibrary(name);
      } catch (UnsatisfiedLinkError error) {
        Timber.e(error, "Failed to load native shared library.");
      }
    }
  };

  private static volatile LibraryLoader loader = DEFAULT;

  public static void setLibraryLoader(LibraryLoader libraryLoader) {
    loader = libraryLoader;
  }

  /**
   * Loads "libmapbox-gl.so" native shared library.
   */
  public static void load() {
    loader.load("mapbox-gl");
  }

  public abstract void load(String name);
}

