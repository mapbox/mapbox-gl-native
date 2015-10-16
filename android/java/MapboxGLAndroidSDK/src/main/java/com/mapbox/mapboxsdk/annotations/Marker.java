package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.view.View;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;

public final class Marker extends Annotation {

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
    Marker() {
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

    public Point getAnchor() {
        return new Point((int)anchorU, (int)anchorV);
    }

    public float getAnchorU() {
        return anchorU;
    }

    public float getAnchorV() {
        return anchorV;
    }

    float getInfoWindowAnchorU() {
        return infoWindowAnchorU;
    }

    float getInfoWindowAnchorV() {
        return infoWindowAnchorV;
    }

    public LatLng getPosition() {
        return position;
    }

    float getRotation() {
        return rotation;
    }

    public String getSnippet() {
        return snippet;
    }

    public String getTitle() {
        return title;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void hideInfoWindow() {
        if (infoWindow != null) {
            infoWindow.close();
        }
        infoWindowShown = false;
    }

    boolean isDraggable() {
        return draggable;
    }

    boolean isFlat() {
        return flat;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
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

    void setPosition(LatLng position) {
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
     * If null you will get the default marker.
     *
     * @param sprite The name of the sprite.
     */
    void setSprite(@Nullable String sprite) {
        if (!TextUtils.isEmpty(sprite)) {
            this.sprite = sprite;
        }
    }

    public String getSprite() {
        return sprite;
    }

    void setTitle(String title) {
        this.title = title;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void showInfoWindow() {
        if (!isVisible() || getMapView() == null) {
            return;
        }

        getInfoWindow().adaptDefaultMarker(this);
        showInfoWindow(getInfoWindow());
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void showInfoWindow(View view){
        if (!isVisible() || getMapView() == null) {
            return;
        }

        infoWindow = new InfoWindow(view, getMapView());
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
            infoWindow = new InfoWindow(R.layout.infowindow_view, getMapView());
        }
        return infoWindow;
    }

    @Override
    void setVisible(boolean visible) {
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

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public int getTopOffsetPixels() {
        return topOffsetPixels;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setTopOffsetPixels(int topOffsetPixels) {
        this.topOffsetPixels = topOffsetPixels;
    }
}
