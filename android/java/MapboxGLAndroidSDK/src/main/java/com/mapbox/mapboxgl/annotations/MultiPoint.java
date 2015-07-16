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

    /**
     * Returns a copy of the points.
     *
     * @return points - as a copy
     */
    public List<LatLng> getPoints() {
        return new ArrayList<>(points);
    }

    /**
     * Sets the points of this polyline. This method will take a copy
     * of the points, so further mutations to points will have no effect
     * on this polyline.
     *
     * @param points
     */
    void setPoints(List<LatLng> points) {
        this.points = new ArrayList<>(points);
    }


    // TODO: Implement hashCode of Google Maps Android API
//    public int hashCode() {
//
//    }

    // TODO: Implement isGeodesic of Google Maps Android API
//    public boolean isGeodesic() {
//
//    }

    // TODO: Implement setGeodesic of Google Maps Android API
//    public void setGeodesic(boolean geodesic) {
//
//    }

}
