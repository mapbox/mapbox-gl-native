package com.mapbox.mapboxsdk.location;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.util.List;

abstract class MapboxCameraAnimatorAdapter extends
  MapboxFloatAnimator<MapboxAnimator.OnCameraAnimationsValuesChangeListener> {
  private final MapboxMap.CancelableCallback cancelableCallback;

  MapboxCameraAnimatorAdapter(Float previous, Float target,
                              List<OnCameraAnimationsValuesChangeListener> updateListeners,
                              @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    super(previous, target, updateListeners);
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
