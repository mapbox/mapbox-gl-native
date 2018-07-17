package com.mapbox.mapboxsdk.http;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v4.util.LongSparseArray;
import android.text.TextUtils;
import android.util.Log;
import com.mapbox.android.telemetry.TelemetryUtils;
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Dispatcher;
import okhttp3.HttpUrl;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;
import timber.log.Timber;

import javax.net.ssl.SSLException;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.NoRouteToHostException;
import java.net.ProtocolException;
import java.net.SocketException;
import java.net.UnknownHostException;

public class OkHttpRequest extends HttpRequest {

  private static final LongSparseArray<Call> calls = new LongSparseArray<>();
  private static final String userAgentString =  TelemetryUtils.toHumanReadableAscii(
    String.format("%s %s (%s) Android/%s (%s)",
      getApplicationIdentifier(),
      BuildConfig.MAPBOX_VERSION_STRING,
      BuildConfig.GIT_REVISION_SHORT,
      Build.VERSION.SDK_INT,
      Build.CPU_ABI)
  );

  private static OkHttpClient client = new OkHttpClient.Builder().dispatcher(getDispatcher()).build();
  private static boolean logEnabled = true;
  private static boolean logRequestUrl = false;

  @Override
  public void executeRequest(NativeHttpRequest httpRequest, long nativePtr, String resourceUrl,
                             String etag, String modified) {
    Call call = null;
    OkHttpCallback callback = new OkHttpCallback(httpRequest);
    try {
      HttpUrl httpUrl = HttpUrl.parse(resourceUrl);
      if (httpUrl == null) {
        log(Log.ERROR, String.format("[HTTP] Unable to parse resourceUrl %s", resourceUrl));
      }

      final String host = httpUrl.host().toLowerCase(MapboxConstants.MAPBOX_LOCALE);
      // Don't try a request to remote server if we aren't connected
      if (!Mapbox.isConnected() && !host.equals("127.0.0.1") && !host.equals("localhost")) {
        throw new NoRouteToHostException("No Internet connection available.");
      }

      if (host.equals("mapbox.com") || host.endsWith(".mapbox.com") || host.equals("mapbox.cn")
        || host.endsWith(".mapbox.cn")) {
        if (httpUrl.querySize() == 0) {
          resourceUrl = resourceUrl + "?";
        } else {
          resourceUrl = resourceUrl + "&";
        }
        resourceUrl = resourceUrl + "events=true";
      }

      Request.Builder builder = new Request.Builder()
        .url(resourceUrl)
        .tag(resourceUrl.toLowerCase(MapboxConstants.MAPBOX_LOCALE))
        .addHeader("User-Agent", userAgentString);
      if (etag.length() > 0) {
        builder = builder.addHeader("If-None-Match", etag);
      } else if (modified.length() > 0) {
        builder = builder.addHeader("If-Modified-Since", modified);
      }
      Request request = builder.build();
      call = client.newCall(request);

      synchronized (calls) {
        calls.put(nativePtr, call);
      }

      call.enqueue(callback);
      Timber.e("Requesting resource with size %s for %s", calls.size(), call.hashCode());
    } catch (Exception exception) {
      callback.handleFailure(call, exception);
    }
  }

  @Override
  public void cancelRequest(long nativePtr) {
    synchronized (calls) {
      Call call = calls.get(nativePtr);
      // call can be null if the constructor gets aborted (e.g, under a NoRouteToHostException).
      if (call != null) {
        call.cancel();
        calls.delete(nativePtr);
      }
    }
  }

  public static void enablePrintRequestUrlOnFailure(boolean enabled) {
    logRequestUrl = enabled;
  }

  public static void enableLog(boolean enabled) {
    logEnabled = enabled;
  }

  public static void setOkHttpClient(OkHttpClient okHttpClient) {
    OkHttpRequest.client = okHttpClient;
  }

  private static class OkHttpCallback implements Callback {

