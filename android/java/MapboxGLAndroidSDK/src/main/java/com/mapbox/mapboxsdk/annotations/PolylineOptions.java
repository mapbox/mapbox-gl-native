package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class PolylineOptions extends MultiPointOptions {

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

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return
     */
    public PolylineOptions width(float width) {
        ((Polyline)annotation).width = width;
        return this;
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

}
