package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * Annotation is an overlay on top of a {@link MapView},
 * from which {@link Polygon}, {@link Polyline} and {@link Marker} are derived.
 * <p>
 * it manages attachment to a map and identification, but does not require
 * content to be placed at a geographical point.
 * </p>
 */
public abstract class Annotation implements Comparable<Annotation> {

    /**
     * <p>
     * The annotation id
     * </p>
     * Internal C++ id is stored as unsigned int.
     */
    private long id = -1; // -1 unless added to a MapView
    protected MapboxMap mapboxMap;
    protected MapView mapView;

    protected Annotation() {
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
        mapboxMap.removeAnnotation(this);
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
    public int compareTo(@NonNull Annotation annotation) {
        if (id < annotation.getId()) {
            return 1;
        } else if (id > annotation.getId()) {
            return -1;
        }
        return 0;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || !(o instanceof Annotation)) return false;
        Annotation that = (Annotation) o;
        return id == that.getId();
    }

    @Override
    public int hashCode() {
        return (int) (getId() ^ (getId() >>> 32));
    }
}
