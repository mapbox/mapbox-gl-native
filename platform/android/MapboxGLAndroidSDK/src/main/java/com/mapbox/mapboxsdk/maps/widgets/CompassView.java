package com.mapbox.mapboxsdk.maps.widgets;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.support.annotation.NonNull;
import android.support.v4.view.ViewCompat;
import android.support.v4.view.ViewPropertyAnimatorCompat;
import android.support.v4.view.ViewPropertyAnimatorListenerAdapter;
import android.support.v7.widget.AppCompatImageView;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * UI element overlaid on a map to show the map's bearing when it isn't true north (0.0). Tapping
 * the compass resets the bearing to true north and hides the compass.
 * <p>
 * You can change the behaviour of this View during initialisation with
 * {@link com.mapbox.mapboxsdk.maps.MapboxMapOptions}, and xml attributes. While running you can
 * use {@link com.mapbox.mapboxsdk.maps.UiSettings}.
 * </p>
 */
public final class CompassView extends AppCompatImageView implements Runnable {

  public static final long TIME_WAIT_IDLE = 500;
  public static final long TIME_MAP_NORTH_ANIMATION = 150;
  private static final long TIME_FADE_ANIMATION = TIME_WAIT_IDLE;

  private float rotation = 0.0f;
  private boolean fadeCompassViewFacingNorth = true;
  private ViewPropertyAnimatorCompat fadeAnimator;
  private MapboxMap.OnCompassAnimationListener compassAnimationListener;
  private boolean isAnimating = false;

  public CompassView(Context context) {
    super(context);
    initialize(context);
  }

  public CompassView(Context context, AttributeSet attrs) {
    super(context, attrs);
    initialize(context);
  }

  public CompassView(Context context, AttributeSet attrs, int defStyleAttr) {
    super(context, attrs, defStyleAttr);
    initialize(context);
  }

  private void initialize(Context context) {
    setEnabled(false);

    // Layout params
    float screenDensity = context.getResources().getDisplayMetrics().density;
    ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams((int) (48 * screenDensity), (int) (48 * screenDensity));
    setLayoutParams(lp);
  }

  public void injectCompassAnimationListener(@NonNull MapboxMap.OnCompassAnimationListener compassAnimationListener) {
    this.compassAnimationListener = compassAnimationListener;
  }

  public void isAnimating(boolean isAnimating) {
    this.isAnimating = isAnimating;
  }

  private void resetAnimation() {
    if (fadeAnimator != null) {
      fadeAnimator.cancel();
    }
    fadeAnimator = null;
  }

  public boolean isHidden() {
    return fadeCompassViewFacingNorth && isFacingNorth();
  }

  public boolean isFacingNorth() {
    // increase range of facing north to more than only 0.0
    return Math.abs(rotation) >= 359.0 || Math.abs(rotation) <= 1.0;
  }

  @Override
  public void setEnabled(boolean enabled) {
    super.setEnabled(enabled);
    if (enabled && !isHidden()) {
      resetAnimation();
      setAlpha(1.0f);
      setVisibility(View.VISIBLE);
    } else {
      resetAnimation();
      setAlpha(0.0f);
      setVisibility(View.INVISIBLE);
    }
  }

  /**
   * Updates the direction of the compass.
   *
   * @param bearing the direction value of the map
   */
  public void update(final double bearing) {
    rotation = (float) bearing;

    if (!isEnabled()) {
      return;
    }

    if (isHidden()) {
      if (getVisibility() == View.INVISIBLE || fadeAnimator != null) {
        return;
      }
      postDelayed(this, TIME_WAIT_IDLE);
      return;
    } else {
      resetAnimation();
      setAlpha(1.0f);
      setVisibility(View.VISIBLE);
    }

    notifyCompassAnimationListenerWhenAnimating();
    setRotation(rotation);
  }

  public void fadeCompassViewFacingNorth(boolean compassFadeFacingNorth) {
    fadeCompassViewFacingNorth = compassFadeFacingNorth;
  }

  public boolean isFadeCompassViewFacingNorth() {
    return fadeCompassViewFacingNorth;
  }

  /**
   * Set the CompassView image.
   *
   * @param compass the drawable to use as compass image
   */
  public void setCompassImage(Drawable compass) {
    setImageDrawable(compass);
  }

  /**
   * Get the current configured CompassView image.
   *
   * @return the drawable used as compass image
   */
  public Drawable getCompassImage() {
    return getDrawable();
  }

  @Override
  public void run() {
    if (isHidden()) {
      compassAnimationListener.onCompassAnimationFinished();
      resetAnimation();
      setLayerType(View.LAYER_TYPE_HARDWARE, null);
      fadeAnimator = ViewCompat.animate(CompassView.this).alpha(0.0f).setDuration(TIME_FADE_ANIMATION);
      fadeAnimator.setListener(new ViewPropertyAnimatorListenerAdapter() {
        @Override
        public void onAnimationEnd(View view) {
          setLayerType(LAYER_TYPE_NONE, null);
          setVisibility(View.INVISIBLE);
          resetAnimation();
        }
      });
    }
  }

  private void notifyCompassAnimationListenerWhenAnimating() {
    if (isAnimating) {
      compassAnimationListener.onCompassAnimation();
    }
  }
}
