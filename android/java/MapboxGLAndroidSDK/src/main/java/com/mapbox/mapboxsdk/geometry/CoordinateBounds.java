package com.mapbox.mapboxsdk.geometry;

/**
 * Implementation of iOS MGLCoordinateBounds
 */
public class CoordinateBounds {

    private LatLng sw;
    private LatLng ne;

    public CoordinateBounds(LatLng sw, LatLng ne) {
        this.sw = sw;
        this.ne = ne;
    }

    public LatLng getSw() {
        return sw;
    }

    public void setSw(LatLng sw) {
        this.sw = sw;
    }

    public LatLng getNe() {
        return ne;
    }

    public void setNe(LatLng ne) {
        this.ne = ne;
    }
}