package com.mapbox.mapboxgl.annotations;

import android.graphics.Color;

import com.mapbox.mapboxgl.geometry.LatLng;

import java.util.List;

public class Polyline extends MultiPoint {

    int color = Color.BLACK; // default color is black
    float width = 10; // As specified by Google API Docs (in pixels)
    List<LatLng> points;

    public Polyline() {
        super();
    }

    // TODO: Implement equals of Google Maps Android API
//    public boolean equals(Object other) {
//
//    }

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

    // TODO: Implement setGeodesic of Google Maps Android API
//    public void setGeodesic(boolean geodesic) {
//
//    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return
     */
    public void setWidth(float width) {
        this.width = width;
    }

    // TODO: Implement setZIndex of Google Maps Android API
//    public void setZIndex(float zIndex) {
//
//    }

}
