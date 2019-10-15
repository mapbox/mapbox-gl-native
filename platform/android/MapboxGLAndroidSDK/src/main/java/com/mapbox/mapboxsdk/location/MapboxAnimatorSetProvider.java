package com.mapbox.mapboxsdk.location;

import android.animation.Animator;
import android.animation.AnimatorSet;
import androidx.annotation.NonNull;
import android.view.animation.Interpolator;

import java.util.List;

class MapboxAnimatorSetProvider {
  private static MapboxAnimatorSetProvider instance;

  private MapboxAnimatorSetProvider() {
    // private constructor
  }

  static MapboxAnimatorSetProvider getInstance() {
    if (instance == null) {
      instance = new MapboxAnimatorSetProvider();
    }
    return instance;
  }

  void startAnimation(@NonNull List<Animator> animators, @NonNull Interpolator interpolator,
                      long duration) {
    AnimatorSet locationAnimatorSet = new AnimatorSet();
    locationAnimatorSet.playTogether(animators);
    locationAnimatorSet.setInterpolator(interpolator);
    locationAnimatorSet.setDuration(duration);
    locationAnimatorSet.start();
  }
}
