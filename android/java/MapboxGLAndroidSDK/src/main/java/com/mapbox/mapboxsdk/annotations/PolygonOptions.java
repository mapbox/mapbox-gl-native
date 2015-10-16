package com.mapbox.mapboxsdk.annotations;


import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public final class PolygonOptions {

    private Polygon polygon;

    public PolygonOptions() {
        polygon = new Polygon();
    }

    public PolygonOptions add(LatLng point) {
        polygon.addPoint(point);
        return this;
    }

    public PolygonOptions add(LatLng... points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolygonOptions addAll(Iterable<LatLng> points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1729
     *
     * @param points - an iterable (list) of points for cutting a hole
     * @return PolygonOptions - the options object
     */
    private PolygonOptions addHole (Iterable<LatLng> points) {
        List<LatLng> hole = new ArrayList<>();
        for (LatLng point : points) {
            hole.add(point);
        }
        polygon.addHole(hole);
        return this;
    }

    public PolygonOptions alpha(float alpha) {
        polygon.setAlpha(alpha);
        return this;
    }

    public float getAlpha() {
        return polygon.getAlpha();
    }

    /**
     * Sets the color of the polygon.
     *
     * @param color - the color in ARGB format
     * @return PolygonOptions - the options object
     */
    public PolygonOptions fillColor(int color) {
        polygon.setFillColor(color);
        return this;
    }

    public int getFillColor() {
        return polygon.getFillColor();
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1729
     *
     * @return a list of lists of points for cutting holes
     */
    private List<List<LatLng>> getHoles() {
        return polygon.getHoles();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public Polygon getPolygon() {
        return polygon;
    }

    public int getStrokeColor() {
        return polygon.getStrokeColor();
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     *
     * @return stroke width as float
     */
    private float getStrokeWidth() {
        return polygon.getStrokeWidth();
    }

    /**
     * Sets the color of the stroke of the polygon.
     *
     * @param color - the color in ARGB format
     * @return PolygonOptions - the options object
     */
    public PolygonOptions strokeColor(int color) {
        polygon.setStrokeColor(color);
        return this;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     *
     * @return stroke width as float
     */
    private PolygonOptions strokeWidth(float width) {
        polygon.setStrokeWidth(width);
        return this;
    }

    private PolygonOptions visible(boolean visible) {
        polygon.setVisible(visible);
        return this;
    }

    private boolean isVisible() {
        return polygon.isVisible();
    }

    public List<LatLng> getPoints() {
        // the getter gives us a copy, which is the safe thing to do...
        return polygon.getPoints();
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

}
