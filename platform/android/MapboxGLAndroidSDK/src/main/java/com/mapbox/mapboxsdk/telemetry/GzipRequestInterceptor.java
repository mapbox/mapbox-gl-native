package com.mapbox.mapboxsdk.telemetry;

import timber.log.Timber;

import java.io.IOException;

import okhttp3.Interceptor;
import okhttp3.MediaType;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;
import okio.BufferedSink;
import okio.GzipSink;
import okio.Okio;

/**
 * OkHttp Interceptor for Gzipping Telemetry Data requests to the server.
 * Based on: https://github.com/square/okhttp/wiki/Interceptors
 */
public final class GzipRequestInterceptor implements Interceptor {

  @Override
  public Response intercept(Interceptor.Chain chain) throws IOException {
    Request originalRequest = chain.request();
    if (originalRequest.body() == null || originalRequest.header("Content-Encoding") != null) {
      Timber.d("Not compressing");
      return chain.proceed(originalRequest);
    }

    Timber.d("Compressing");
    Request compressedRequest = originalRequest.newBuilder()
      .header("Content-Encoding", "gzip")
      .method(originalRequest.method(), gzip(originalRequest.body()))
      .build();
    return chain.proceed(compressedRequest);
  }

  private RequestBody gzip(final RequestBody body) {
    return new RequestBody() {
      @Override
      public MediaType contentType() {
        return body.contentType();
      }

      @Override
      public long contentLength() {
        return -1; // We don't know the compressed length in advance!
      }

      @Override
      public void writeTo(BufferedSink sink) throws IOException {
        BufferedSink gzipSink = Okio.buffer(new GzipSink(sink));
        body.writeTo(gzipSink);
        gzipSink.close();
      }
    };
  }
}
