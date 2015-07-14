package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.views.MapView;

/**
 * Created by Nicholas Hallahan on 7/13/15.
 * nick@theoutpost.io
 */
public abstract class Annotation {

    /**
     * The annotation id
     *
     * Internal C++ id is stored as unsigned int.
     */
    protected Long id;
    private MapView mapView;

    public Annotation() {}

    public Annotation(long id) {
        this.id = id;
    }

    public long getId() {
        return id;
    }

    public void remove() {
        if (mapView == null) return;
        mapView.removeAnnotation(this);
    }

    public void setId(Long id) {
        this.id = id;
    }

    public void setMapView(MapView mapView) {
        this.mapView = mapView;
    }

}
