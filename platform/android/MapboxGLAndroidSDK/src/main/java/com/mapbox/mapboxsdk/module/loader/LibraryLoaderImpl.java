package com.mapbox.mapboxsdk.module.loader;

import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.ModuleProvider;
import com.mapbox.mapboxsdk.log.Logger;

import static android.content.ContentValues.TAG;

/**
 * Loads the mapbox-gl shared library
 * <p>
 * By default uses the {@link System#loadLibrary(String)},
 * use {@link com.mapbox.mapboxsdk.Mapbox#setModuleProvider(ModuleProvider)} to provide an alternative
 * library loading hook.
 * </p>
 */
public class LibraryLoaderImpl implements LibraryLoader {

  private boolean loaded;

  @Override
  public void load() {
    if (loaded) {
      return;
    }

    try {
      System.loadLibrary("mapbox-gl");
      loaded = true;
    } catch (UnsatisfiedLinkError error) {
      String message = "Failed to load native shared library.";
      Logger.e(TAG, message, error);
      MapStrictMode.strictModeViolation(message, error);
    } catch (java.lang.NoClassDefFoundError error){
      String message = "Failed to load native shared library.";
      Logger.e(TAG, message, error);
      MapStrictMode.strictModeViolation(message, error);
    }
  }
}