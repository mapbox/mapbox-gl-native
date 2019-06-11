package com.mapbox.mapboxsdk.module.loader;

import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.LibraryLoaderProvider;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;
import com.mapbox.mapboxsdk.log.Logger;

import static com.facebook.soloader.SoLoader.init;
import static com.facebook.soloader.SoLoader.loadLibrary;

/**
 * Concrete implementation of a native library loader.
 * <p>
 * Uses ReLinker from https://github.com/KeepSafe/ReLinker.
 * </p>
 */
public class LibraryLoaderProviderImpl implements LibraryLoaderProvider {

  /**
   * Creates and returns a the default Library Loader.
   *
   * @return the default library loader
   */
  @Override
  public LibraryLoader getDefaultLibraryLoader() {
    return new SoLibraryLoader();
  }

  /**
   * Concrete implementation of a LibraryLoader using ReLinker.
   */
  private static class SoLibraryLoader extends LibraryLoader {

    private static final String TAG = "SoLibraryLoader";

    @Override
    public void load(String name) {
      try {
        // nativeExopackage = false, https://buck.build/article/exopackage.html
        init(Mapbox.getApplicationContext(), false);
        loadLibrary(name);
      } catch (MapboxConfigurationException exception) {
        Logger.e(TAG, "Couldn't load so file with relinker, application context missing, "
          + "call Mapbox.getInstance(Context context, String accessToken) first");
      }
    }
  }
}
