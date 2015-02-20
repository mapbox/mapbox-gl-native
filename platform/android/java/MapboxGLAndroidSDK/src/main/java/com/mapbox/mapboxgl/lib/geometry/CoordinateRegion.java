package com.mapbox.mapboxgl.lib.geometry;

/**
 * Implementation of iOS MKCoordinateRegion
 */
public class CoordinateRegion {
    private LatLng center;
    private CoordinateSpan span;

    public CoordinateRegion(LatLng center, CoordinateSpan span) {
        this.center = center;
        this.span = span;
    }

    public LatLng getCenter() {
        return center;
    }

    public void setCenter(LatLng center) {
        this.center = center;
    }

    public CoordinateSpan getSpan() {
        return span;
    }

    public void setSpan(CoordinateSpan span) {
        this.span = span;
    }
}
