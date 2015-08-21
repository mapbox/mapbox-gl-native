package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.geometry.LatLng;


public class Marker extends Annotation {

    float anchorU;
    float anchorV;
    boolean draggable;
    boolean flat;
    float infoWindowAnchorU;
    float infoWindowAnchorV;
    LatLng position;
    float rotation;
    String snippet;
    String sprite = "default_marker";
    String title;

    private boolean infoWindowShown = false;

    public Marker() {}

    /**
     * If two markers have the same LatLng, they are equal.
     *
     * @param other object
     * @return boolean - do they have the same LatLng
     */
    public boolean equals(Object other) {
        if (!(other instanceof Marker)) return false;
        double lat = position.getLatitude();
        double lng = position.getLongitude();
        LatLng otherPosition = ((Marker)other).getPosition();
        double otherLat = otherPosition.getLatitude();
        double otherLng = otherPosition.getLongitude();
        return (lat == otherLat && otherLng == lng);
    }

    public float getAlpha() {
        return alpha;
    }

    public float getAnchorU() {
        return anchorU;
    }

    public float getAnchorV() {
        return anchorV;
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

    void setAnchor(float u, float v) {
        this.anchorU = u;
        this.anchorV = v;
    }

    void setDraggable(boolean draggable) {
        this.draggable = draggable;
    }

    void setFlat(boolean flat) {
        this.flat = flat;
    }

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

    /**
     * You can specify the name of a sprite to get a marker other than the default marker.
     * This name can be found in the sprite json file:
     *
     * https://github.com/mapbox/mapbox-gl-styles/blob/mb-pages/sprites/mapbox-streets.json
     *
     * @param sprite
     */
    void setSprite(String sprite) {
        this.sprite = sprite;
    }

    void setTitle(String title) {
        this.title = title;
    }

    void showInfoWindow() {
        infoWindowShown = true;
    }


    //  TODO Method in Google Maps Android API
//    public int hashCode()

    // TODO: Implement this method of Google Maps Android API
//    void setIcon(BitmapDescriptor icon) {
//
//    }
}
