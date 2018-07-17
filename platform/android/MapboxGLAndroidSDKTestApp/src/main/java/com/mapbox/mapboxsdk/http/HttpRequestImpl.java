package com.mapbox.mapboxsdk.http;

import com.koushikdutta.async.future.Future;
import com.koushikdutta.ion.Ion;
import com.koushikdutta.ion.Response;
import com.koushikdutta.ion.builder.Builders;
import com.mapbox.mapboxsdk.Mapbox;

import java.util.concurrent.ExecutionException;

public class HttpRequestImpl extends HttpRequest {

  @Override
  public void executeRequest(HttpRequestResponder httpRequest, long nativePtr, String resourceUrl,
                             String etag, String modified) {
    Builders.Any.B loadBuilder = Ion.with(Mapbox.getApplicationContext()).load(resourceUrl);
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

  @Override
  public void cancelRequest() {
    // do nothing
    // TODO manage Future objects and execute Future#cancel
  }
}
