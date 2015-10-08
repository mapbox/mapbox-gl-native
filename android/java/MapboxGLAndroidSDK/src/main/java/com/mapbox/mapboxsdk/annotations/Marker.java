package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.Nullable;
import android.view.View;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;

public final class Marker extends Annotation {

    private LatLng position;
    private String snippet;
    private Sprite icon;
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

    public LatLng getPosition() {
        return position;
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

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public boolean isInfoWindowShown() {
        return infoWindowShown;
    }

    void setPosition(LatLng position) {
        this.position = position;
    }

    void setSnippet(String snippet) {
        this.snippet = snippet;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setIcon(@Nullable Sprite icon) {
        this.icon = icon;
    }

    public Sprite getIcon() {
        return icon;
    }

    void setTitle(String title) {
        this.title = title;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void showInfoWindow() {
        if (getMapView() == null) {
            return;
        }

        MapView.InfoWindowAdapter infoWindowAdapter = getMapView().getInfoWindowAdapter();
        if (infoWindowAdapter != null) {
            // end developer is using a custom InfoWindowAdapter
            View content = infoWindowAdapter.getInfoWindow(this);
            if (content != null) {
                infoWindow = new InfoWindow(content, getMapView());
                showInfoWindow(infoWindow);
                return;
            }
        }

        getInfoWindow().adaptDefaultMarker(this);
        showInfoWindow(getInfoWindow());
    }

    private void showInfoWindow(InfoWindow iw) {
        iw.open(this, getPosition(), 0, topOffsetPixels);
        infoWindowShown = true;
    }

    private InfoWindow getInfoWindow() {
        if (infoWindow == null) {
            infoWindow = new InfoWindow(R.layout.infowindow_view, getMapView());
        }
        return infoWindow;
    }

    /*
    @Override
    void setVisible(boolean visible) {
        super.setVisible(visible);
        if (!visible && infoWindowShown) {
            hideInfoWindow();
        }
    }
    */

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setTopOffsetPixels(int topOffsetPixels) {
        this.topOffsetPixels = topOffsetPixels;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        if (!super.equals(o)) return false;

        Marker marker = (Marker) o;
        return !(getPosition() != null ? !getPosition().equals(marker.getPosition()) : marker.getPosition() != null);
    }

    @Override
    public int hashCode() {
        int result = super.hashCode();
        result = 31 * result + (getPosition() != null ? getPosition().hashCode() : 0);
        return result;
    }
}
