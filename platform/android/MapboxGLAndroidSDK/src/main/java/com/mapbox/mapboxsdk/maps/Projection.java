package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.VisibleRegion;

/**
 * A projection is used to translate between on screen location and geographic coordinates on
 * the surface of the Earth (LatLng). Screen location is in screen pixels (not display pixels)
 * with respect to the top left corner of the map (and not necessarily of the whole screen).
 */
public class Projection {

    private MapView mMapView;

    Projection(@NonNull MapView mapView) {
        this.mMapView = mapView;
    }

    /**
     * Returns the geographic location that corresponds to a screen location.
     * The screen location is specified in screen pixels (not display pixels) relative to the
     * top left of the map (not the top left of the whole screen).
     *
     * @param point A Point on the screen in screen pixels.
     * @return The LatLng corresponding to the point on the screen, or null if the ray through
     * the given screen point does not intersect the ground plane.
     */
    public LatLng fromScreenLocation(PointF point) {
        return mMapView.fromScreenLocation(point);
    }

    /**
     * Gets a projection of the viewing frustum for converting between screen coordinates and
     * geo-latitude/longitude coordinates.
     *
     * @return The projection of the viewing frustum in its current state.
     */
    public VisibleRegion getVisibleRegion() {
        LatLngBounds.Builder builder = new LatLngBounds.Builder();

        int viewportWidth = mMapView.getWidth();
        int viewportHeight = mMapView.getHeight();

        LatLng topLeft = fromScreenLocation(new PointF(0, 0));
        LatLng topRight = fromScreenLocation(new PointF(viewportWidth, 0));
        LatLng bottomRight = fromScreenLocation(new PointF(viewportWidth, viewportHeight));
        LatLng bottomLeft = fromScreenLocation(new PointF(0, viewportHeight));

        builder.include(topLeft)
                .include(topRight)
                .include(bottomRight)
                .include(bottomLeft);

        return new VisibleRegion(topLeft,topRight,bottomLeft,bottomRight,builder.build());
    }

    /**
     * Returns a screen location that corresponds to a geographical coordinate (LatLng).
     * The screen location is in screen pixels (not display pixels) relative to the top left
     * of the map (not of the whole screen).
     *
     * @param location A LatLng on the map to convert to a screen location.
     * @return A Point representing the screen location in screen pixels.
     */
    public PointF toScreenLocation(LatLng location) {
        return mMapView.toScreenLocation(location);
    }
}
