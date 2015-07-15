package com.mapbox.mapboxgl.annotations;

import com.mapbox.mapboxgl.geometry.LatLng;

import java.util.List;

public class PolylineOptions {

    private Polyline polyline;

    public PolylineOptions() {
        polyline = new Polyline();
    }

    public PolylineOptions alpha(float alpha) {
        polyline.alpha = alpha;
        return this;
    }

    public PolylineOptions add(LatLng point) {
        polyline.points.add(point);
        return this;
    }

    public PolylineOptions add(LatLng... points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolylineOptions addAll(Iterable<LatLng> points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolylineOptions color(int color) {
        polyline.color = color;
        return this;
    }

    // TODO: Implement geodesic of Google Maps Android API
//    public PolylineOptions geodesic (boolean geodesic) {
//
//    }

    public int getColor() {
        return polyline.color;
    }

    public List<LatLng> getPoints() {
        return polyline.points;
    }

    public Polyline getPolyline() {
        return polyline;
    }

    public float getWidth() {
        return polyline.width;
    }

    // TODO: Implement getZIndex of Google Maps Android API
//    public float getZIndex() {
//
//    }

    // TODO: Implement isGeodesic of Google Maps Android API
//    public boolean isGeodesic() {
//
//    }

    public boolean isVisible() {
        return polyline.visible;
    }

    public PolylineOptions visible(boolean visible) {
        polyline.visible = visible;
        return this;
    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return
     */
    public PolylineOptions width(float width) {
        polyline.width = width;
        return this;
    }

    // TODO: Implement writeToParcel of Google Maps Android API
//    public void writeToParcel(Parcel out, int flags) {
//
//    }

    // TODO: Implement zIndex of Google Maps Android API
//    public PolylineOptions zIndex(float zIndex) {
//
//        return this;
//    }

}
