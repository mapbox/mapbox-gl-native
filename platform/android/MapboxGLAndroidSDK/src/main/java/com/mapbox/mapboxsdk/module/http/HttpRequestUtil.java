package com.mapbox.mapboxsdk.module.http;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import okhttp3.OkHttpClient;
import okio.Buffer;

/**
 * Utility class for setting OkHttpRequest configurations
 */
public class HttpRequestUtil {

  /**
   * Set the log state of OkHttpRequest. Default value is true.
   * <p>
   * This configuration will outlast the lifecycle of the Map.
   * </p>
   *
   * @param enabled True will enable logging, false will disable
   */
  public static void setLogEnabled(boolean enabled) {
    HttpRequestImpl.enableLog(enabled);
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
    HttpRequestImpl.enablePrintRequestUrlOnFailure(enabled);
  }

  /**
   * Set the OkHttpClient used for requesting map resources.
   * <p>
   * This configuration survives across mapView instances.
   * Reset the OkHttpClient to the default by passing null as parameter.
   * </p>
   *
   * @param client the OkHttpClient
   */
  public static void setOkHttpClient(@Nullable OkHttpClient client) {
    HttpRequestImpl.setOkHttpClient(client);
  }

  @NonNull
  static String toHumanReadableAscii(String s) {
    for (int i = 0, length = s.length(), c; i < length; i += Character.charCount(c)) {
      c = s.codePointAt(i);
      if (c > '\u001f' && c < '\u007f') {
        continue;
      }

      Buffer buffer = new Buffer();
      buffer.writeUtf8(s, 0, i);
      for (int j = i; j < length; j += Character.charCount(c)) {
        c = s.codePointAt(j);
        buffer.writeUtf8CodePoint(c > '\u001f' && c < '\u007f' ? c : '?');
      }
      return buffer.readUtf8();
    }
    return s;
  }
}