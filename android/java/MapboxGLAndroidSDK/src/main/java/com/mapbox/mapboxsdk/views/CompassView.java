package com.mapbox.mapboxsdk.views;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.content.Context;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewPropertyAnimator;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.TimerTask;

final class CompassView extends ImageView {

    private Timer mNorthTimer;
    private double mDirection = 0.0f;
    private ViewPropertyAnimator mFadeAnimator;

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
        float mScreenDensity = context.getResources().getDisplayMetrics().density;
        ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams((int) (48 * mScreenDensity), (int) (48 * mScreenDensity));
        setLayoutParams(lp);
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        if (enabled) {
            if (mDirection != 0.0) {
                if (mNorthTimer != null){
                    mNorthTimer.cancel();
                    mNorthTimer = null;
                }
                if (mFadeAnimator != null) {
                    mFadeAnimator.cancel();
                }
                mFadeAnimator = null;
                setAlpha(1.0f);
                setVisibility(View.VISIBLE);
            }
        } else {
            if (mNorthTimer != null){
                mNorthTimer.cancel();
                mNorthTimer = null;
            }
            if (mFadeAnimator != null) {
                mFadeAnimator.cancel();
            }
            mFadeAnimator = null;
            setVisibility(View.INVISIBLE);
        }
    }

    public void update(double direction) {
        mDirection = direction;
        setRotation((float) direction);

        if (direction == 0.0) {
            if (getVisibility() == View.INVISIBLE) {
                return;
            }

            if (mNorthTimer == null) {
                if (mFadeAnimator != null) {
                    mFadeAnimator.cancel();
                }
                mFadeAnimator = null;

                mNorthTimer = new Timer("CompassView North timer");
                mNorthTimer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        post(new Runnable() {
                            @Override
                            public void run() {
                                setAlpha(1.0f);
                                mFadeAnimator = animate().alpha(0.0f).setDuration(1000);
                                mFadeAnimator.setListener(new AnimatorListenerAdapter() {
                                    @Override
                                    public void onAnimationEnd(Animator animation) {
                                        setVisibility(View.INVISIBLE);
                                        mNorthTimer = null;
                                    }
                                });
                            }
                        });
                    }
                }, 1000);
            }
        } else {
            if (mNorthTimer != null){
                mNorthTimer.cancel();
                mNorthTimer = null;
            }
            if (mFadeAnimator != null) {
                mFadeAnimator.cancel();
            }
            setAlpha(1.0f);
            setVisibility(View.VISIBLE);
        }
    }

    public static class CompassClickListener implements View.OnClickListener {

        private WeakReference<MapView> mMapView;

        public CompassClickListener(final MapView mapView) {
            mMapView = new WeakReference<>(mapView);
        }

        @Override
        public void onClick(View v) {
            final MapView mapView = mMapView.get();
            if (mapView != null) {
                mapView.resetNorth();
            }
        }
    }
}
