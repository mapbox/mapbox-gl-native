package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.graphics.PointF;
import android.support.annotation.AnimatorRes;
import android.support.annotation.NonNull;

public class MarkerViewSettings {

    private PointF centerOffset;
    private Point infoWindowOffset;
    private boolean flat;

    private int selectAnimRes;
    private int deselectAnimRes;

    private float tiltValue;

    MarkerViewSettings(PointF centerOffset, Point infoWindowOffset, boolean flat, int selectAnimRes, int deselectAnimRes) {
        this.centerOffset = centerOffset;
        this.infoWindowOffset = infoWindowOffset;
        this.flat = flat;
        this.selectAnimRes = selectAnimRes;
        this.deselectAnimRes = deselectAnimRes;
    }

    @NonNull
    public PointF getCenterOffset() {
        return centerOffset;
    }

    @NonNull
    public Point getInfoWindowOffset() {
        return infoWindowOffset;
    }

    @AnimatorRes
    public int getSelectAnimRes() {
        return selectAnimRes;
    }

    @AnimatorRes
    public int getDeselectAnimRes() {
        return deselectAnimRes;
    }

    public boolean isFlat() {
        return flat;
    }

    public void setTiltValue(float tilt) {
        tiltValue = tilt;
    }

    public float getTiltValue() {
        return tiltValue;
    }

    /**
     * Builder for composing LatLngBounds objects.
     */
    public static final class Builder {

        private PointF centerOffset;
        private Point infoWindowOffset;
        private boolean flat;

        private int animSelectRes;
        private int animDeselectRes;

        public Builder() {
            centerOffset = new PointF();
            infoWindowOffset = new Point();
        }

        public Builder centerOffset(float dx, float dy) {
            centerOffset.offset(dx, dy);
            return this;
        }

        public Builder infoWindowOffset(int dx, int dy) {
            infoWindowOffset.offset(dx, -dy);
            return this;
        }

        public Builder flat(boolean isFlat) {
            flat = isFlat;
            return this;
        }

        public Builder animSelectRes(@AnimatorRes int animatorRes) {
            animSelectRes = animatorRes;
            return this;
        }

        public Builder animDeselectRes(@AnimatorRes int animatorRes) {
            animDeselectRes = animatorRes;
            return this;
        }

        public MarkerViewSettings build() {
            return new MarkerViewSettings(centerOffset, infoWindowOffset, flat, animSelectRes, animDeselectRes);
        }
    }
}
