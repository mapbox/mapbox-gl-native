package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;

public final class Polyline extends MultiPoint {

    private int color = Color.BLACK; // default color is black
    private float width = 10; // As specified by Google API Docs (in pixels)

    Polyline() {
        super();
    }

    public int getColor() {
        return color;
    }

    public float getWidth() {
        return width;
    }

    /**
     * Sets the color of the polyline.
     *
     * @param color - the color in ARGB format
     */
    void setColor(int color) {
        this.color = color;
    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     */
    void setWidth(float width) {
        this.width = width;
    }
}
