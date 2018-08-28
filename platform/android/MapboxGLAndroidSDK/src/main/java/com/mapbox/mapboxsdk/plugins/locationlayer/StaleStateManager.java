package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.os.Handler;

/**
 * Class controls the location layer stale state when the {@link android.location.Location} hasn't
 * been updated in 'x' amount of time. {@link LocationLayerOptions#staleStateTimeout()} can be used to
 * control the amount of time before the locations considered stale.
 * {@link LocationLayerOptions#enableStaleState()} is available for disabling this behaviour.
 */
class StaleStateManager {

  private final OnLocationStaleListener innerOnLocationStaleListeners;
  private final Handler handler;
  private boolean isStale = true;
  private long delayTime;

  StaleStateManager(OnLocationStaleListener innerListener, long delayTime) {
    innerOnLocationStaleListeners = innerListener;
    this.delayTime = delayTime;
    handler = new Handler();
    innerOnLocationStaleListeners.onStaleStateChange(true);
  }

  private Runnable staleStateRunnable = new Runnable() {
    @Override
    public void run() {
      isStale = true;
      innerOnLocationStaleListeners.onStaleStateChange(true);
    }
  };

  boolean isStale() {
    return isStale;
  }

  void updateLatestLocationTime() {
    if (isStale) {
      isStale = false;
      innerOnLocationStaleListeners.onStaleStateChange(false);
    }
    postTheCallback();
  }

  void setDelayTime(long delayTime) {
    this.delayTime = delayTime;
    postTheCallback();
  }

  void onStart() {
    if (!isStale) {
      postTheCallback();
    }
  }

  void onStop() {
    handler.removeCallbacksAndMessages(null);
  }

  private void postTheCallback() {
    handler.removeCallbacksAndMessages(null);
    handler.postDelayed(staleStateRunnable, delayTime);
  }
}
