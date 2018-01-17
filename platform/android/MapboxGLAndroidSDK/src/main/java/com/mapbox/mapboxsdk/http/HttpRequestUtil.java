package com.mapbox.mapboxsdk.http;

import okhttp3.OkHttpClient;

/**
 * Utility class for setting HttpRequest configurations
 */
public class HttpRequestUtil {

  /**
   * Set the log state of HttpRequest. Default value is true.
   * <p>
   * This configuration will outlast the lifecycle of the Map.
   * </p>
   *
   * @param enabled True will enable logging, false will disable
   */
  public static void setLogEnabled(boolean enabled) {
    HTTPRequest.enableLog(enabled);
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
   */
  public static void setPrintRequestUrlOnFailure(boolean enabled) {
    HTTPRequest.enablePrintRequestUrlOnFailure(enabled);
  }

  /**
   * Set the OkHttpClient used for requesting map resources.
   *
   * @param client the OkHttpClient
   */
  public static void setOkHttpClient(OkHttpClient client) {
    HTTPRequest.setOKHttpClient(client);
  }

}