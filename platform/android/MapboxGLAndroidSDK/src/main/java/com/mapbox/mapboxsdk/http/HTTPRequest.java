package com.mapbox.mapboxsdk.http;


import android.content.Context;
import android.content.pm.PackageInfo;
import android.os.Build;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.io.IOException;
import java.io.InterruptedIOException;
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

class HTTPRequest implements Callback {

  private static OkHttpClient mClient = new OkHttpClient();
  private String USER_AGENT_STRING = null;

  private static final int CONNECTION_ERROR = 0;
  private static final int TEMPORARY_ERROR = 1;
  private static final int PERMANENT_ERROR = 2;

  // Reentrancy is not needed, but "Lock" is an
  // abstract class.
  private ReentrantLock mLock = new ReentrantLock();

  private long mNativePtr = 0;

  private Call mCall;
  private Request mRequest;

  private native void nativeOnFailure(int type, String message);

  private native void nativeOnResponse(int code, String etag, String modified, String cacheControl, String expires,
                                       String retryAfter, String xRateLimitReset, byte[] body);

  private HTTPRequest(long nativePtr, String resourceUrl, String etag, String modified) {
    mNativePtr = nativePtr;

    try {
      // Don't try a request if we aren't connected
      if (!Mapbox.isConnected()) {
        throw new NoRouteToHostException("No Internet connection available.");
      }

      HttpUrl httpUrl = HttpUrl.parse(resourceUrl);
      final String host = httpUrl.host().toLowerCase(MapboxConstants.MAPBOX_LOCALE);
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
        .addHeader("User-Agent", getUserAgent());
      if (etag.length() > 0) {
        builder = builder.addHeader("If-None-Match", etag);
      } else if (modified.length() > 0) {
        builder = builder.addHeader("If-Modified-Since", modified);
      }
      mRequest = builder.build();
      mCall = mClient.newCall(mRequest);
      mCall.enqueue(this);
    } catch (Exception exception) {
      onFailure(exception);
    }
  }

  public void cancel() {
    // mCall can be null if the constructor gets aborted (e.g, under a NoRouteToHostException).
    if (mCall != null) {
      mCall.cancel();
    }

    // TODO: We need a lock here because we can try
    // to cancel at the same time the request is getting
    // answered on the OkHTTP thread. We could get rid of
    // this lock by using Runnable when we move Android
    // implementation of mbgl::RunLoop to Looper.
    mLock.lock();
    mNativePtr = 0;
    mLock.unlock();
  }

  @Override
  public void onResponse(Call call, Response response) throws IOException {
    if (response.isSuccessful()) {
      Timber.v(String.format("[HTTP] Request was successful (code = %d).", response.code()));
    } else {
      // We don't want to call this unsuccessful because a 304 isn't really an error
      String message = !TextUtils.isEmpty(response.message()) ? response.message() : "No additional information";
      Timber.d(String.format(
        "[HTTP] Request with response code = %d: %s",
        response.code(), message));
    }

    byte[] body;
    try {
      body = response.body().bytes();
    } catch (IOException ioException) {
      onFailure(ioException);
      // throw ioException;
      return;
    } finally {
      response.body().close();
    }

    mLock.lock();
    if (mNativePtr != 0) {
      nativeOnResponse(response.code(),
        response.header("ETag"),
        response.header("Last-Modified"),
        response.header("Cache-Control"),
        response.header("Expires"),
        response.header("Retry-After"),
        response.header("x-rate-limit-reset"),
        body);
    }
    mLock.unlock();
  }

  @Override
  public void onFailure(Call call, IOException e) {
    onFailure(e);
  }

  private void onFailure(Exception e) {
    int type = PERMANENT_ERROR;
    if ((e instanceof NoRouteToHostException) || (e instanceof UnknownHostException) || (e instanceof SocketException)
      || (e instanceof ProtocolException) || (e instanceof SSLException)) {
      type = CONNECTION_ERROR;
    } else if ((e instanceof InterruptedIOException)) {
      type = TEMPORARY_ERROR;
    }

    String errorMessage = e.getMessage() != null ? e.getMessage() : "Error processing the request";

    if (type == TEMPORARY_ERROR) {
      Timber.d(String.format(MapboxConstants.MAPBOX_LOCALE,
        "Request failed due to a temporary error: %s", errorMessage));
    } else if (type == CONNECTION_ERROR) {
      Timber.i(String.format(MapboxConstants.MAPBOX_LOCALE,
        "Request failed due to a connection error: %s", errorMessage));
    } else {
      // PERMANENT_ERROR
      Timber.w(String.format(MapboxConstants.MAPBOX_LOCALE,
        "Request failed due to a permanent error: %s", errorMessage));
    }

    mLock.lock();
    if (mNativePtr != 0) {
      nativeOnFailure(type, errorMessage);
    }
    mLock.unlock();
  }

  private String getUserAgent() {
    if (USER_AGENT_STRING == null) {
      return USER_AGENT_STRING = Util.toHumanReadableAscii(
        String.format("%s %s (%s) Android/%s (%s)",
          getApplicationIdentifier(),
          BuildConfig.MAPBOX_VERSION_STRING,
          BuildConfig.GIT_REVISION_SHORT,
          Build.VERSION.SDK_INT,
          Build.CPU_ABI)
      );
    } else {
      return USER_AGENT_STRING;
    }
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
}
