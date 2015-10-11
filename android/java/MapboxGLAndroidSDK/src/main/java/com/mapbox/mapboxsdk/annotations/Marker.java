package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.view.View;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;

public class Marker extends Annotation {

    private float anchorU;
    private float anchorV;
    private boolean draggable;
    private boolean flat;
    private float infoWindowAnchorU;
    private float infoWindowAnchorV;
    private LatLng position;
    private float rotation;
    private String snippet;
    private String sprite = "default_marker";
    private String title;
    private InfoWindow infoWindow = null;

    private boolean infoWindowShown = false;
    private int topOffsetPixels;

    /**
     * Constructor
     */
    public Marker() {
        super();
    }

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

    public Point getAnchor() {
        return new Point((int)anchorU, (int)anchorV);
    }

    public float getAnchorU() {
        return anchorU;
    }

    public float getAnchorV() {
        return anchorV;
    }

    public float getInfoWindowAnchorU() {
        return infoWindowAnchorU;
    }

    public float getInfoWindowAnchorV() {
        return infoWindowAnchorV;
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
        if (infoWindow != null) {
            infoWindow.close();
        }
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

    public void setAnchor(float u, float v) {
        this.anchorU = u;
        this.anchorV = v;
    }

    public void setDraggable(boolean draggable) {
        this.draggable = draggable;
    }

    public void setFlat(boolean flat) {
        this.flat = flat;
    }

    public void setInfoWindowAnchor(float u, float v) {
        infoWindowAnchorU = u;
        infoWindowAnchorV = v;
    }

    public void setPosition(LatLng position) {
        this.position = position;
    }

    public void setRotation(float rotation) {
        this.rotation = rotation;
    }

    public void setSnippet(String snippet) {
        this.snippet = snippet;
    }

    /**
     * You can specify the name of a sprite to get a marker other than the default marker.
     * This name can be found in the sprite json file:
     *
     * https://github.com/mapbox/mapbox-gl-styles/blob/mb-pages/sprites/mapbox-streets.json
     *
     * If null you will get the default marker.
     *
     * @param sprite The name of the sprite.
     */
    public void setSprite(@Nullable String sprite) {
        if (!TextUtils.isEmpty(sprite)) {
            this.sprite = sprite;
        }
    }

    public String getSprite() {
        return sprite;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public void showInfoWindow() {
        if (!isVisible() || mapView == null) {
            return;
        }

        getInfoWindow().adaptDefaultMarker(this);
        showInfoWindow(getInfoWindow());
    }

    public void showInfoWindow(View view){
        if (!isVisible() || mapView == null) {
            return;
        }

        infoWindow = new InfoWindow(view, mapView);
        showInfoWindow(infoWindow);
    }

    private void showInfoWindow(InfoWindow iw) {
        iw.open(this, getPosition(), (int) anchorU, (int) anchorV + topOffsetPixels);
        iw.setBoundMarker(this);
        infoWindowShown = true;
    }

    /**
     * Use to set a custom OnTouchListener for the InfoWindow.
     * By default the InfoWindow will close on touch.
     * @param listener Custom OnTouchListener
     */
    public void setInfoWindowOnTouchListener(View.OnTouchListener  listener) {
        if (listener == null) {
            return;
        }
        getInfoWindow().setOnTouchListener(listener);
    }

    /**
     * Common internal InfoWindow initialization method
     * @return InfoWindow for Marker
     */
    private InfoWindow getInfoWindow() {
        if (infoWindow == null) {
            infoWindow = new InfoWindow(R.layout.infowindow, mapView);
        }
        return infoWindow;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        if (!visible && infoWindowShown) {
            hideInfoWindow();
        }
    }

    //  TODO Method in Google Maps Android API
//    public int hashCode()

    // TODO: Implement this method of Google Maps Android API
//    void setIcon(BitmapDescriptor icon) {
//
//    }

    void setTopOffsetPixels(int topOffsetPixels) {
        this.topOffsetPixels = topOffsetPixels;
    }
}
