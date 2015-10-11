package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.Nullable;
import android.text.TextUtils;
import com.mapbox.mapboxsdk.geometry.LatLng;

public class MarkerOptions extends AnnotationOptions {

    public MarkerOptions() {
        annotation = new Marker();
    }

    public MarkerOptions anchor(float u, float v) {
        ((Marker)annotation).setAnchor(u, v);
        return this;
    }

    public MarkerOptions draggable(boolean draggable) {
        ((Marker)annotation).setDraggable(draggable);
        return this;
    }

    public MarkerOptions flat(boolean flat) {
        ((Marker)annotation).setFlat(flat);
        return this;
    }

    public float getAnchorU() {
        return ((Marker)annotation).getAnchorU();
    }

    public float getAnchorV() {
        return ((Marker)annotation).getAnchorV();
    }

    // TODO: Implement this method of Google Maps Android API
//    public BitmapDescriptor getIcon () {
//
//    }

    public float getInfoWindowAnchorU() {
        return ((Marker)annotation).getInfoWindowAnchorU();
    }

    public float getInfoWindowAnchorV() {
        return ((Marker)annotation).getInfoWindowAnchorV();
    }

    public Marker getMarker() {
        return (Marker)annotation;
    }

    public LatLng getPosition() {
        return ((Marker)annotation).getPosition();
    }

    public float getRotation() {
        return ((Marker)annotation).getRotation();
    }

    public String getSnippet() {
        return ((Marker)annotation).getSnippet();
    }

    public String getTitle() {
        return ((Marker)annotation).getTitle();
    }

    public String getSprite() {
        return ((Marker)annotation).getSprite();
    }

    public MarkerOptions infoWindowAnchor(float u, float v) {
        ((Marker)annotation).setInfoWindowAnchor(u, v);
        return this;
    }

    public boolean isDraggable() {
        return ((Marker)annotation).isDraggable();
    }

    public boolean isFlat() {
        return ((Marker)annotation).isFlat();
    }

    public boolean isVisible() {
        return ((Marker)annotation).isVisible();
    }

    public MarkerOptions position(LatLng position) {
        ((Marker)annotation).setPosition(position);
        return this;
    }

    public MarkerOptions rotation(float rotation) {
        ((Marker)annotation).setRotation(rotation);
        return this;
    }

    public MarkerOptions snippet(String snippet) {
        ((Marker)annotation).setSnippet(snippet);
        return this;
    }

    public MarkerOptions sprite(@Nullable String sprite) {
        if (!TextUtils.isEmpty(sprite)) {
            ((Marker)annotation).setSprite(sprite);
        }
        return this;
    }

    public MarkerOptions title(String title) {
        ((Marker)annotation).setTitle(title);
        return this;
    }

    public MarkerOptions visible(boolean visible) {
        annotation.setVisible(visible);
        return this;
    }


    // TODO: Implement this method of Google Maps Android API
//    public MarkerOptions icon(BitmapDescriptor icon) {
//
//    }

    // TODO: Implement this method of Google Maps Android API
//    public void writeToParcel (Parcel out, int flags)

}
