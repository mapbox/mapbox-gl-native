package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public abstract class MultiPoint extends Annotation {

    List<LatLng> points;

    public MultiPoint() {
        super();
        points = new ArrayList<>();
    }

    public List<LatLng> getPoints() {
        return points;
    }

    // TODO: Implement getZIndex of Google Maps Android API
//    public float getZIndex() {
//
//    }

    // TODO: Implement hashCode of Google Maps Android API
//    public int hashCode() {
//
//    }

    // TODO: Implement isGeodesic of Google Maps Android API
//    public boolean isGeodesic() {
//
//    }

    /**
     * Sets the points of this polyline. This method will take a copy
     * of the points, so further mutations to points will have no effect
     * on this polyline.
     *
     * @param points
     */
    void setPoints(List<LatLng> points) {
        this.points = new ArrayList<>();
        for (LatLng latLng : points) {
            this.points.add(latLng);
        }
    }

}
