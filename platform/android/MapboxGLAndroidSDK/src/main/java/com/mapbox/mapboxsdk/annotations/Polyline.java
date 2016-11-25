package com.mapbox.mapboxsdk.annotations;

import android.graphics.Color;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.util.ArrayList;
import java.util.List;

/**
 * Polyline is a geometry feature with an unclosed list of coordinates drawn as a line
 */
public final class Polyline implements Comparable<Polyline> {

    private long id = -1; // -1 unless added to a MapView
    private MapboxMap mapboxMap;
    private MapView mapView;
    private List<LatLng> points;
    private float alpha = 1.0f;

    private int color = Color.BLACK; // default color is black
    private float width = 10; // As specified by Google API Docs (in pixels)

    Polyline() {
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
    public void setPoints(List<LatLng> points) {
        this.points = new ArrayList<>(points);
        update();
    }

    /**
     * Add a point to this polyline and draw it.
     *
     * @param point the point of the polyline to add
     */
    public void addPoint(LatLng point) {
        points.add(point);
        update();
    }

    /**
     * Return the alpha value of the polyline.
     *
     * @return the alpha value
     */
    public float getAlpha() {
        return alpha;
    }

    /**
     * Sets the alpha value of the polyline.
     *
     * @param alpha the alpha value
     */
    public void setAlpha(float alpha) {
        this.alpha = alpha;
        update();
    }

    /**
     * Returns the Polyline tint color.
     *
     * @return the tint color
     */
    public int getColor() {
        return color;
    }

    /**
     * Returns the Polyline width.
     *
     * @return the width
     */
    public float getWidth() {
        return width;
    }

    /**
     * Sets the color of the polyline.
     *
     * @param color - the color in ARGB format
     */
    public void setColor(int color) {
        this.color = color;
        update();
    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     */
    public void setWidth(float width) {
        this.width = width;
        update();
    }

    void update() {
        if (mapboxMap != null) {
            mapboxMap.updatePolyline(this);
        }
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

    public void remove() {
        if (mapboxMap == null) {
            return;
        }
        mapboxMap.removePolyline(this);
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
    public int compareTo(@NonNull Polyline annotation) {
        if (id < annotation.getId()) {
            return 1;
        } else if (id > annotation.getId()) {
            return -1;
        }
        return 0;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || !(o instanceof Polyline)) return false;
        Polyline that = (Polyline) o;
        return id == that.getId();
    }

    @Override
    public int hashCode() {
        return (int) (getId() ^ (getId() >>> 32));
    }
}
