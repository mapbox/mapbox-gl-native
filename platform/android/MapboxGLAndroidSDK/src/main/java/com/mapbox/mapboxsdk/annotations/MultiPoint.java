package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

/**
 * Multipoint is an abstract annotation for combining geographical locations.
 */
public abstract class MultiPoint extends Annotation {

    private List<LatLng> points;
    private List<List<LatLng>> holes;
    private float alpha = 1.0f;

    protected MultiPoint() {
        super();
        points = new ArrayList<>();
        holes = new ArrayList<>();
    }

    /**
     * Returns a copy of the points.
     *
     * @return points - as a copy
     */
    public List<LatLng> getPoints() {
        return new ArrayList<>(points);
    }

    /*
     * Returns a copy of the holes.
     * @return holes - as a copy
     */
    public List<List<LatLng>> getHoles() {
        return new ArrayList<>(holes);
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

    void addHole(List<LatLng> hole) {
        holes.add(hole);
    }

    public float getAlpha() {
        return alpha;
    }

    void setAlpha(float alpha) {
        this.alpha = alpha;
    }
}
