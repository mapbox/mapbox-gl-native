package com.mapbox.mapboxsdk.geometry;

/**
 * Implementation of iOS MGLCoordinateBounds
 */
public class CoordinateBounds {

    private LatLng southWest;
    private LatLng northEast;

    public CoordinateBounds(LatLng southWest, LatLng northEast) {
        this.southWest = southWest;
        this.northEast = northEast;
    }

    public LatLng getSouthWest() {
        return southWest;
    }

    public void setSouthWest(LatLng southWest) {
        this.southWest = southWest;
    }

    public LatLng getNorthEast() {
        return northEast;
    }

    public void setNorthEast(LatLng northEast) {
        this.northEast = northEast;
    }

    @Override
    public int hashCode() {
        int result;
        long temp;
        temp = southWest.hashCode();
        result = (int) (temp ^ (temp >>> 32));
        temp = northEast.hashCode();
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o instanceof CoordinateBounds) {
            CoordinateBounds other = (CoordinateBounds) o;
            return getNorthEast().equals(other.getNorthEast())
                    && getSouthWest().equals(other.getSouthWest());
        }
        return false;
    }

    @Override
    public String toString() {
        return "CoordinateBounds [northEast[" + getNorthEast() + "], southWest[]" + getSouthWest() + "]";
    }
}
