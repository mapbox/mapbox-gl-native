package com.mapbox.mapboxsdk.storage;

import android.support.annotation.UiThread;
import android.support.annotation.NonNull;

@UiThread
public class DefaultFileSource {
  //
  // Native peer management
  //
  static {
    System.loadLibrary("mapbox-gl");
  }

  // Pointer to the native peer
  private long peer;

  // Initializes the native peer object
  private native void initialize(String cachePath, String assetRoot, long maximumCacheSize);

  @Override
  protected native void finalize() throws Throwable;

  //
  // Constants
  //

  /*
   * The maximumCacheSize parameter is a limit applied to non-offline resources only,
   * i.e. resources added to the database for the "ambient use" caching functionality.
   * There is no size limit for offline resources.
   */
  private static final long DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

  //
  // Constructors
  //

  /**
   * Creates a new DefaultFileSource object.
   *
   * @param cachePath Path to the cache database file
   * @param assetRoot Path to the APK that contains the assets
   * @param maximumCacheSize Maximum size of the database file, in bytes
   */
  public DefaultFileSource(@NonNull String cachePath, @NonNull String assetRoot, long maximumCacheSize) {
    initialize(cachePath, assetRoot, maximumCacheSize);
  }

  /**
   * Creates a new DefaultFileSource object.
   *
   * @param cachePath Path to the cache database file
   * @param assetRoot Path to the APK that contains the assets
   */
  public DefaultFileSource(@NonNull String cachePath, @NonNull String assetRoot) {
    initialize(cachePath, assetRoot, DEFAULT_MAX_CACHE_SIZE);
  }

  //
  // Methods
  //

  /**
   * Sets the API base URL when connecting to Mapbox servers. Defaults to <code>https://api.mapbox.com</code>.
   *
   * @param apiBaseURL New path prefix of Mapbox-hosted assets
   */
  public native void setAPIBaseURL(@NonNull String apiBaseURL);

  /**
   * Returns the currently set API base URL for Mapbox-hosted assets.
   *
   * @return The path prefix for Mapbox-hosted assets
   */
  public native String getAPIBaseURL();

  /**
   * Sets the access token for Mapbox resources.
   *
   * @param accessToken New access token for Mapbox-hosted assets
   */
  public native void setAccessToken(@NonNull String accessToken);

  /**
   * Returns the current Mapbox access token.
   *
   * @return The access token currently used for requesting Mapbox-hosted assets
   */
  public native String getAccessToken();
}
