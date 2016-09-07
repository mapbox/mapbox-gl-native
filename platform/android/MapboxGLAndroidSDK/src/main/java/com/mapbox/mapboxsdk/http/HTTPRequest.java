package com.mapbox.mapboxsdk.http;

import android.text.TextUtils;
import android.util.Log;

import com.mapbox.mapboxsdk.MapboxAccountManager;
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

class HTTPRequest implements Callback {

    private static OkHttpClient mClient = new OkHttpClient();
    private final String LOG_TAG = HTTPRequest.class.getName();

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

    private native void nativeOnResponse(int code, String etag, String modified, String cacheControl, String expires, byte[] body);

    private HTTPRequest(long nativePtr, String resourceUrl, String userAgent, String etag, String modified) {
        mNativePtr = nativePtr;

        try {
            // Don't try a request if we aren't connected
            if (!MapboxAccountManager.getInstance().isConnected()) {
                throw new NoRouteToHostException("No Internet connection available.");
            }

            HttpUrl httpUrl = HttpUrl.parse(resourceUrl);
            final String host = httpUrl.host().toLowerCase(MapboxConstants.MAPBOX_LOCALE);
            if (host.equals("mapbox.com") || host.endsWith(".mapbox.com")) {
                if (httpUrl.querySize() == 0) {
                    resourceUrl = resourceUrl + "?";
                } else {
                    resourceUrl = resourceUrl + "&";
                }
                resourceUrl = resourceUrl + "events=true";
            }

            Request.Builder builder = new Request.Builder().url(resourceUrl).tag(resourceUrl.toLowerCase(MapboxConstants.MAPBOX_LOCALE)).addHeader("User-Agent", userAgent);
            if (etag.length() > 0) {
                builder = builder.addHeader("If-None-Match", etag);
            } else if (modified.length() > 0) {
                builder = builder.addHeader("If-Modified-Since", modified);
            }
            mRequest = builder.build();
            mCall = mClient.newCall(mRequest);
            mCall.enqueue(this);
        } catch (Exception e) {
            onFailure(e);
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
            Log.v(LOG_TAG, String.format("[HTTP] Request was successful (code = %d).", response.code()));
        } else {
            // We don't want to call this unsuccessful because a 304 isn't really an error
            String message = !TextUtils.isEmpty(response.message()) ? response.message() : "No additional information";
            Log.d(LOG_TAG, String.format(
                    "[HTTP] Request with response code = %d: %s",
                    response.code(), message));
        }

        byte[] body;
        try {
            body = response.body().bytes();
        } catch (IOException e) {
            onFailure(e);
            //throw e;
            return;
        } finally {
            response.body().close();
        }

        mLock.lock();
        if (mNativePtr != 0) {
            nativeOnResponse(response.code(), response.header("ETag"), response.header("Last-Modified"), response.header("Cache-Control"), response.header("Expires"), body);
        }
        mLock.unlock();
    }

    @Override
    public void onFailure(Call call, IOException e) {
        onFailure(e);
    }

    private void onFailure(Exception e) {
        Log.w(LOG_TAG, String.format("[HTTP] Request could not be executed: %s", e.getMessage()));

        int type = PERMANENT_ERROR;
        if ((e instanceof NoRouteToHostException) || (e instanceof UnknownHostException) || (e instanceof SocketException) || (e instanceof ProtocolException) || (e instanceof SSLException)) {
            type = CONNECTION_ERROR;
        } else if ((e instanceof InterruptedIOException)) {
            type = TEMPORARY_ERROR;
        }

        String errorMessage = e.getMessage() != null ? e.getMessage() : "Error processing the request";

        mLock.lock();
        if (mNativePtr != 0) {
            nativeOnFailure(type, errorMessage);
        }
        mLock.unlock();
    }
}
