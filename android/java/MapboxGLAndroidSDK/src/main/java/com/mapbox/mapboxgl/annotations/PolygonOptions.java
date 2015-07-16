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

    public PolygonOptions addHole (Iterable<LatLng> points) {
        List<LatLng> hole = new ArrayList<>();
        for (LatLng point : points) {
            hole.add(point);
        }
        ((Polygon)annotation).holes.add(hole);
        return this;
    }

    public PolygonOptions fillColor(int color) {
        ((Polygon)annotation).fillColor = color;
        return this;
    }

    public int getFillColor() {
        return ((Polygon)annotation).fillColor;
    }

    public List<List<LatLng>> getHoles() {
        return ((Polygon)annotation).holes;
    }

    public Polygon getPolygon() {
        return ((Polygon)annotation);
    }

    public int getStrokeColor() {
        return ((Polygon)annotation).strokeColor;
    }

    public float getStrokeWidth() {
        return ((Polygon)annotation).strokeWidth;
    }

    public PolygonOptions strokeColor(int color) {
        ((Polygon)annotation).strokeColor = color;
        return this;
    }

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
