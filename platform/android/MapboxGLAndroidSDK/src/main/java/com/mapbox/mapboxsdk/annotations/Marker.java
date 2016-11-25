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
public class Marker implements Comparable<Marker> {

    private long id = -1; // -1 unless added to a MapView
    private MapboxMap mapboxMap;
    private MapView mapView;
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

    Marker(LatLng position, Icon icon, String title, String snippet) {
        this.position = position;
        this.icon = icon;
        this.title = title;
        this.snippet = snippet;
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
            infoWindow = new InfoWindow(mapView, R.layout.mapbox_infowindow_view, getMapboxMap());
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

    /**
     * <p>
     * Gets the annotation's unique ID.
     * </p>
     * This ID is unique for a MapView instance and is suitable for associating your own extra
     * data with.
     *
     * @return the assigned id
     */
    public long getId() {
        return id;
    }

    public void remove() {
        if (mapboxMap == null) {
            return;
        }
        mapboxMap.removeMarker(this);
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @param id the assigned id
     */
    public void setId(long id) {
        this.id = id;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     *
     * @param mapboxMap the hosting mapbox map
     */
    public void setMapboxMap(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
    }

    /**
     * Gets the hosting mapbox map.
     *
     * @return the MapboxMap
     */
    protected MapboxMap getMapboxMap() {
        return mapboxMap;
    }

    /**
     * Don not use this method.  Used internally by the SDK.
     *
     * @param mapView the hosting map view
     */
    public void setMapView(MapView mapView) {
        this.mapView = mapView;
    }

    /**
     * Gets the hosting map view.
     *
     * @return The MapView
     */
    protected MapView getMapView() {
        return mapView;
    }

    @Override
    public int compareTo(@NonNull Marker marker) {
        if (id < marker.getId()) {
            return 1;
        } else if (id > marker.getId()) {
            return -1;
        }
        return 0;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || !(o instanceof Marker)) return false;
        Marker that = (Marker) o;
        return id == that.getId();
    }

    @Override
    public int hashCode() {
        return (int) (getId() ^ (getId() >>> 32));
    }

}
