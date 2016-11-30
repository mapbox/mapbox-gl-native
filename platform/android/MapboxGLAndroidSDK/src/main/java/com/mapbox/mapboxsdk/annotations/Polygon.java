package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.util.ArrayList;
import java.util.List;

/**
 * Polygon is a geometry annotation that's a closed loop of coordinates.
 */
public final class Polygon implements Comparable<Polygon> {

    private long id = -1; // -1 unless added to a MapView
    private MapboxMap mapboxMap;
    private MapView mapView;
    private List<LatLng> points;
    private float alpha = 1.0f;
    private int fillColor = Color.BLACK; // default fillColor is black
    private int strokeColor = Color.BLACK; // default strokeColor is black

    Polygon() {
        points = new ArrayList<>();
    }

    /**
     * Get the color of the fill region of the polygon.
     *
     * @return the color of the fill
     */
    public int getFillColor() {
        return fillColor;
    }

    /**
     * Get the color fo the stroke of the polygon.
     *
     * @return the color of the stroke
     */
    public int getStrokeColor() {
        return strokeColor;
    }

    /**
     * Sets the color of the fill region of the polygon.
     *
     * @param color - the color in ARGB format
     */
    public void setFillColor(int color) {
        fillColor = color;
        update();
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
     * Sets the points of this polygon. This method will take a copy
     * of the points, so further mutations to points will have no effect
     * on this polyline.
     *
     * @param points the points of the polyline
     */
    public void setPoints(List<LatLng> points) {
        this.points = new ArrayList<>(points);
        update();
    }

    /**
     * Add a point to this polygon and draw it.
     *
     * @param point the point of the polyline to add
     */
    public void addPoint(LatLng point) {
        points.add(point);
        update();
    }

    /**
     * Return the alpha value of the polygon.
     *
     * @return the alpha value
     */
    public float getAlpha() {
        return alpha;
    }

    /**
     * Sets the alpha value of the polygon.
     *
     * @param alpha the alpha value
     */
    public void setAlpha(float alpha) {
        this.alpha = alpha;
        update();
    }

    /**
     * Sets the color of the stroke of the polygon.
     *
     * @param color - the color in ARGB format
     */
    public void setStrokeColor(int color) {
        strokeColor = color;
        update();
    }

    void update() {
        if (mapboxMap != null) {
            mapboxMap.updatePolygon(this);
        }
    }

    public void remove() {
        if (mapboxMap == null) {
            return;
        }
        mapboxMap.removePolygon(this);
    }

    /**
     * <p>
     * Gets the annotation's unique ID.
     * </p>
     * This ID is unique for a MapView instance and is suitable for associating your own extra
     * data with.
     *
     * @return the assigned id
     */
    public long getId() {
        return id;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @param id the assigned id
     */
    public void setId(long id) {
        this.id = id;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @param mapboxMap the hosting mapbox map
     */
    public void setMapboxMap(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
    }

    /**
     * Gets the hosting mapbox map.
     *
     * @return the MapboxMap
     */
    protected MapboxMap getMapboxMap() {
        return mapboxMap;
    }

    /**
     * Don not use this method.  Used internally by the SDK.
     *
     * @param mapView the hosting map view
     */
    public void setMapView(MapView mapView) {
        this.mapView = mapView;
    }

    /**
     * Gets the hosting map view.
     *
     * @return The MapView
     */
    protected MapView getMapView() {
        return mapView;
    }

    @Override
    public int compareTo(@NonNull Polygon polygon) {
        if (id < polygon.getId()) {
            return 1;
        } else if (id > polygon.getId()) {
            return -1;
        }
        return 0;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || !(o instanceof Polygon)) return false;
        Polygon that = (Polygon) o;
        return id == that.getId();
    }

    @Override
    public int hashCode() {
        return (int) (getId() ^ (getId() >>> 32));
    }
}
