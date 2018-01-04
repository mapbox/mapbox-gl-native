package com.mapbox.mapboxsdk.utils;

import android.animation.Animator;
import android.animation.AnimatorInflater;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.support.annotation.AnimatorRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.view.animation.FastOutSlowInInterpolator;
import android.view.View;

/**
 * Animator utility class.
 */
public class AnimatorUtils {

  /**
   * Animate a view from an animator resource.
   *
   * @param view        the view to be animated
   * @param animatorRes the animator resource to be loaded
   * @param listener    the animator end listener
   */
  public static void animate(@NonNull final View view, @AnimatorRes int animatorRes,
                             @Nullable OnAnimationEndListener listener) {
    animate(view, animatorRes, -1, listener);
  }

  /**
   * Animate a view from an animator resource.
   *
   * @param view        the view to be animated
   * @param animatorRes the animator resource to be loaded
   * @param duration    the duration of the animator
   * @param listener    the animator end listener
   */
  public static void animate(final View view, @AnimatorRes int animatorRes, int duration,
                             @Nullable final OnAnimationEndListener listener) {
    if (view == null) {
      return;
    }

    view.setLayerType(View.LAYER_TYPE_HARDWARE, null);
    Animator animator = AnimatorInflater.loadAnimator(view.getContext(), animatorRes);
    if (duration != -1) {
      animator.setDuration(duration);
    }

    animator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationEnd(Animator animation) {
        super.onAnimationEnd(animation);
        view.setLayerType(View.LAYER_TYPE_NONE, null);
        if (listener != null) {
          listener.onAnimationEnd();
        }
      }
    });
    animator.setTarget(view);
    animator.start();
  }

  /**
   * Animate a view from an animator resource.
   *
   * @param view        the view to be animated
   * @param animatorRes the animator resource to be loaded
   */
  public static void animate(@NonNull final View view, @AnimatorRes int animatorRes) {
    animate(view, animatorRes, -1);
  }

  /**
   * Animate a view from an animator resource.
   *
   * @param view        the view to be animated
   * @param animatorRes the animator resource to be loaded
   * @param duration    the duration of the animator
   */
  public static void animate(@NonNull final View view, @AnimatorRes int animatorRes, int duration) {
    animate(view, animatorRes, duration, null);
  }

  /**
   * Animate a view rotation property to a value.
   *
   * @param view     the view to be rotated
   * @param rotation the value to animate to
   */
  public static void rotate(@NonNull final View view, float rotation) {
    view.setLayerType(View.LAYER_TYPE_HARDWARE, null);
    ObjectAnimator rotateAnimator = ObjectAnimator.ofFloat(view, View.ROTATION, view.getRotation(), rotation);
    rotateAnimator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationEnd(Animator animation) {
        super.onAnimationEnd(animation);
        view.setLayerType(View.LAYER_TYPE_NONE, null);
      }
    });
    rotateAnimator.start();
  }

  /**
   * Animate a view rotation property by a value.
   *
   * @param view       the view to be rotated
   * @param rotationBy the value to animate by
   */
  public static void rotateBy(@NonNull final View view, float rotationBy) {
    view.setLayerType(View.LAYER_TYPE_HARDWARE, null);
    view.animate().rotationBy(rotationBy).setInterpolator(new FastOutSlowInInterpolator()).setListener(
      new AnimatorListenerAdapter() {
        @Override
        public void onAnimationEnd(Animator animation) {
          super.onAnimationEnd(animation);
          view.setLayerType(View.LAYER_TYPE_NONE, null);
        }
      });
  }

  /**
   * Animate a view alpha property to a value.
   *
   * @param convertView the view to be animated
   * @param alpha       the value to animate to
   * @param listener    the animator end listener
   */
  public static void alpha(@NonNull final View convertView, float alpha,
                           @Nullable final OnAnimationEndListener listener) {
    convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
    ObjectAnimator rotateAnimator = ObjectAnimator.ofFloat(convertView, View.ALPHA, convertView.getAlpha(), alpha);
    rotateAnimator.addListener(new AnimatorListenerAdapter() {
      @Override
      public void onAnimationStart(Animator animation) {
        super.onAnimationStart(animation);
        convertView.setVisibility(View.VISIBLE);
      }

      @Override
      public void onAnimationEnd(Animator animation) {
        super.onAnimationEnd(animation);
        convertView.setLayerType(View.LAYER_TYPE_NONE, null);
        if (listener != null) {
          listener.onAnimationEnd();
        }
      }
    });
    rotateAnimator.start();
  }

  /**
   * Animate a view alpha property to a value.
   *
   * @param convertView the view to be animated
   * @param alpha       the value to animate to
   */
  public static void alpha(@NonNull final View convertView, float alpha) {
    alpha(convertView, alpha, null);
  }

  /**
   * An interface definition that is invoked when an animation ends.
   */
  public interface OnAnimationEndListener {
    void onAnimationEnd();
  }
}
