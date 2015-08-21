package com.mapbox.mapboxgl.geometry;

/**
 * Implementation of iOS MKCoordinateRegion
 */
public class CoordinateRegion {
    private LatLng center;
    private CoordinateSpan span;

    public CoordinateRegion(final LatLng center, final CoordinateSpan span) {
        this.center = center;
        this.span = span;
    }

    public LatLng getCenter() {
        return center;
    }

    public void setCenter(final LatLng center) {
        this.center = center;
    }

    public CoordinateSpan getSpan() {
        return span;
    }

    public void setSpan(final CoordinateSpan span) {
        this.span = span;
    }
}
