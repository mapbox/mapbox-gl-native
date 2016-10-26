package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.VisibleRegion;

/**
 * A projection is used to translate between on screen location and geographic coordinates on
 * the surface of the Earth. Screen location is in screen pixels (not display pixels)
 * with respect to the top left corner of the map (and not necessarily of the whole screen).
 */
public class Projection {

    private final MapView mapView;
    private final float screenDensity;
    private final PointF screenLocationPoint;

    Projection(@NonNull MapView mapView) {
        this.mapView = mapView;
        this.screenLocationPoint = new PointF();
        this.screenDensity = mapView.getContext() != null ? /* return default if unit test */
                mapView.getContext().getResources().getDisplayMetrics().density : 1.0f;
    }

    /**
     * <p>
     * Returns the distance spanned by one pixel at the specified latitude and current zoom level.
     * </p>
     * The distance between pixels decreases as the latitude approaches the poles.
     * This relationship parallels the relationship between longitudinal coordinates at different latitudes.
     *
     * @param latitude The latitude for which to return the value.
     * @return The distance measured in meters.
     */
    public double getMetersPerPixelAtLatitude(@FloatRange(from = -90, to = 90) double latitude) {
        return mapView.getMetersPerPixelAtLatitude(latitude);
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
        screenLocationPoint.set(point.x / screenDensity, point.y / screenDensity);
        return mapView.fromNativeScreenLocation(screenLocationPoint);
    }

    /**
     * Gets a projection of the viewing frustum for converting between screen coordinates and
     * geo-latitude/longitude coordinates.
     *
     * @return The projection of the viewing frustum in its current state.
     */
    public VisibleRegion getVisibleRegion() {
        LatLngBounds.Builder builder = new LatLngBounds.Builder();

        float left = mapView.getContentPaddingLeft();
        float right = mapView.getWidth() - mapView.getContentPaddingRight();
        float top = mapView.getContentPaddingTop();
        float bottom = mapView.getHeight() - mapView.getContentPaddingBottom();

        LatLng topLeft = fromScreenLocation(new PointF(left, top));
        LatLng topRight = fromScreenLocation(new PointF(right, top));
        LatLng bottomRight = fromScreenLocation(new PointF(right, bottom));
        LatLng bottomLeft = fromScreenLocation(new PointF(left, bottom));

        builder.include(topLeft)
                .include(topRight)
                .include(bottomRight)
                .include(bottomLeft);

        return new VisibleRegion(topLeft, topRight, bottomLeft, bottomRight, builder.build());
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
        PointF pointF = mapView.toNativeScreenLocation(location);
        pointF.set(pointF.x * screenDensity, pointF.y * screenDensity);
        return pointF;
    }

    /**
     * Calculates a zoom level based on minimum scale and current scale from MapView
     *
     * @param minScale The minimum scale to calculate the zoom level.
     * @return zoom level that fits the MapView.
     */
    public double calculateZoom(float minScale) {
        return Math.log(mapView.getScale() * minScale) / Math.log(2);
    }
}
