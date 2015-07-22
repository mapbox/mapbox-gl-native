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

    public int getFillColor() {
        return fillColor;
    }

    public List<List<LatLng>> getHoles () {
        return holes;
    }

    public int getStrokeColor() {
        return strokeColor;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     * @return stroke width as float
     */
    public float getStrokeWidth() {
        return strokeWidth;
    }

    public void setFillAlpha(float alpha) {
        this.alpha = alpha;
    }

    public void setFillColor(int color) {
        fillColor = color;
    }

    /**
     * Sets the holes of this polygon. This method will take a copy of the holes,
     * so further mutations to holes parameter will have no effect on this polygon.
     *
     * @param holes
     */
    public void setHoles(List<? extends List<LatLng>> holes) {
        this.holes = new ArrayList<>();
        for (List<LatLng> hole : holes) {
            this.holes.add(new ArrayList<>(hole));
        }
    }

    /**
     * Sets the alpha (opacity) of the stroke
     *
     * UNIMPLEMENTED: Needs implementation in Native.
     */
    public void setStrokeAlpha(float alpha) {
        strokeAlpha = alpha;
    }

    public void setStrokeColor(int color) {
        strokeColor = color;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     * @return stroke width as float
     */
    public void setStrokeWidth(float width) {
        strokeWidth = width;
    }


    // TODO: Implement equals of Google Maps Android API
//    public boolean equals (Object other) {
//
//    }

    // TODO: Implement hashCode of Google Maps Android API
//    public int hashCode () {
//
//    }
}
