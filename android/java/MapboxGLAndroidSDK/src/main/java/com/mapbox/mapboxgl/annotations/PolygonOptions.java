package com.mapbox.mapboxgl.annotations;


import com.mapbox.mapboxgl.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public class PolygonOptions extends MultiPointOptions {

    public PolygonOptions() {
        annotation = new Polygon();
    }

    public PolygonOptions add(LatLng point) {
        ((MultiPoint)annotation).points.add(point);
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
    public PolygonOptions addHole (Iterable<LatLng> points) {
        List<LatLng> hole = new ArrayList<>();
        for (LatLng point : points) {
            hole.add(point);
        }
        ((Polygon)annotation).holes.add(hole);
        return this;
    }

    /**
     * Sets the color of the polygon.
     *
     * @param color - the color in ARGB format
     * @return PolygonOptions - the options object
     */
    public PolygonOptions fillColor(int color) {
        ((Polygon)annotation).fillColor = color;
        return this;
    }

    public int getFillColor() {
        return ((Polygon)annotation).fillColor;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1729
     *
     * @return a list of lists of points for cutting holes
     */
    public List<List<LatLng>> getHoles() {
        return ((Polygon)annotation).holes;
    }

    public Polygon getPolygon() {
        return ((Polygon)annotation);
    }

    public int getStrokeColor() {
        return ((Polygon)annotation).strokeColor;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     *
     * @return stroke width as float
     */
    public float getStrokeWidth() {
        return ((Polygon)annotation).strokeWidth;
    }

    /**
     * Sets the color of the stroke of the polygon.
     *
     * @param color - the color in ARGB format
     * @return PolygonOptions - the options object
     */
    public PolygonOptions strokeColor(int color) {
        ((Polygon)annotation).strokeColor = color;
        return this;
    }

    /**
     * UNIMPLEMENTED: Needs implementation in Native.
     * https://github.com/mapbox/mapbox-gl-native/issues/1737
     *
     * @return stroke width as float
     */
    public PolygonOptions strokeWidth(float width) {
        ((Polygon)annotation).strokeWidth = width;
        return this;
    }

    public PolygonOptions visible(boolean visible) {
        annotation.visible = visible;
        return this;
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

}
