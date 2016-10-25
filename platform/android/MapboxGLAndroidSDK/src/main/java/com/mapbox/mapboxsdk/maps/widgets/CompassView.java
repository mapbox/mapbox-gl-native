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
import java.util.Timer;
import java.util.TimerTask;

/**
 * UI element overlaid on a map to show the map's bearing
 * when it isn't true north (0.0). Tapping the compass resets the bearing to true
 * north and hides the compass.
 */
public final class CompassView extends ImageView {

    private Timer northTimer;
    private double direction = 0.0f;
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

        // View configuration
        setImageDrawable(ContextCompat.getDrawable(getContext(), R.drawable.compass));
        setContentDescription(getResources().getString(R.string.compassContentDescription));
        setEnabled(false);

        // Layout params
        float screenDensity = context.getResources().getDisplayMetrics().density;
        ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams((int) (48 * screenDensity), (int) (48 * screenDensity));
        setLayoutParams(lp);
    }

    public void setMapboxMap(@NonNull MapboxMap mapboxMap){
        setOnClickListener(new CompassClickListener(mapboxMap));
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        if (enabled) {
            if (direction != 0.0) {
                if (northTimer != null) {
                    northTimer.cancel();
                    northTimer = null;
                }
                if (fadeAnimator != null) {
                    fadeAnimator.cancel();
                }
                fadeAnimator = null;
                setAlpha(1.0f);
                setVisibility(View.VISIBLE);
            }
        } else {
            if (northTimer != null) {
                northTimer.cancel();
                northTimer = null;
            }
            if (fadeAnimator != null) {
                fadeAnimator.cancel();
            }
            fadeAnimator = null;
            setVisibility(View.INVISIBLE);
        }
    }

    public void update(double direction) {
        this.direction = direction;
        setRotation((float) direction);

        if (!isEnabled()) {
            return;
        }

        if (direction == 0.0) {
            if (getVisibility() == View.INVISIBLE) {
                return;
            }

            if (northTimer == null) {
                if (fadeAnimator != null) {
                    fadeAnimator.cancel();
                }
                fadeAnimator = null;

                northTimer = new Timer("CompassView North timer");
                northTimer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        post(new Runnable() {
                            @Override
                            public void run() {
                                setAlpha(1.0f);
                                fadeAnimator = ViewCompat.animate(CompassView.this).alpha(0.0f).setDuration(1000).withLayer();
                                fadeAnimator.setListener(new ViewPropertyAnimatorListenerAdapter() {
                                    @Override
                                    public void onAnimationEnd(View view) {
                                        setVisibility(View.INVISIBLE);
                                        northTimer = null;
                                    }
                                });
                            }
                        });
                    }
                }, 1000);
            }
        } else {
            if (northTimer != null) {
                northTimer.cancel();
                northTimer = null;
            }
            if (fadeAnimator != null) {
                fadeAnimator.cancel();
            }
            setAlpha(1.0f);
            setVisibility(View.VISIBLE);
        }
    }

    static class CompassClickListener implements View.OnClickListener {

        private WeakReference<MapboxMap> mapboxMap;

        public CompassClickListener(final MapboxMap mapboxMap) {
            this.mapboxMap = new WeakReference<>(mapboxMap);
        }

        @Override
        public void onClick(View view) {
            final MapboxMap mapboxMap = this.mapboxMap.get();
            if (mapboxMap != null) {
                mapboxMap.resetNorth();
            }
        }
    }
}
