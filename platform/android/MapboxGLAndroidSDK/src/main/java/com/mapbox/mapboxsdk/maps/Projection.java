package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.geometry.ProjectedMeters;
import com.mapbox.mapboxsdk.geometry.VisibleRegion;

/**
 * A projection is used to translate between on screen location and geographic coordinates on
 * the surface of the Earth. Screen location is in screen pixels (not display pixels)
 * with respect to the top left corner of the map (and not necessarily of the whole screen).
 */
public class Projection {

  private final NativeMapView nativeMapView;
  private int[] contentPadding;

  Projection(@NonNull NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
    this.contentPadding = new int[] {0, 0, 0, 0};
  }

  void setContentPadding(int[] contentPadding, int[] userLocationViewPadding) {
    this.contentPadding = contentPadding;

    int[] padding = new int[] {
      contentPadding[0] + userLocationViewPadding[0],
      contentPadding[1] + userLocationViewPadding[1],
      contentPadding[2] + userLocationViewPadding[2],
      contentPadding[3] + userLocationViewPadding[3]
    };

    nativeMapView.setContentPadding(padding);
  }

  int[] getContentPadding() {
    return contentPadding;
  }

  public void invalidateContentPadding(int[] userLocationViewPadding) {
    setContentPadding(contentPadding, userLocationViewPadding);
  }

  /**
   * Returns the spherical Mercator projected meters for a LatLng.
   */
  public ProjectedMeters getProjectedMetersForLatLng(LatLng latLng) {
    return nativeMapView.projectedMetersForLatLng(latLng);
  }

  /**
   * Returns the LatLng for a spherical Mercator projected meters.
   */
  public LatLng getLatLngForProjectedMeters(ProjectedMeters projectedMeters) {
    return nativeMapView.latLngForProjectedMeters(projectedMeters);
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
    return nativeMapView.getMetersPerPixelAtLatitude(latitude);
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
    return nativeMapView.latLngForPixel(point);
  }

  /**
   * Gets a projection of the viewing frustum for converting between screen coordinates and
   * geo-latitude/longitude coordinates.
   *
   * @return The projection of the viewing frustum in its current state.
   */
  public VisibleRegion getVisibleRegion() {
    float left = 0;
    float right = nativeMapView.getWidth();
    float top = 0;
    float bottom = nativeMapView.getHeight();

    LatLng topLeft = fromScreenLocation(new PointF(left, top));
    LatLng topRight = fromScreenLocation(new PointF(right, top));
    LatLng bottomRight = fromScreenLocation(new PointF(right, bottom));
    LatLng bottomLeft = fromScreenLocation(new PointF(left, bottom));

    return new VisibleRegion(topLeft, topRight, bottomLeft, bottomRight,
      new LatLngBounds.Builder()
        .include(topRight)
        .include(bottomLeft)
        .include(bottomRight)
        .include(topLeft)
        .build()
    );
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
    return nativeMapView.pixelForLatLng(location);
  }

  float getHeight() {
    return nativeMapView.getHeight();
  }

  float getWidth() {
    return nativeMapView.getWidth();
  }

  /**
   * Calculates a zoom level based on minimum scale and current scale from MapView
   *
   * @param minScale The minimum scale to calculate the zoom level.
   * @return zoom level that fits the MapView.
   */
  public double calculateZoom(float minScale) {
    return nativeMapView.getZoom() + Math.log(minScale) / Math.log(2);
  }
}
