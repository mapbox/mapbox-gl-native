package com.mapbox.mapboxsdk.location;

import android.os.Handler;

/**
 * Class controls the location stale state when the {@link android.location.Location} hasn't
 * been updated in 'x' amount of time. {@link LocationComponentOptions#staleStateTimeout()} can be used to
 * control the amount of time before the location's considered stale.
 * {@link LocationComponentOptions#enableStaleState()} is available for disabling this behaviour.
 */
class StaleStateManager {

  private boolean isEnabled;
  private final OnLocationStaleListener innerOnLocationStaleListeners;
  private final Handler handler;
  private boolean isStale = true;
  private long delayTime;

  StaleStateManager(OnLocationStaleListener innerListener, LocationComponentOptions options) {
    innerOnLocationStaleListeners = innerListener;
    handler = new Handler();
    isEnabled = options.enableStaleState();
    delayTime = options.staleStateTimeout();
  }

  private Runnable staleStateRunnable = new Runnable() {
    @Override
    public void run() {
      setState(true);
    }
  };

  void setEnabled(boolean enabled) {
    if (enabled) {
      setState(isStale);
    } else if (isEnabled) {
      onStop();
      innerOnLocationStaleListeners.onStaleStateChange(false);
    }
    isEnabled = enabled;
  }

  boolean isStale() {
    return isStale;
  }

  void updateLatestLocationTime() {
    setState(false);
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

  private void setState(boolean stale) {
    if (stale != isStale) {
      isStale = stale;
      if (isEnabled) {
        innerOnLocationStaleListeners.onStaleStateChange(stale);
      }
    }
  }
}
