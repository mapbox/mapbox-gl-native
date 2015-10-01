package com.mapbox.mapboxsdk.annotations;


import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * UNIMPLEMENTED: Needs implementation in Native.
 *
 * https://github.com/mapbox/mapbox-gl-native/issues/1882
 * https://github.com/mapbox/mapbox-gl-native/issues/1726
 */
public class CircleOptions extends AnnotationOptions {

    private Circle circle;

    public CircleOptions() {
        circle = new Circle();
    }
    public CircleOptions center(LatLng center) {
        circle.center = center;
        return this;
    }

    public CircleOptions fillColor(int color) {
        circle.fillColor = color;
        return this;
    }

    public LatLng getCenter() {
        return circle.center;
    }

    public int getFillColor() {
        return circle.fillColor;
    }

    public double getRadius() {
        return circle.radius;
    }

    public int getStrokeColor () {
        return circle.strokeColor;
    }

    public float getStrokeWidth() {
        return circle.strokeWidth;
    }

    public CircleOptions radius (double radius) {
        circle.radius = radius;
        return this;
    }

    public CircleOptions strokeColor(int color) {
        circle.strokeColor = color;
        return this;
    }

    public CircleOptions strokeWidth (float width) {
        circle.strokeWidth = width;
        return this;
    }

}
