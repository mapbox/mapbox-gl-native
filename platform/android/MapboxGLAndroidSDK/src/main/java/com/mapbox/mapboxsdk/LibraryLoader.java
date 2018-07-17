package com.mapbox.mapboxsdk;

import com.mapbox.mapboxsdk.log.Logger;

/**
 * Loads the mapbox-gl shared library
 * <p>
 * By default uses the {@link System#loadLibrary(String)},
 * use {@link #setLibraryLoader(LibraryLoader)} to provide an alternative library loading hook.
 * </p>
 */
public abstract class LibraryLoader {

  private static final String TAG = "Mbgl-LibraryLoader";

  private static final LibraryLoader DEFAULT = new LibraryLoader() {
    @Override
    public void load(String name) {
      System.loadLibrary(name);
    }
  };

  private static volatile LibraryLoader loader = DEFAULT;

  /**
   * Set the library loader that loads the shared library.
   *
   * @param libraryLoader the library loader
   */
  public static void setLibraryLoader(LibraryLoader libraryLoader) {
    loader = libraryLoader;
  }

  /**
   * Loads "libmapbox-gl.so" native shared library.
   * <p>
   * Catches UnsatisfiedLinkErrors and prints a warning to logcat.
   * </p>
   */
  public static void load() {
    try {
      loader.load("mapbox-gl");
    } catch (UnsatisfiedLinkError error) {
      Logger.e(TAG, "Failed to load native shared library.", error);
    }
  }

  public abstract void load(String name);
}