    private NativeHttpRequest httpRequest;

    OkHttpCallback(NativeHttpRequest httpRequest) {
      this.httpRequest = httpRequest;
    }

    @Override
    public void onFailure(@NonNull Call call, @NonNull IOException e) {
      handleFailure(call, e);
    }

    @Override
    public void onResponse(@NonNull Call call, @NonNull Response response) {
      if (response.isSuccessful()) {
        log(Log.VERBOSE, String.format("[HTTP] Request was successful (code = %s).", response.code()));
      } else {
        // We don't want to call this unsuccessful because a 304 isn't really an error
        String message = !TextUtils.isEmpty(response.message()) ? response.message() : "No additional information";
        log(Log.DEBUG, String.format("[HTTP] Request with response code = %s: %s", response.code(), message));
      }

      removeCall(call);

      ResponseBody responseBody = response.body();
      if (responseBody == null) {
        log(Log.ERROR, "[HTTP] Received empty response body");
        return;
      }

      byte[] body;
      try {
        body = responseBody.bytes();
      } catch (IOException ioException) {
        onFailure(call, ioException);
        // throw ioException;
        return;
      } finally {
        response.close();
      }

      httpRequest.onResponse(response.code(),
        response.header("ETag"),
        response.header("Last-Modified"),
        response.header("Cache-Control"),
        response.header("Expires"),
        response.header("Retry-After"),
        response.header("x-rate-limit-reset"),
        body);
    }

    private void handleFailure(Call call, Exception e) {
      String errorMessage = e.getMessage() != null ? e.getMessage() : "Error processing the request";
      int type = getFailureType(e);

      if (logEnabled && call != null && call.request() != null) {
        String requestUrl = call.request().url().toString();
        logFailure(type, errorMessage, requestUrl);
      }

      removeCall(call);
      Timber.e("Handle Failure with size %s for %s", calls.size(), call.hashCode());
      httpRequest.handleFailure(type, errorMessage);
    }

    private void logFailure(int type, String errorMessage, String requestUrl) {
      log(type == TEMPORARY_ERROR ? Log.DEBUG : type == CONNECTION_ERROR ? Log.INFO : Log.WARN,
        String.format(
          "Request failed due to a %s error: %s %s",
          type == TEMPORARY_ERROR ? "temporary" : type == CONNECTION_ERROR ? "connection" : "permanent",
          errorMessage,
          logRequestUrl ? requestUrl : ""
        )
      );
    }

    private int getFailureType(Exception e) {
      if ((e instanceof NoRouteToHostException) || (e instanceof UnknownHostException) || (e instanceof SocketException)
        || (e instanceof ProtocolException) || (e instanceof SSLException)) {
        return CONNECTION_ERROR;
      } else if ((e instanceof InterruptedIOException)) {
        return TEMPORARY_ERROR;
      }
      return PERMANENT_ERROR;
    }

    private void removeCall(Call call) {
      synchronized (calls) {
        Call currentCall;
        for (int i = 0; i < calls.size(); i++) {
          currentCall = calls.valueAt(i);
          if (call.equals(currentCall)) {
            calls.delete(calls.keyAt(i));
            return;
          }
        }
      }
    }
  }

  private static String getApplicationIdentifier() {
    try {
      Context context = Mapbox.getApplicationContext();
      PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
      return String.format("%s/%s (%s)", context.getPackageName(), packageInfo.versionName, packageInfo.versionCode);
    } catch (Exception exception) {
      return "";
    }
  }

  private static Dispatcher getDispatcher() {
    Dispatcher dispatcher = new Dispatcher();
    // Matches core limit set on
    // https://github.com/mapbox/mapbox-gl-native/blob/master/platform/android/src/http_file_source.cpp#L192
    dispatcher.setMaxRequestsPerHost(20);
    return dispatcher;
  }

  static void log(int type, String errorMessage) {
    if (logEnabled) {
      Timber.log(type, errorMessage);
    }
  }
}
