package com.mapbox.mapboxsdk.maps.widgets;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;

/**
 * UI element overlaid on a map to show the map scale.
 */

public final class ScaleView extends TextView {

    float width = 0.34f;
    UiSettings.ScaleUnit scaleUnit = UiSettings.ScaleUnit.KM;
    private FrameLayout.LayoutParams layoutParams;

    public ScaleView(Context context) {
        super(context);
        initialize();
    }

    public ScaleView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize();
    }

    public ScaleView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize();
    }

    private void initialize() {
        setBackgroundResource(R.drawable.bg_scale_black);
        setGravity(Gravity.CENTER_HORIZONTAL | Gravity.BOTTOM);
    }

    public void setWidth(float width) {
        this.width = width;
    }

    public void setScaleUnit(UiSettings.ScaleUnit scaleUnit) {
        this.scaleUnit = scaleUnit;
    }

    public void setColor(int color) {
        switch(color) {
            case MapboxConstants.SCALE_COLOR_BLACK:
                setBackgroundResource(R.drawable.bg_scale_black);
                setTextColor(Color.BLACK);
                break;

            case MapboxConstants.SCALE_COLOR_WHITE:
                setBackgroundResource(R.drawable.bg_scale_white);
                setTextColor(Color.WHITE);
                break;
        }
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        setVisibility(enabled ? VISIBLE : INVISIBLE);
    }

    @Override
    public void setLayoutParams(ViewGroup.LayoutParams params) {
        super.setLayoutParams(params);
        layoutParams = (FrameLayout.LayoutParams)params;
    }

    public void update(MapView mapView, Projection projection) {
        LatLngBounds bounds = projection.getVisibleRegion().latLngBounds;
        double metersPerPixel;
        if ((layoutParams.gravity & Gravity.TOP) != 0)
            metersPerPixel = projection.getMetersPerPixelAtLatitude(bounds.getLatNorth());
        else if ((layoutParams.gravity & Gravity.BOTTOM) != 0)
            metersPerPixel = projection.getMetersPerPixelAtLatitude(bounds.getLatSouth());
        else
            metersPerPixel = projection.getMetersPerPixelAtLatitude((bounds.getLatSouth() + bounds.getLatNorth()) / 2);
        int parentWidth = mapView.getWidth();
        float totalWidth = (float)(parentWidth * metersPerPixel / scaleUnit.ratio);
        // calculate an initial guess at step size
        float tempStep = totalWidth * width;

        // get the magnitude of the step size
        float mag = (float)(Math.log10(tempStep));
        int digits = (int)Math.floor(-mag + 2);
        mag = (float)Math.pow(10, Math.floor(mag));

        float magMsd = (tempStep / mag);
        if (magMsd > 7.5f) {
            magMsd = 10.0f;
            digits--;
        } else if (magMsd > 5f)
            magMsd = 7.5f;
        else if (magMsd > 4f)
            magMsd = 5f;
        else if (magMsd > 3f)
            magMsd = 4f;
        else if (magMsd > 2)
            magMsd = 3f;
        else if (magMsd > 1.5f)
            magMsd = 2f;
        else if (magMsd > 1f)
            magMsd = 1.5f;
        else
            magMsd = 1f;
        float length = magMsd * mag;
        if (digits < 0)
            digits = 0;
        setText(String.format(MapboxConstants.MAPBOX_LOCALE, "%." + digits + "f %s", length, scaleUnit.unit));
        // set the total width to the appropriate fraction of the display
        layoutParams.width = Math.round(parentWidth * length / totalWidth);
        setLayoutParams(layoutParams);
    }
}
