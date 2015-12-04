package com.mapbox.mapboxsdk.views;

import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.RectF;
import android.view.MotionEvent;

public abstract class Overlay {

    private boolean _enabled = false;

    public abstract void drawLayer(final Canvas canvas, final RectF wgsBounds, final PointF wgsCenter);

    public abstract void onTouchEvent(final MotionEvent event);

    public void setEnabled(boolean enabled) {
        _enabled = enabled;
    }

    boolean isEnabled() {
        return _enabled;
    }
}
