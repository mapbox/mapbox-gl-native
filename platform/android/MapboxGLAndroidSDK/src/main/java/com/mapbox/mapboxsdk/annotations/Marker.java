package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.View;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * Marker is an annotation that shows an icon image at a geographical location.
 * <p>
 * An {@link InfoWindow} can be shown when a Marker is pressed
 * </p>
 */
public class Marker extends Annotation {

    private LatLng position;
    private String snippet;
    private Icon icon;
    private String title;

    private InfoWindow infoWindow;
    private boolean infoWindowShown;

    private int topOffsetPixels;
    private int rightOffsetPixels;

    /**
     * Constructor
     */
    Marker() {
        super();
    }

    public Marker(BaseMarkerOptions baseMarkerOptions) {
        position = baseMarkerOptions.position;
        snippet = baseMarkerOptions.snippet;
        icon = baseMarkerOptions.icon;
        title = baseMarkerOptions.title;
    }

    Marker(BaseMarkerViewOptions baseMarkerViewOptions) {
        position = baseMarkerViewOptions.position;
        snippet = baseMarkerViewOptions.snippet;
        icon = baseMarkerViewOptions.icon;
        title = baseMarkerViewOptions.title;
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
     *
     * @return true if the infoWindow is shown
     */
    public boolean isInfoWindowShown() {
        return infoWindowShown;
    }

    /**
     * Sets the position.
     *
     * @param position new position
     */
    public void setPosition(LatLng position) {
        this.position = position;
        MapboxMap map = getMapboxMap();
        if (map != null) {
            map.updateMarker(this);
        }
    }

    public void setSnippet(String snippet) {
        this.snippet = snippet;
        refreshInfoWindowContent();
    }

    /**
     * Sets the icon.
     *
     * @param icon The icon to be used as Marker image
     */
    public void setIcon(@Nullable Icon icon) {
        this.icon = icon;
        MapboxMap map = getMapboxMap();
        if (map != null) {
            map.updateMarker(this);
        }
    }

    public Icon getIcon() {
        return icon;
    }

    public void setTitle(String title) {
        this.title = title;
        refreshInfoWindowContent();
    }

    @Nullable
    public InfoWindow getInfoWindow() {
        return infoWindow;
    }

    /**
     * Update only for default Marker's InfoWindow content for Title and Snippet
     */
    private void refreshInfoWindowContent() {
        if (isInfoWindowShown() && mapView != null && mapboxMap != null && mapboxMap.getInfoWindowAdapter() == null) {
            InfoWindow infoWindow = getInfoWindow(mapView);
            if (mapView.getContext() != null) {
                infoWindow.adaptDefaultMarker(this, mapboxMap, mapView);
            }
            MapboxMap map = getMapboxMap();
            if (map != null) {
                map.updateMarker(this);
            }
            infoWindow.update();
        }
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @param mapboxMap the hosting mapbox map
     * @param mapView   the hosting map view
     * @return the info window that was shown
     */
    public InfoWindow showInfoWindow(@NonNull MapboxMap mapboxMap, @NonNull MapView mapView) {
        setMapboxMap(mapboxMap);
        setMapView(mapView);
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
        iw.open(mapView, this, getPosition(), rightOffsetPixels, topOffsetPixels);
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
     *
     * @param topOffsetPixels the top offset pixels.
     */
    public void setTopOffsetPixels(int topOffsetPixels) {
        this.topOffsetPixels = topOffsetPixels;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @param rightOffsetPixels the right offset pixels.
     */
    public void setRightOffsetPixels(int rightOffsetPixels) {
        this.rightOffsetPixels = rightOffsetPixels;
    }

    @Override
    public String toString() {
        return "Marker [position[" + getPosition() + "]]";
    }
}
