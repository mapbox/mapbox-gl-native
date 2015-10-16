package com.mapbox.mapboxsdk.annotations;


import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * UNIMPLEMENTED: Needs implementation in Native.
 *
 * https://github.com/mapbox/mapbox-gl-native/issues/1882
 * https://github.com/mapbox/mapbox-gl-native/issues/1726
 */
class CircleOptions {

    private Circle circle;

    public CircleOptions() {
        circle = new Circle();
    }
    public CircleOptions center(LatLng center) {
        circle.setCenter(center);;
        return this;
    }

    public CircleOptions fillColor(int color) {
        circle.setFillColor(color);
        return this;
    }

    public LatLng getCenter() {
        return circle.getCenter();
    }

    public int getFillColor() {
        return circle.getFillColor();
    }

    public double getRadius() {
        return circle.getRadius();
    }

    public int getStrokeColor () {
        return circle.getStrokeColor();
    }

    public float getStrokeWidth() {
        return circle.getStrokeWidth();
    }

    public CircleOptions radius(double radius) {
        circle.setRadius(radius);
        return this;
    }

    public CircleOptions strokeColor(int color) {
        circle.setStrokeColor(color);
        return this;
    }

    public CircleOptions strokeWidth (float width) {
        circle.setStrokeWidth(width);
        return this;
    }

    public CircleOptions alpha(float alpha) {
        circle.setAlpha(alpha);
        return this;
    }

    public float getAlpha() {
        return circle.getAlpha();
    }

    private CircleOptions visible(boolean visible) {
        circle.setVisible(visible);
        return this;
    }

    private boolean isVisible() {
        return circle.isVisible();
    }
}
