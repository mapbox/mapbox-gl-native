package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.geometry.LatLng;

/**
 * Created by Nicholas Hallahan on 7/13/15.
 * nick@theoutpost.io
 */
public class MarkerOptions {

    private Marker marker;

    public MarkerOptions() {
        marker = new Marker();
    }

    public MarkerOptions alpha(float alpha) {
        marker.alpha = alpha;
        return this;
    }

    public MarkerOptions anchor(float u, float v) {
        marker.anchorU = u;
        marker.anchorV = v;
        return this;
    }

    public MarkerOptions draggable(boolean draggable) {
        marker.draggable = draggable;
        return this;
    }

    public MarkerOptions flat(boolean flat) {
        marker.flat = flat;
        return this;
    }

    public float getAlpha() {
        return marker.alpha;
    }

    public float getAnchorU() {
        return marker.anchorU;
    }

    public float getAnchorV() {
        return marker.anchorV;
    }

    // TODO: Implement this method of Google Maps Android API
//    public BitmapDescriptor getIcon () {
//
//    }

    public float getInfoWindowAnchorU() {
        return marker.infoWindowAnchorU;
    }

    public float getInfoWindowAnchorV() {
        return marker.infoWindowAnchorV;
    }

    public Marker getMarker() {
        return marker;
    }

    public LatLng getPosition() {
        return marker.position;
    }

    public float getRotation() {
        return marker.rotation;
    }

    public String getSnippet() {
        return marker.snippet;
    }

    public String getTitle() {
        return marker.title;
    }

    // TODO: Implement this method of Google Maps Android API
//    public MarkerOptions icon(BitmapDescriptor icon) {
//
//    }

    public MarkerOptions infoWindowAnchor(float u, float v) {
        marker.infoWindowAnchorU = u;
        marker.infoWindowAnchorV = v;
        return this;
    }

    public boolean isDraggable() {
        return marker.draggable;
    }

    public boolean isFlat() {
        return marker.flat;
    }

    public boolean isVisible() {
        return marker.visible;
    }

    public MarkerOptions position(LatLng position) {
        marker.position = position;
        return this;
    }

    public MarkerOptions rotation(float rotation) {
        marker.rotation = rotation;
        return this;
    }

    public MarkerOptions snippet(String snippet) {
        marker.snippet = snippet;
        return this;
    }

    public MarkerOptions title(String title) {
        marker.title = title;
        return this;
    }

    public MarkerOptions visible(boolean visible) {
        marker.visible = visible;
        return this;
    }

    // TODO: Implement this method of Google Maps Android API
//    public void writeToParcel (Parcel out, int flags)

}
