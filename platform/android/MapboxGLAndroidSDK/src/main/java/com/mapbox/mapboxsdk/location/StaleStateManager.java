package com.mapbox.mapboxsdk.location;

import android.os.Handler;
import android.os.Message;
import androidx.annotation.NonNull;

import java.lang.ref.WeakReference;

/**
 * Class controls the location stale state when the {@link android.location.Location} hasn't
 * been updated in 'x' amount of time. {@link LocationComponentOptions#staleStateTimeout()} can be used to
 * control the amount of time before the location's considered stale.
 * {@link LocationComponentOptions#enableStaleState()} is available for disabling this behaviour.
 */
class StaleStateManager {

  private boolean isEnabled;
  private final OnLocationStaleListener innerOnLocationStaleListeners;
  @NonNull
  private final StaleMessageHandler handler;
  private boolean isStale = true;
  private long delayTime;

  private final int staleStateMessage = 1;

  StaleStateManager(OnLocationStaleListener innerListener, LocationComponentOptions options) {
    innerOnLocationStaleListeners = innerListener;
    handler = new StaleMessageHandler(this);
    isEnabled = options.enableStaleState();
    delayTime = options.staleStateTimeout();
  }

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
    if (handler.hasMessages(staleStateMessage)) {
      postTheCallback();
    }
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
    handler.sendEmptyMessageDelayed(staleStateMessage, delayTime);
  }

  private void setState(boolean stale) {
    if (stale != isStale) {
      isStale = stale;
      if (isEnabled) {
        innerOnLocationStaleListeners.onStaleStateChange(stale);
      }
    }
  }

  private static class StaleMessageHandler extends Handler {

    private final WeakReference<StaleStateManager> managerWeakReference;

    private StaleMessageHandler(StaleStateManager staleStateManager) {
      this.managerWeakReference = new WeakReference<>(staleStateManager);
    }

    @Override
    public void handleMessage(Message msg) {
      StaleStateManager manager = managerWeakReference.get();
      if (manager != null) {
        manager.setState(true);
      }
    }
  }
}
