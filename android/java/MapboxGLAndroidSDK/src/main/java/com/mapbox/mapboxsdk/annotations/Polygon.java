package com.mapbox.mapboxsdk.annotations;


import android.graphics.Color;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public final class Polygon extends MultiPoint {

    private int fillColor = Color.BLACK; // default fillColor is black
    private float strokeAlpha = 1;
    private int strokeColor = Color.BLACK; // default strokeColor is black
    private float strokeWidth = 10; // As specified by Google API Docs (in pixels)

    private List<List<LatLng>> holes;

    Polygon() {
        super();
        holes = new ArrayList<>();
    }

    public int getFillColor() {
        return fillColor;
    }

    List<List<LatLng>> getHoles () {
        return new ArrayList<>(holes);
    }

    public int getStrokeColor() {
        return strokeColor;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     * @return stroke width as float
     */
    float getStrokeWidth() {
        return strokeWidth;
    }

    private void setFillAlpha(float alpha) {
        this.setAlpha(alpha);
    }

    void setFillColor(int color) {
        fillColor = color;
    }

    /**
     * Sets the holes of this polygon. This method will take a copy of the holes,
     * so further mutations to holes parameter will have no effect on this polygon.
     *
     * @param holes
     */
    void setHoles(List<? extends List<LatLng>> holes) {
        this.holes = new ArrayList<>();
        for (List<LatLng> hole : holes) {
            this.holes.add(new ArrayList<>(hole));
        }
    }

    void addHole(List<LatLng> hole) {
        holes.add(hole);
    }

    /**
     * Sets the alpha (opacity) of the stroke
     *
     * UNIMPLEMENTED: Needs implementation in Native.
     */
    void setStrokeAlpha(float alpha) {
        strokeAlpha = alpha;
    }

    void setStrokeColor(int color) {
        strokeColor = color;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     */
    void setStrokeWidth(float width) {
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
