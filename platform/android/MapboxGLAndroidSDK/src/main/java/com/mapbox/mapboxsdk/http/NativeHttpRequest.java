package com.mapbox.mapboxsdk.http;

import java.util.concurrent.locks.ReentrantLock;

public class NativeHttpRequest {

  // Reentrancy is not needed, but "Lock" is an abstract class.
  private ReentrantLock lock = new ReentrantLock();
  private long nativePtr = 0;

  private NativeHttpRequest(long nativePtr, String resourceUrl, String etag, String modified) {
    this.nativePtr = nativePtr;

    if (resourceUrl.startsWith("local://")) {
      // used by render test to serve files from assets
      executeLocalRequest(resourceUrl);
      return;
    }
    HttpRequest.execute(this, nativePtr, resourceUrl, etag, modified);
  }

  public void cancel() {
    HttpRequest.cancel(nativePtr);

    // TODO: We need a lock here because we can try
    // to cancel at the same time the request is getting
    // answered on the OkHTTP thread. We could get rid of
    // this lock by using Runnable when we move Android
    // implementation of mbgl::RunLoop to Looper.
    lock.lock();
    nativePtr = 0;
    lock.unlock();
  }

  public void onResponse(int responseCode, String etag, String lastModified, String cacheControl, String expires,
                         String retryAfter, String xRateLimitReset, byte[] body) {
    lock.lock();
    if (nativePtr != 0) {
      nativeOnResponse(responseCode,
        etag,
        lastModified,
        cacheControl,
        expires,
        retryAfter,
        xRateLimitReset,
        body);
    }
    lock.unlock();
  }

  private void executeLocalRequest(String resourceUrl) {
    new LocalRequestTask(bytes -> {
      if (bytes != null) {
        lock.lock();
        if (nativePtr != 0) {
          nativeOnResponse(200, null, null, null, null, null, null, bytes);
        }
        lock.unlock();
      }
    }).execute(resourceUrl);
  }

  public void handleFailure(int type, String errorMessage) {
    lock.lock();
    if (nativePtr != 0) {
      nativeOnFailure(type, errorMessage);
    }
    lock.unlock();
  }

  private native void nativeOnFailure(int type, String message);

  private native void nativeOnResponse(int code, String etag, String modified, String cacheControl, String expires,
                                       String retryAfter, String xRateLimitReset, byte[] body);
}
