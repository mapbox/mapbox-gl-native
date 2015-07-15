package com.mapbox.mapboxgl.annotations;


import com.mapbox.mapboxgl.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public class PolygonOptions {

    private Polygon polygon;

    public PolygonOptions() {
        polygon = new Polygon();
    }

    public PolygonOptions add(LatLng point) {
        polygon.points.add(point);
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
        polygon.holes.add(hole);
        return this;
    }

    public PolygonOptions fillColor(int color) {
        polygon.fillColor = color;
        return this;
    }

    public int getFillColor() {
        return polygon.fillColor;
    }

    public List<List<LatLng>> getHoles() {
        return polygon.holes;
    }

    public Polygon getPolygon() {
        return polygon;
    }

    public int getStrokeColor() {
        return polygon.strokeColor;
    }

    public float getStrokeWidth() {
        return polygon.strokeWidth;
    }

    // TODO: Implement getZIndex of Google Maps Android API
//    public float getZIndex() {
//
//    }

    public PolygonOptions strokeColor(int color) {
        polygon.strokeColor = color;
        return this;
    }

    public PolygonOptions strokeWidth(float width) {
        polygon.strokeWidth = width;
        return this;
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

    // TODO: Implement zIndex of Google Maps Android API
//    public PolygonOptions zIndex(float zIndex) {
//
//        return this;
//    }
}
