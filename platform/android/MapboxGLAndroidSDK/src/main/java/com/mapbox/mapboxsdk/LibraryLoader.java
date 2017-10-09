package com.mapbox.mapboxsdk;

import android.content.Context;

import java.io.File;

import timber.log.Timber;

/**
 * Centralises the knowledge about "mapbox-gl" library loading.
 */
public class LibraryLoader {

  private static final String LIBRARY_NAME = "libmapbox-gl.so";

  /**
   * Loads "libmapbox-gl.so" native shared library.
   */
  public static void load() {
    try {
      System.loadLibrary("mapbox-gl");
    } catch (UnsatisfiedLinkError error) {
      Context context = Mapbox.getApplicationContext();
      if (context != null) {
        Timber.d("Loading %s from internal storage.", LIBRARY_NAME);
        System.load(getLibraryLocation(context).getAbsolutePath());
      }
    }
  }

  /**
   * Returns a file in the app internal storage that may contain a locally cached copy
   * of the Mapbox native library.
   *
   * @param context The application context
   * @return a file object
   */
  public static File getLibraryLocation(Context context) {
    return new File(context.getFilesDir(), LIBRARY_NAME);
  }
}
