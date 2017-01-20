package com.mapbox.mapboxsdk.maps.widgets.scaleview;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;

import com.mapbox.mapboxsdk.R;

class ViewConfig {

    final int color;
    final float textSize;
    final float strokeWidth;

    final int desiredWidth;


    ViewConfig(Context context, AttributeSet attrs) {
//        float fontScale = context.getResources().getDisplayMetrics().scaledDensity;
        float density = context.getResources().getDisplayMetrics().density;

        desiredWidth = (int) (100 * density);

        TypedArray a = context.getTheme().obtainStyledAttributes(attrs, R.styleable.mapbox_MapView_MapScaleView, 0, 0);
        try {
            color = a.getColor(R.styleable.mapbox_MapView_MapScaleView_mapbox_uiScale_color, Color.parseColor("#333333"));
            textSize = a.getDimension(R.styleable.mapbox_MapView_MapScaleView_mapbox_uiScale_textSize, 12 * density);
            strokeWidth = a.getDimension(R.styleable.mapbox_MapView_MapScaleView_mapbox_uiScale_strokeWidth, 1.5f * density);
        } finally {
            a.recycle();
        }

    }

}
