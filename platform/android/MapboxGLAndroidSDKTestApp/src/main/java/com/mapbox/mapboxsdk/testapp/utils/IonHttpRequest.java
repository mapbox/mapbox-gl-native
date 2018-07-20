package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;

import com.koushikdutta.ion.Ion;
import com.koushikdutta.ion.Response;
import com.koushikdutta.ion.builder.Builders;
import com.mapbox.mapboxsdk.http.HttpRequest;
import com.mapbox.mapboxsdk.http.NativeHttpRequest;

import java.lang.ref.WeakReference;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

public class IonHttpRequest extends HttpRequest {

  private WeakReference<Context> context;

  public IonHttpRequest(Context context) {
    this.context = new WeakReference<>(context);
  }

  @Override
  public void executeRequest(NativeHttpRequest httpRequest, long nativePtr, String resourceUrl,
                             String etag, String modified) {
    Context context = this.context.get();
    if (context != null) {
      Builders.Any.B loadBuilder = Ion.with(context).load(resourceUrl);
      if (etag.length() > 0) {
        loadBuilder.addHeader("If-None-Match", etag);
      } else if (modified.length() > 0) {
        loadBuilder.addHeader("If-Modified-Since", modified);
      }
      Future<Response<byte[]>> future = loadBuilder.asByteArray().withResponse();
      try {
        Response<byte[]> result = future.get();
        int statusCode = result.getHeaders().code();
        httpRequest.onResponse(statusCode, null, null, null, null, null, null, result.getResult());
      } catch (InterruptedException interruptedException) {
        interruptedException.printStackTrace();
      } catch (ExecutionException executionException) {
        executionException.printStackTrace();
      }
    }
  }

  @Override
  public void cancelRequest(long nativePtr) {
    // do nothing
    // TODO manage Future objects and execute Future#cancel
  }

}
