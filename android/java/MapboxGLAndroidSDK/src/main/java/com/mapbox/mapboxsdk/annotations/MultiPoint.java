package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public abstract class MultiPoint extends Annotation {

    private List<LatLng> points;
    private float alpha = 1.0f;

    protected MultiPoint() {
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
     * @param points the points of the polyline
     */
    void setPoints(List<LatLng> points) {
        this.points = new ArrayList<>(points);
    }

    void addPoint(LatLng point) {
        points.add(point);
    }

    public float getAlpha() {
        return alpha;
    }

    void setAlpha(float alpha) {
        this.alpha = alpha;
    }
}
