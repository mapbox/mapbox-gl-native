package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.views.MapView;

public abstract class Annotation implements Comparable<Annotation> {

    /**
     * The annotation id
     *
     * Internal C++ id is stored as unsigned int.
     */
    private long id = -1; // -1 unless added to a MapView
    private MapView mapView;

    private float alpha = 1.0f;
    private boolean visible = true;

    protected Annotation() {}

    public float getAlpha() {
        return alpha;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public long getId() {
        return id;
    }

    boolean isVisible() {
        return visible;
    }

    public void remove() {
        if (mapView == null) return;
        mapView.removeAnnotation(this);
    }

    void setAlpha(float alpha) {
        this.alpha = alpha;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setId(long id) {
        this.id = id;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setMapView(MapView mapView) {
        this.mapView = mapView;
    }

    protected MapView getMapView() {
        return mapView;
    }

    void setVisible(boolean visible) {
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


    @Override
    public boolean equals(Object o) {
        if (o == null) {
            return false;
        }

        if (o instanceof Annotation) {
            Annotation comp = (Annotation) o;
            return id == comp.id;
        }
        return false;
    }

    @Override
    public int compareTo(Annotation annotation) {

        if (annotation == null) {
            return -1;
        }

        if (id < annotation.getId()) {
            return 1;
        } else if (id > annotation.getId()) {
            return -1;
        }

        // Equal
        return 0;
    }
}
