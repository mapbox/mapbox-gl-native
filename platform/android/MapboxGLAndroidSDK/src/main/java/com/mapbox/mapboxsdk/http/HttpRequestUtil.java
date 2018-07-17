package com.mapbox.mapboxsdk.http;

import okhttp3.OkHttpClient;

/**
 * Utility class for setting HttpRequest configurations.
 *
 * @deprecated use {@link com.mapbox.mapboxsdk.module.http.HttpRequestUtil} instead
 */
@Deprecated
public class HttpRequestUtil {

  /**
   * Set the log state of HttpRequest. Default value is true.
   * <p>
   * This configuration will outlast the lifecycle of the Map.
   * </p>
   *
   * @param enabled True will enable logging, false will disable
   * @deprecated use {@link com.mapbox.mapboxsdk.module.http.HttpRequestUtil#setLogEnabled(boolean)} instead
   */
  @Deprecated
  public static void setLogEnabled(boolean enabled) {
    com.mapbox.mapboxsdk.module.http.HttpRequestUtil.setLogEnabled(enabled);
  }

  /**
   * Enable printing of the request url when an error occurred. Default value is false.
   * <p>
   * Requires {@link #setLogEnabled(boolean)} to be activated.
   * </p>
   * <p>
   * This configuration will outlast the lifecycle of the Map.
   * </p>
   *
   * @param enabled True will print urls, false will disable
   * @deprecated use {@link com.mapbox.mapboxsdk.module.http.HttpRequestUtil#setPrintRequestUrlOnFailure(boolean)}
   * instead
   */
  @Deprecated
  public static void setPrintRequestUrlOnFailure(boolean enabled) {
    com.mapbox.mapboxsdk.module.http.HttpRequestUtil.setPrintRequestUrlOnFailure(enabled);
  }

  /**
   * Set the OkHttpClient used for requesting map resources.
   *
   * @param client the OkHttpClient
   * @deprecated use {@link com.mapbox.mapboxsdk.module.http.HttpRequestUtil#setOkHttpClient(OkHttpClient)} instead.
   */
  @Deprecated
  public static void setOkHttpClient(OkHttpClient client) {
    com.mapbox.mapboxsdk.module.http.HttpRequestUtil.setOkHttpClient(client);
  }
}