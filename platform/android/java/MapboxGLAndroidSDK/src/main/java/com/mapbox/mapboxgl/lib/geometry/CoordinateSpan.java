package com.mapbox.mapboxgl.lib.geometry;

/**
 * Implementation of iOS MKCoordinateSpan
 */
public final class CoordinateSpan {

    private double latitudeSpan;
    private double longitudeSpan;

    public CoordinateSpan(double latitudeSpan, double longitudeSpan) {
        this.latitudeSpan = latitudeSpan;
        this.longitudeSpan = longitudeSpan;
    }

    public double getLatitudeSpan() {
        return latitudeSpan;
    }

    public void setLatitudeSpan(double latitudeSpan) {
        this.latitudeSpan = latitudeSpan;
    }

    public double getLongitudeSpan() {
        return longitudeSpan;
    }

    public void setLongitudeSpan(double longitudeSpan) {
        this.longitudeSpan = longitudeSpan;
    }
}
