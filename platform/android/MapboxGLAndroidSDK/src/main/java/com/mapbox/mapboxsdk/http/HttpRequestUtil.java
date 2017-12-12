package com.mapbox.mapboxsdk.http;

/**
 * Utility class for setting HttpRequest configurations
 */
public class HttpRequestUtil {

  /**
   * Set the log state of HttpRequest.
   * <p>
   * This configuration will outlast the lifecycle of the Map.
   * </p>
   *
   * @param enabled True will enable logging, false will disable
   */
  public static void setLogEnabled(boolean enabled) {
    HTTPRequest.enableLog(enabled);
  }
}