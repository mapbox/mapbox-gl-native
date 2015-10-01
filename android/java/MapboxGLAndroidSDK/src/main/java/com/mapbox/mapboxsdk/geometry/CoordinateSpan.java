package com.mapbox.mapboxsdk.geometry;

/**
 * Implementation of iOS MKCoordinateSpan
 */
public class CoordinateSpan {

    private double latitudeSpan;
    private double longitudeSpan;

    public CoordinateSpan(final double latitudeSpan, final double longitudeSpan) {
        this.latitudeSpan = latitudeSpan;
        this.longitudeSpan = longitudeSpan;
    }

    public double getLatitudeSpan() {
        return latitudeSpan;
    }

    public void setLatitudeSpan(final double latitudeSpan) {
        this.latitudeSpan = latitudeSpan;
    }

    public double getLongitudeSpan() {
        return longitudeSpan;
    }

    public void setLongitudeSpan(final double longitudeSpan) {
        this.longitudeSpan = longitudeSpan;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o instanceof CoordinateSpan) {
            CoordinateSpan other = (CoordinateSpan) o;
            return longitudeSpan == other.getLongitudeSpan()
                    && latitudeSpan == other.getLatitudeSpan();
        }
        return false;
    }

}
