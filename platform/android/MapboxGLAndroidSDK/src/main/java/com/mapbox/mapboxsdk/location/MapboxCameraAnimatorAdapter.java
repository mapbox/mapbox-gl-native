package com.mapbox.mapboxsdk.location;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import androidx.annotation.Nullable;

import com.mapbox.mapboxsdk.maps.MapboxMap;

class MapboxCameraAnimatorAdapter extends MapboxFloatAnimator {
  @Nullable
  private final MapboxMap.CancelableCallback cancelableCallback;

  MapboxCameraAnimatorAdapter(Float previous, Float target,
                              AnimationsValueChangeListener updateListener,
                              @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    super(previous, target, updateListener, Integer.MAX_VALUE);
    this.cancelableCallback = cancelableCallback;
    addListener(new MapboxAnimatorListener());
  }

  private final class MapboxAnimatorListener extends AnimatorListenerAdapter {
    @Override
    public void onAnimationCancel(Animator animation) {
      if (cancelableCallback != null) {
        cancelableCallback.onCancel();
      }
    }

    @Override
    public void onAnimationEnd(Animator animation) {
      if (cancelableCallback != null) {
        cancelableCallback.onFinish();
      }
    }
  }
}
