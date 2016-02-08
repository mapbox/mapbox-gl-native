package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;

/**
 * Marker is an annotation that shows an icon image at a geographical location.
 * </p>
 * An {@link InfoWindow} can be shown when a Marker is pressed
 * <p/>
 */
public final class Marker extends Annotation {

    private LatLng position;
    private String snippet;
    private Icon icon;
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
    public void setIcon(@Nullable Icon icon) {
        this.icon = icon;
    }

    public Icon getIcon() {
        return icon;
    }

    void setTitle(String title) {
        this.title = title;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public InfoWindow showInfoWindow(@NonNull MapboxMap mapboxMap, @NonNull MapView mapView) {
        setMapboxMap(mapboxMap);
        MapboxMap.InfoWindowAdapter infoWindowAdapter = getMapboxMap().getInfoWindowAdapter();
        if (infoWindowAdapter != null) {
            // end developer is using a custom InfoWindowAdapter
            View content = infoWindowAdapter.getInfoWindow(this);
            if (content != null) {
                infoWindow = new InfoWindow(content, mapboxMap);
                showInfoWindow(infoWindow, mapView);
                return infoWindow;
            }
        }

        InfoWindow infoWindow = getInfoWindow(mapView);
        if (mapView.getContext() != null) {
            infoWindow.adaptDefaultMarker(this, mapboxMap, mapView);
        }
        return showInfoWindow(infoWindow, mapView);
    }

    private InfoWindow showInfoWindow(InfoWindow iw, MapView mapView) {
        iw.open(mapView, this, getPosition(), 0, topOffsetPixels);
        infoWindowShown = true;
        return iw;
    }

    private InfoWindow getInfoWindow(@NonNull MapView mapView) {
        if (infoWindow == null && mapView.getContext() != null) {
            infoWindow = new InfoWindow(mapView, R.layout.infowindow_view, getMapboxMap());
        }
        return infoWindow;
    }

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

    @Override
    public String toString() {
        return "Marker [position[" + getPosition() + "]]";
    }
}
