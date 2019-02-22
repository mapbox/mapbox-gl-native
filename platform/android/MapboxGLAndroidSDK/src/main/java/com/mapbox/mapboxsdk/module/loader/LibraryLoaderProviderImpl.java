package com.mapbox.mapboxsdk.module.loader;

import android.content.Context;
import com.getkeepsafe.relinker.ReLinker;
import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.LibraryLoaderProvider;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;
import com.mapbox.mapboxsdk.log.Logger;

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
    return new ReLinkerLibraryLoader();
  }

  /**
   * Concrete implementation of a LibraryLoader using ReLinker.
   */
  private static class ReLinkerLibraryLoader extends LibraryLoader {

    @Override
    public void load(String name) {
      try {
        Context context = Mapbox.getApplicationContext();
        ReLinker.log(new LibraryLogger()).loadLibrary(context, name);
      } catch (MapboxConfigurationException exception) {
        Logger.e(LibraryLogger.TAG, "Couldn't load so file with relinker, application context missing, "
          + "call Mapbox.getInstance(Context context, String accessToken) first");
      }
    }

    /**
     * Relinker library loader logger.
     */
    private static class LibraryLogger implements ReLinker.Logger {

      private static final String TAG = "Mbgl-LibraryLoader";

      @Override
      public void log(String message) {
        Logger.d(TAG, message);
      }
    }
  }
}
