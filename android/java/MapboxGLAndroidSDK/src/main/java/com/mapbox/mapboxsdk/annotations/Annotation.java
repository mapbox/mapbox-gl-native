package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.views.MapView;

import java.lang.ref.WeakReference;

public abstract class Annotation implements Comparable<Annotation> {

    /**
     * The annotation id
     * <p/>
     * Internal C++ id is stored as unsigned int.
     */
    private long id = -1; // -1 unless added to a MapView
    private WeakReference<MapView> mapView;

    protected Annotation() {
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public long getId() {
        return id;
    }

    public void remove() {
        if ((mapView == null) || (mapView.get() == null)) {
            return;
        }
        mapView.get().removeAnnotation(this);
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setId(long id) {
        this.id = id;
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public void setMapView(MapView mapView) {
        this.mapView = new WeakReference<>(mapView);
    }

    protected MapView getMapView() {
        if (mapView == null) {
            return null;
        }
        return mapView.get();
    }

    @Override
    public int compareTo(@NonNull Annotation annotation) {
        if (id < annotation.getId()) {
            return 1;
        } else if (id > annotation.getId()) {
            return -1;
        }

        // Equal
        return 0;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Annotation that = (Annotation) o;

        return getId() == that.getId();
    }

    @Override
    public int hashCode() {
        return (int) (getId() ^ (getId() >>> 32));
    }
}
