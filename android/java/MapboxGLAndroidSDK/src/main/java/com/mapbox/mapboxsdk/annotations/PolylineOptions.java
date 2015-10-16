package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

public final class PolylineOptions {

    private Polyline polyline;

    public PolylineOptions() {
        polyline = new Polyline();
    }

    public PolylineOptions add(LatLng point) {
        polyline.addPoint(point);
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
        polyline.setAlpha(alpha);
        return this;
    }

    public float getAlpha() {
        return polyline.getAlpha();
    }

    /**
     * Sets the color of the polyline.
     *
     * @param color - the color in ARGB format
     */
    public PolylineOptions color(int color) {
        polyline.setColor(color);
        return this;
    }

    public int getColor() {
        return polyline.getColor();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public Polyline getPolyline() {
        return polyline;
    }

    public float getWidth() {
        return polyline.getWidth();
    }

    PolylineOptions visible(boolean visible) {
        polyline.setVisible(visible);
        return this;
    }

    private boolean isVisible() {
        return polyline.isVisible();
    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return a new PolylineOptions
     */
    public PolylineOptions width(float width) {
        polyline.setWidth(width);
        return this;
    }

    public List<LatLng> getPoints() {
        // the getter gives us a copy, which is the safe thing to do...
        return polyline.getPoints();
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

}
