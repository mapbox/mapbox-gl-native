package com.mapbox.mapboxsdk.http;

import android.text.TextUtils;
import android.util.Log;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.ProtocolException;
import java.net.SocketException;
import java.net.UnknownHostException;

import javax.net.ssl.SSLException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Interceptor;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

class HTTPContext {

    private static final int CONNECTION_ERROR = 0;
    private static final int TEMPORARY_ERROR = 1;
    private static final int PERMANENT_ERROR = 2;
    private static final int CANCELED_ERROR = 3;

    private static HTTPContext mInstance = null;

    private OkHttpClient mClient;

    private HTTPContext() {
        super();
        mClient = new OkHttpClient();
        //mClient.interceptors().add(new LoggingInterceptor());
    }

    public static HTTPContext getInstance() {
        if (mInstance == null) {
            mInstance = new HTTPContext();
        }

        return mInstance;
    }

    public HTTPRequest createRequest(long nativePtr, String resourceUrl, String userAgent, String etag, String modified) {
        return new HTTPRequest(nativePtr, resourceUrl, userAgent, etag, modified);
    }

    public class HTTPRequest implements Callback {
        private final String LOG_TAG = HTTPRequest.class.getName();

        private long mNativePtr = 0;

        private Call mCall;
        private Request mRequest;

        private native void nativeOnFailure(long nativePtr, int type, String message);
        private native void nativeOnResponse(long nativePtr, int code, String message, String etag, String modified, String cacheControl, String expires, byte[] body);

        private HTTPRequest(long nativePtr, String resourceUrl, String userAgent, String etag, String modified) {
            mNativePtr = nativePtr;
            Request.Builder builder = new Request.Builder().url(resourceUrl).tag(resourceUrl.toLowerCase(MapboxConstants.MAPBOX_LOCALE)).addHeader("User-Agent", userAgent);
            if (etag.length() > 0) {
                builder = builder.addHeader("If-None-Match", etag);
            } else if (modified.length() > 0) {
                builder = builder.addHeader("If-Modified-Since", modified);
            }
            mRequest = builder.build();
        }

        public void start() {
            mCall = HTTPContext.getInstance().mClient.newCall(mRequest);
            mCall.enqueue(this);
        }

        public void cancel() {
            mCall.cancel();
        }

        @Override
        public void onResponse(Call call, Response response) throws IOException {
            if (response.isSuccessful()) {
                Log.d(LOG_TAG, String.format("[HTTP] Request was successful (code = %d).", response.code()));
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
                onFailure(null, e);
                //throw e;
                return;
            } finally {
                response.body().close();
            }

            nativeOnResponse(mNativePtr, response.code(), response.message(), response.header("ETag"), response.header("Last-Modified"), response.header("Cache-Control"), response.header("Expires"), body);
        }

        @Override
        public void onFailure(Call call, IOException e) {
            Log.w(LOG_TAG, String.format("[HTTP] Request could not be executed: %s", e.getMessage()));

            int type = PERMANENT_ERROR;
            if ((e instanceof UnknownHostException) || (e instanceof SocketException) || (e instanceof ProtocolException) || (e instanceof SSLException)) {
                type = CONNECTION_ERROR;
            } else if ((e instanceof InterruptedIOException)) {
                type = TEMPORARY_ERROR;
            } else if (mCall.isCanceled()) {
                type = CANCELED_ERROR;
            }

            nativeOnFailure(mNativePtr, type, e.getMessage());
        }
    }

    /*
     * Application interceptor that logs the outgoing request and the incoming response.
     * Based on https://github.com/square/okhttp/wiki/Interceptors
     */

    class LoggingInterceptor implements Interceptor {

        private final static String LOG_TAG = "LoggingInterceptor";

        @Override public Response intercept(Interceptor.Chain chain) throws IOException {
            Request request = chain.request();

            long t1 = System.nanoTime();
            Log.i(LOG_TAG, String.format("Sending request %s on %s%n%s",
                    request.url(), chain.connection(), request.headers()));

            Response response = chain.proceed(request);

            long t2 = System.nanoTime();
            Log.i(LOG_TAG, String.format("Received response for %s in %.1fms%n%s",
                    response.request().url(), (t2 - t1) / 1e6d, response.headers()));

            return response;
        }
    }
}
