package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.Nullable;
import android.text.TextUtils;
import com.mapbox.mapboxsdk.geometry.LatLng;

public final class MarkerOptions {

    private Marker marker;

    public MarkerOptions() {
        marker = new Marker();
    }

    private MarkerOptions anchor(float u, float v) {
        marker.setAnchor(u, v);
        return this;
    }

    private MarkerOptions draggable(boolean draggable) {
        marker.setDraggable(draggable);
        return this;
    }

    private MarkerOptions flat(boolean flat) {
        marker.setFlat(flat);
        return this;
    }

    private float getAnchorU() {
        return marker.getAnchorU();
    }

    private float getAnchorV() {
        return marker.getAnchorV();
    }

    // TODO: Implement this method of Google Maps Android API
//    public BitmapDescriptor getIcon () {
//
//    }

    private float getInfoWindowAnchorU() {
        return marker.getInfoWindowAnchorU();
    }

    private float getInfoWindowAnchorV() {
        return marker.getInfoWindowAnchorV();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public Marker getMarker() {
        return (Marker) marker;
    }

    public LatLng getPosition() {
        return marker.getPosition();
    }

    private float getRotation() {
        return marker.getRotation();
    }

    public String getSnippet() {
        return marker.getSnippet();
    }

    public String getTitle() {
        return marker.getTitle();
    }

    public Sprite getIcon() {
        return marker.getIcon();
    }

    private MarkerOptions infoWindowAnchor(float u, float v) {
        marker.setInfoWindowAnchor(u, v);
        return this;
    }

    private boolean isDraggable() {
        return marker.isDraggable();
    }

    private boolean isFlat() {
        return marker.isFlat();
    }

    private boolean isVisible() {
        return marker.isVisible();
    }

    public MarkerOptions position(LatLng position) {
        marker.setPosition(position);
        return this;
    }

    private MarkerOptions rotation(float rotation) {
        marker.setRotation(rotation);
        return this;
    }

    public MarkerOptions snippet(String snippet) {
        marker.setSnippet(snippet);
        return this;
    }

    public MarkerOptions icon(@Nullable Sprite icon) {
        marker.setIcon(icon);
        return this;
    }

    public MarkerOptions title(String title) {
        marker.setTitle(title);
        return this;
    }

    private MarkerOptions visible(boolean visible) {
        marker.setVisible(visible);
        return this;
    }

    private MarkerOptions alpha(float alpha) {
        marker.setAlpha(alpha);
        return this;
    }

    private float getAlpha() {
        return marker.getAlpha();
    }

    // TODO: Implement this method of Google Maps Android API
//    public MarkerOptions icon(BitmapDescriptor icon) {
//
//    }

    // TODO: Implement this method of Google Maps Android API
//    public void writeToParcel (Parcel out, int flags)

}
