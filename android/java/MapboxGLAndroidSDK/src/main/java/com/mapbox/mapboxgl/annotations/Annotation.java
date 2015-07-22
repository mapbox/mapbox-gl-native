package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.views.MapView;


public abstract class Annotation {

    /**
     * The annotation id
     *
     * Internal C++ id is stored as unsigned int.
     */
    protected Long id; // null unless added to a MapView
    private MapView mapView;

    float alpha = 1;
    boolean visible = true;


    public Annotation() {}

    public float getAlpha() {
        return alpha;
    }

    public long getId() {
        return id;
    }

    public boolean isVisible() {
        return visible;
    }

    public void remove() {
        if (mapView == null) return;
        mapView.removeAnnotation(this);
    }

    public void setAlpha(float alpha) {
        this.alpha = alpha;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public void setMapView(MapView mapView) {
        this.mapView = mapView;
    }

    public void setVisible(boolean visible) {
        this.visible = visible;
    }

    // TODO: Implement getZIndex of Google Maps Android API
//    public float getZIndex() {
//
//    }

    // TODO: Implement setZIndex of Google Maps Android API
//    public void setZIndex(float zIndex) {
//
//    }
}
