package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.geometry.LatLng;


public class Marker extends Annotation {

    /**
     * Member fields are without access qualifiers
     * so that they can be directly set by MarkerOptions
     * from within the module.
     */
    float anchorU;
    float anchorV;
    boolean draggable;
    boolean flat;
    float infoWindowAnchorU;
    float infoWindowAnchorV;
    LatLng position;
    float rotation;
    String snippet;
    String title;

    private boolean infoWindowShown = false;

    public Marker() {}

    public boolean equals(Object other) {
        double lat = position.getLatitude();
        double lng = position.getLongitude();
        // TODO Implement equals
        return false;
    }

    public float getAlpha() {
        return alpha;
    }

    /**
     * NOTE: Google Maps Android API uses String for IDs.
     *
     * Internal C++ id is stored as unsigned int.
     *
     * @return the annotation id
     */
    public long getId() {
        return id;
    }

    public LatLng getPosition() {
        return position;
    }

    public float getRotation() {
        return rotation;
    }

    public String getSnippet() {
        return snippet;
    }

    public String getTitle() {
        return title;
    }

//    Method in Google Maps Android API
//    public int hashCode()

    public void hideInfoWindow() {
        //TODO hideInfoWindow
        infoWindowShown = false;
    }

    public boolean isDraggable() {
        return draggable;
    }

    public boolean isFlat() {
        return flat;
    }

    public boolean isInfoWindowShown () {
        return infoWindowShown;
    }

    void setAlpha(float alpha) {
        this.alpha = alpha;
    }

    void setDraggable(boolean draggable) {
        this.draggable = draggable;
    }

    void setFlat(boolean flat) {
        this.flat = flat;
    }

    // TODO: Implement this method of Google Maps Android API
//    void setIcon(BitmapDescriptor icon) {
//
//    }

    void setInfoWindowAnchor(float u, float v) {
        infoWindowAnchorU = u;
        infoWindowAnchorV = v;
    }

    void setPosition(LatLng latlng) {
        this.position = position;
    }

    void setRotation(float rotation) {
        this.rotation = rotation;
    }

    void setSnippet(String snippet) {
        this.snippet = snippet;
    }

    void setTitle(String title) {
        this.title = title;
    }

    void showInfoWindow() {
        infoWindowShown = true;
    }
}
