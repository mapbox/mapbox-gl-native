package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

public class PolylineOptions {

    protected Annotation annotation;

    public PolylineOptions() {
        annotation = new Polyline();
    }

    public PolylineOptions add(LatLng point) {
        ((MultiPoint)annotation).points.add(point);
        return this;
    }

    public PolylineOptions add(LatLng... points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolylineOptions addAll(Iterable<LatLng> points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolylineOptions alpha(float alpha) {
        annotation.alpha = alpha;
        return this;
    }

    public float getAlpha() {
        return annotation.alpha;
    }

    /**
     * Sets the color of the polyline.
     *
     * @param color - the color in ARGB format
     */
    public PolylineOptions color(int color) {
        ((Polyline)annotation).color = color;
        return this;
    }

    public int getColor() {
        return ((Polyline)annotation).color;
    }

    public Polyline getPolyline() {
        return ((Polyline)annotation);
    }

    public float getWidth() {
        return ((Polyline)annotation).width;
    }

    public PolylineOptions visible(boolean visible) {
        annotation.visible = visible;
        return this;
    }

    public boolean isVisible() {
        return annotation.visible;
    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return a new PolylineOptions
     */
    public PolylineOptions width(float width) {
        ((Polyline)annotation).width = width;
        return this;
    }

    public List<LatLng> getPoints() {
        // the getter gives us a copy, which is the safe thing to do...
        return ((MultiPoint)annotation).getPoints();
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

}
