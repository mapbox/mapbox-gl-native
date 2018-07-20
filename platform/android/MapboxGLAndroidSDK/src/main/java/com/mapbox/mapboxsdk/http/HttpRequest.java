package com.mapbox.mapboxsdk.http;

import android.support.annotation.Nullable;

public abstract class HttpRequest {

  private static HttpRequest httpRequest;

  static final int CONNECTION_ERROR = 0;
  static final int TEMPORARY_ERROR = 1;
  static final int PERMANENT_ERROR = 2;

  public static synchronized void setHttpRequest(@Nullable HttpRequest requestImpl) {
    httpRequest = requestImpl;
  }

  public abstract void executeRequest(NativeHttpRequest httpRequest, long nativePtr, String resourceUrl,
                                      String etag, String modified);

  public abstract void cancelRequest(long nativePtr);

  static void execute(NativeHttpRequest httpRequest, long nativePtr, String resourceUrl,
                             String etag, String modified) {
    getInstance().executeRequest(httpRequest, nativePtr, resourceUrl, etag, modified);
  }

  static void cancel(long nativePtr) {
    getInstance().cancelRequest(nativePtr);
  }

  private static synchronized HttpRequest getInstance() {
    if (httpRequest == null) {
      httpRequest = new OkHttpRequest();
    }
    return httpRequest;
  }
}
