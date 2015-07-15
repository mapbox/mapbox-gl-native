package com.mapbox.mapboxgl.annotations;


import android.graphics.Color;

import com.mapbox.mapboxgl.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public class Polygon extends MultiPoint {

    int fillColor = Color.BLACK; // default fillColor is black
    float strokeAlpha = 1;
    int strokeColor = Color.BLACK; // default strokeColor is black
    float strokeWidth = 10; // As specified by Google API Docs (in pixels)

    List<List<LatLng>> holes;

    public Polygon() {
        super();
        holes = new ArrayList<>();
    }

    // TODO: Implement equals of Google Maps Android API
//    public boolean equals (Object other) {
//
//    }

    public int getFillColor() {
        return fillColor;
    }

    public List<List<LatLng>> getHoles () {
        return holes;
    }

    public int getStrokeColor() {
        return strokeColor;
    }

    public float getStrokeWidth() {
        return strokeWidth;
    }

    // TODO: Implement getZIndex of Google Maps Android API
//    public float getZIndex() {
//
//    }

    // TODO: Implement hashCode of Google Maps Android API
//    public int hashCode () {
//
//    }

    public void setFillAlpha(float alpha) {
        this.alpha = alpha;
    }

    public void setFillColor(int color) {
        fillColor = color;
    }

    /**
     * Sets the holes of this polygon. This method will take a copy of the holes,
     * so further mutations to holes will have no effect on this polygon.
     *
     * @param holes
     */
    public void setHoles(List<? extends List<LatLng>> holes) {
        this.holes = new ArrayList<>();
        for (List<LatLng> hole : holes) {
            List<LatLng> newHole = new ArrayList<>();
            this.holes.add(newHole);
            for (LatLng latLng : hole) {
                newHole.add(latLng);
            }
        }
    }

    /**
     * Sets the alpha (opacity) of the stroke
     */
    public void setStrokeAlpha(float alpha) {
        strokeAlpha = alpha;
    }

    public void setStrokeColor(int color) {
        strokeColor = color;
    }

    public void setStrokeWidth(float width) {
        strokeWidth = width;
    }

    // TODO: Implement setZIndex of Google Maps Android API
//    public void setZIndex(float zIndex) {
//
//    }
}
