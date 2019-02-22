package com.mapbox.mapboxsdk;

/**
 * Injects the default library loader.
 */
public interface LibraryLoaderProvider {

  /**
   * Creates and returns a the default Library Loader.
   *
   * @return the default library loader
   */
  LibraryLoader getDefaultLibraryLoader();

}
