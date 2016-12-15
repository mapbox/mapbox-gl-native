package com.mapbox.mapboxsdk.maps.widgets;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.ViewCompat;
import android.support.v4.view.ViewPropertyAnimatorCompat;
import android.support.v4.view.ViewPropertyAnimatorListenerAdapter;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.lang.ref.WeakReference;

/**
 * UI element overlaid on a map to show the map's bearing when it isn't true north (0.0). Tapping
 * the compass resets the bearing to true north and hides the compass.
 * <p>
 * You can change the behaviour of this View during initialisation with
 * {@link com.mapbox.mapboxsdk.maps.MapboxMapOptions}, and xml attributes. While running you can
 * use {@link com.mapbox.mapboxsdk.maps.UiSettings}.
 * </p>
 */
public final class CompassView extends ImageView implements Runnable {

  private static final long TIME_WAIT_IDLE = 500;
  private static final long TIME_FADE_ANIMATION = TIME_WAIT_IDLE;
  private static final long TIME_MAP_NORTH_ANIMATION = 150;

  private double direction = 0.0;
  private boolean fadeCompassViewFacingNorth = true;
  private ViewPropertyAnimatorCompat fadeAnimator;

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
    setImageDrawable(ContextCompat.getDrawable(getContext(), R.drawable.mapbox_compass_icon));
    setContentDescription(getResources().getString(R.string.mapbox_compassContentDescription));
    setEnabled(false);

    // Layout params
    float screenDensity = context.getResources().getDisplayMetrics().density;
    ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams((int) (48 * screenDensity), (int) (48 * screenDensity));
    setLayoutParams(lp);
  }

  // TODO refactor MapboxMap and replace with interface
  public void setMapboxMap(@NonNull MapboxMap mapboxMap) {
    setOnClickListener(new CompassClickListener(mapboxMap, this));
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
    // increase range more than just 0.0
    return direction >= 359.0 || direction <= 1.0;
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

  public void update(final double direction) {
    this.direction = direction;

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

    setRotation((float) direction);
  }

  public void fadeCompassViewFacingNorth(boolean compassFadeFacingNorth) {
    fadeCompassViewFacingNorth = compassFadeFacingNorth;
  }

  public boolean isFadeCompassViewFacingNorth() {
    return fadeCompassViewFacingNorth;
  }

  @Override
  public void run() {
    if (isFacingNorth() && fadeCompassViewFacingNorth) {
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

  static class CompassClickListener implements View.OnClickListener {

    private WeakReference<MapboxMap> mapboxMap;
    private WeakReference<CompassView> compassView;

    CompassClickListener(final MapboxMap mapboxMap, CompassView compassView) {
      this.mapboxMap = new WeakReference<>(mapboxMap);
      this.compassView = new WeakReference<>(compassView);
    }

    @Override
    public void onClick(View view) {
      final MapboxMap mapboxMap = this.mapboxMap.get();
      final CompassView compassView = this.compassView.get();
      if (mapboxMap != null && compassView != null) {
        mapboxMap.resetNorth();
        compassView.postDelayed(compassView, TIME_WAIT_IDLE + TIME_MAP_NORTH_ANIMATION);
      }
    }
  }
}
