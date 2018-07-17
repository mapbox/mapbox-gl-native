package com.mapbox.mapboxsdk.http;

import android.util.Log;
import com.mapbox.mapboxsdk.log.Logger;

import static com.mapbox.mapboxsdk.http.HttpRequest.CONNECTION_ERROR;
import static com.mapbox.mapboxsdk.http.HttpRequest.TEMPORARY_ERROR;

public class HttpLogger {

  private static final String TAG = "Mbgl-HttpRequest";

  public static boolean logRequestUrl;
  public static boolean logEnabled = true;

  private HttpLogger(){
  }

  public static void logFailure(int type, String errorMessage, String requestUrl) {
    log(type == TEMPORARY_ERROR ? Log.DEBUG : type == CONNECTION_ERROR ? Log.INFO : Log.WARN,
      String.format(
        "Request failed due to a %s error: %s %s",
        type == TEMPORARY_ERROR ? "temporary" : type == CONNECTION_ERROR ? "connection" : "permanent",
        errorMessage,
        logRequestUrl ? requestUrl : ""
      )
    );
  }

  public static void log(int type, String errorMessage) {
    if (logEnabled) {
      Logger.log(type, TAG, errorMessage);
    }
  }
}
