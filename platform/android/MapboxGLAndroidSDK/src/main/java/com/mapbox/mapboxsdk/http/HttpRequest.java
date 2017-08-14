package com.mapbox.mapboxsdk.http;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Build;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.io.IOException;
import java.io.InterruptedIOException;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.net.NoRouteToHostException;
import java.net.ProtocolException;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.concurrent.locks.ReentrantLock;

import javax.net.ssl.SSLException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.HttpUrl;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.internal.Util;
import timber.log.Timber;

/**
 * Gateway to request and return HttpRequest to c++
 * <p>
 * Reentrancy lock is not needed, but "Lock" is an abstract class.
 * </p>
 */
class HttpRequest implements Callback {

  @IntDef( {CONNECTION_ERROR, TEMPORARY_ERROR, PERMANENT_ERROR})
  @Retention(RetentionPolicy.SOURCE)
  @interface HttpError {
  }

  private static final int CONNECTION_ERROR = 0;
  private static final int TEMPORARY_ERROR = 1;
  private static final int PERMANENT_ERROR = 2;

  private static final ReentrantLock LOCK = new ReentrantLock();
  private static final OkHttpClient HTTP_CLIENT = new OkHttpClient();

  private static boolean logEnabled = true;
  private static String userAgentString;

  private long nativePtr;
  private Call call;

  private native void nativeOnFailure(@HttpError int type, String message);

  private native void nativeOnResponse(int code, String etag, String modified, String cacheControl, String expires,
                                       String retryAfter, String xRateLimitReset, byte[] body);

  private HttpRequest(long nativePtr, String resourceUrl, String etag, String modified) {
    this.nativePtr = nativePtr;

    try {
      // Don't try a request if we aren't connected
      if (!Mapbox.isConnected()) {
        throw new NoRouteToHostException("No Internet connection available.");
      }

      resourceUrl = adaptResourceUrl(resourceUrl);
      call = HTTP_CLIENT.newCall(buildRequest(resourceUrl, etag, modified));
      call.enqueue(this);
    } catch (Exception exception) {
      onFailure(exception);
    }
  }

  private String adaptResourceUrl(String resourceUrl) {
    HttpUrl httpUrl = HttpUrl.parse(resourceUrl);
    if (httpUrl != null && isMapboxHost(httpUrl)) {
      HttpUrl.Builder builder = httpUrl.newBuilder();
      builder.addQueryParameter("events", "true");
      resourceUrl = builder.build().toString();
    }
    return resourceUrl;
  }

  private boolean isMapboxHost(@NonNull HttpUrl httpUrl) {
    final String host = httpUrl.host().toLowerCase(MapboxConstants.MAPBOX_LOCALE);
    return host.equals("mapbox.com") || host.endsWith(".mapbox.com")
      || host.equals("mapbox.cn") || host.endsWith(".mapbox.cn");
  }

  private Request buildRequest(String resourceUrl, String etag, String modified) {
    Request.Builder builder = new Request.Builder()
      .url(resourceUrl)
      .tag(resourceUrl.toLowerCase(MapboxConstants.MAPBOX_LOCALE))
      .addHeader("User-Agent", getUserAgentString());
    if (etag.length() > 0) {
      builder = builder.addHeader("If-None-Match", etag);
    } else if (modified.length() > 0) {
      builder = builder.addHeader("If-Modified-Since", modified);
    }
    return builder.build();
  }

  public void cancel() {
    // call can be null if the constructor gets aborted (e.g, under a NoRouteToHostException).
    if (call != null) {
      call.cancel();
    }

    // TODO: We need a LOCK here because we can try
    // to cancel at the same time the request is getting
    // answered on the OkHTTP thread. We could get rid of
    // this LOCK by using Runnable when we move Android
    // implementation of mbgl::RunLoop to Looper.
    LOCK.lock();
    nativePtr = 0;
    LOCK.unlock();
  }

  @Override
  public void onResponse(@NonNull Call call, @NonNull Response response) throws IOException {
    if (logEnabled) {
      logOnResponse(response);
    }

    byte[] body;
    try {
      body = response.body().bytes();
    } catch (IOException ioException) {
      onFailure(ioException);
      return;
    } finally {
      response.body().close();
    }

    LOCK.lock();
    if (nativePtr != 0) {
      nativeOnResponse(response.code(),
        response.header("ETag"),
        response.header("Last-Modified"),
        response.header("Cache-Control"),
        response.header("Expires"),
        response.header("Retry-After"),
        response.header("x-rate-limit-reset"),
        body);
    }
    LOCK.unlock();
  }

  private void logOnResponse(Response response) {
    if (response.isSuccessful()) {
      Timber.v("[HTTP] Request was successful (code = %s).", response.code());
    } else if (logEnabled) {
      // We don't want to call this unsuccessful because a 304 isn't really an error
      String message = !TextUtils.isEmpty(response.message()) ? response.message() : "No additional information";
      Timber.d("[HTTP] Request with response code = %s: %s", response.code(), message);
    }
  }

  @Override
  public void onFailure(@NonNull Call call, @NonNull IOException exception) {
    onFailure(exception);
  }

  private void onFailure(Exception exception) {
    int failType = resolveFailureType(exception);
    String errorMessage = exception.getMessage() != null ? exception.getMessage() : "Error processing the request";
    if (logEnabled) {
      logOnFailure(failType, errorMessage);
    }

    LOCK.lock();
    if (nativePtr != 0) {
      nativeOnFailure(failType, errorMessage);
    }
    LOCK.unlock();
  }

  @HttpError
  private int resolveFailureType(Exception exception) {
    int type = PERMANENT_ERROR;
    if ((exception instanceof NoRouteToHostException) || (exception instanceof UnknownHostException)
      || (exception instanceof SocketException) || (exception instanceof ProtocolException)
      || (exception instanceof SSLException)) {
      type = CONNECTION_ERROR;
    } else if ((exception instanceof InterruptedIOException)) {
      type = TEMPORARY_ERROR;
    }
    return type;
  }

  private void logOnFailure(@HttpError int type, String errorMessage) {
    if (type == TEMPORARY_ERROR) {
      Timber.d("Request failed due to a temporary error: %s", errorMessage);
    } else if (type == CONNECTION_ERROR) {
      Timber.i("Request failed due to a connection error: %s", errorMessage);
    } else {
      // PERMANENT_ERROR
      Timber.w("Request failed due to a permanent error: %s", errorMessage);
    }
  }

  private String getUserAgentString() {
    if (userAgentString == null) {
      userAgentString = Util.toHumanReadableAscii(
        String.format("%s %s (%s) Android/%s (%s)",
          getApplicationIdentifier(),
          BuildConfig.MAPBOX_VERSION_STRING,
          BuildConfig.GIT_REVISION_SHORT,
          Build.VERSION.SDK_INT,
          Build.CPU_ABI)
      );
    }
    return userAgentString;
  }

  private String getApplicationIdentifier() {
    try {
      Context context = Mapbox.getApplicationContext();
      PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
      return String.format("%s/%s (%s)", context.getPackageName(), packageInfo.versionName, packageInfo.versionCode);
    } catch (Exception exception) {
      return "";
    }
  }

  static void enableLog(boolean enabled) {
    logEnabled = enabled;
  }
}
