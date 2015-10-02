package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;

public class Polyline extends MultiPoint {

    int color = Color.BLACK; // default color is black
    float width = 10; // As specified by Google API Docs (in pixels)

    public Polyline() {
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
    public void setColor(int color) {
        this.color = color;
    }


    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return
     */
    public void setWidth(float width) {
        this.width = width;
    }

    // TODO: Implement equals of Google Maps Android API
//    public boolean equals(Object other) {
//
//    }
}
