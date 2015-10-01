package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class MarkerOptions extends AnnotationOptions {

    public MarkerOptions() {
        annotation = new Marker();
    }

    public MarkerOptions anchor(float u, float v) {
        ((Marker)annotation).anchorU = u;
        ((Marker)annotation).anchorV = v;
        return this;
    }

    public MarkerOptions draggable(boolean draggable) {
        ((Marker)annotation).draggable = draggable;
        return this;
    }

    public MarkerOptions flat(boolean flat) {
        ((Marker)annotation).flat = flat;
        return this;
    }

    public float getAnchorU() {
        return ((Marker)annotation).anchorU;
    }

    public float getAnchorV() {
        return ((Marker)annotation).anchorV;
    }

    // TODO: Implement this method of Google Maps Android API
//    public BitmapDescriptor getIcon () {
//
//    }

    public float getInfoWindowAnchorU() {
        return ((Marker)annotation).infoWindowAnchorU;
    }

    public float getInfoWindowAnchorV() {
        return ((Marker)annotation).infoWindowAnchorV;
    }

    public Marker getMarker() {
        return (Marker)annotation;
    }

    public LatLng getPosition() {
        return ((Marker)annotation).position;
    }

    public float getRotation() {
        return ((Marker)annotation).rotation;
    }

    public String getSnippet() {
        return ((Marker)annotation).snippet;
    }

    public String getTitle() {
        return ((Marker)annotation).title;
    }

    public MarkerOptions infoWindowAnchor(float u, float v) {
        ((Marker)annotation).infoWindowAnchorU = u;
        ((Marker)annotation).infoWindowAnchorV = v;
        return this;
    }

    public boolean isDraggable() {
        return ((Marker)annotation).draggable;
    }

    public boolean isFlat() {
        return ((Marker)annotation).flat;
    }

    public boolean isVisible() {
        return ((Marker)annotation).visible;
    }

    public MarkerOptions position(LatLng position) {
        ((Marker)annotation).position = position;
        return this;
    }

    public MarkerOptions rotation(float rotation) {
        ((Marker)annotation).rotation = rotation;
        return this;
    }

    public MarkerOptions snippet(String snippet) {
        ((Marker)annotation).snippet = snippet;
        return this;
    }

    public MarkerOptions sprite(String sprite) {
        ((Marker)annotation).sprite = sprite;
        return this;
    }

    public MarkerOptions title(String title) {
        ((Marker)annotation).title = title;
        return this;
    }

    public MarkerOptions visible(boolean visible) {
        annotation.visible = visible;
        return this;
    }


    // TODO: Implement this method of Google Maps Android API
//    public MarkerOptions icon(BitmapDescriptor icon) {
//
//    }

    // TODO: Implement this method of Google Maps Android API
//    public void writeToParcel (Parcel out, int flags)

}
