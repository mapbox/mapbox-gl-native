package com.mapbox.mapboxsdk.annotations;


import android.graphics.Color;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * UNIMPLEMENTED: Needs implementation in Native.
 *
 * https://github.com/mapbox/mapbox-gl-native/issues/1882
 * https://github.com/mapbox/mapbox-gl-native/issues/1726
 */
public class Circle extends Annotation {

    LatLng center;
    int fillColor = Color.BLACK;
    double radius;
    int strokeColor = Color.BLACK;
    float strokeWidth = 10; // Google Maps API defaults to 10

    public LatLng getCenter() {
        return center;
    }

    public int getFillColor() {
        return fillColor;
    }

    /**
     * Returns the circle's radius, in meters.
     *
     * @return radius in meters
     */
    public double getRadius() {
        return radius;
    }

    public int getStrokeColor() {
        return strokeColor;
    }

    public float getStrokeWidth() {
        return strokeWidth;
    }

    public void setCenter(LatLng center) {
        this.center = center;
    }

    public void setFillColor(int color) {
        fillColor = color;
    }

    public void setRadius(double radius) {
        this.radius = radius;
    }

    public void setStrokeColor (int color) {
        strokeColor = color;
    }

    public void setStrokeWidth (float width) {
        strokeWidth = width;
    }

}
