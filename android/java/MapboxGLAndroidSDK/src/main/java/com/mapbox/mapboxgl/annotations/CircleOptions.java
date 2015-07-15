package com.mapbox.mapboxgl.annotations;


import com.mapbox.mapboxgl.geometry.LatLng;

public class CircleOptions {

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

    // TODO: Implement getZIndex of Google Maps Android API
//    public float getZIndex () {
//
//    }

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

    // TODO: Implement zIndex of Google Maps Android API
//    public CircleOptions zIndex (float zIndex) {
//
//    }
    
}
