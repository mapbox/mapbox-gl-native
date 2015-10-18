package com.mapbox.mapboxsdk.views;

import android.content.Context;
import android.hardware.GeomagneticField;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import com.mapbox.mapboxsdk.R;

import java.lang.ref.WeakReference;

final class CompassView extends ImageView {

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

        // Layout params
        float mScreenDensity = context.getResources().getDisplayMetrics().density;
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) (48 * mScreenDensity), (int) (48 * mScreenDensity));
        setLayoutParams(lp);
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        setVisibility(enabled ? View.VISIBLE : View.INVISIBLE);
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
