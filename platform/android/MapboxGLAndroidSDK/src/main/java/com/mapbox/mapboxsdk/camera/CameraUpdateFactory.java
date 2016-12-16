package com.mapbox.mapboxsdk.camera;

import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.RectF;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.utils.MathUtils;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Factory for creating {@link CameraUpdate} objects.
 */
public final class CameraUpdateFactory {

  /**
   * Returns a CameraUpdate that moves the camera to a specified CameraPosition.
   *
   * @param cameraPosition Camera Position to change to
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newCameraPosition(@NonNull CameraPosition cameraPosition) {
    return new CameraPositionUpdate(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt,
      cameraPosition.zoom);
  }

  /**
   * Returns a CameraUpdate that moves the center of the screen to a latitude and longitude
   * specified by a LatLng object. This centers the camera on the LatLng object.
   *
   * @param latLng Target location to change to
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLng(@NonNull LatLng latLng) {
    return new CameraPositionUpdate(-1, latLng, -1, -1);
  }

  /**
   * Returns a {@link CameraUpdate} that transforms the camera such that the specified
   * latitude/longitude bounds are centered on screen at the greatest possible zoom level.
   * You can specify padding, in order to inset the bounding box from the map view's edges.
   * The returned CameraUpdate has a bearing of 0 and a tilt of 0.
   *
   * @param bounds  Bounds to match Camera position with
   * @param padding Padding added to the bounds
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, int padding) {
    return newLatLngBounds(bounds, padding, padding, padding, padding);
  }

  /**
   * Returns a {@link CameraUpdate} that transforms the camera such that the specified
   * latitude/longitude bounds are centered on screen at the greatest possible zoom level.
   * You can specify padding, in order to inset the bounding box from the map view's edges.
   * The returned CameraUpdate has a bearing of 0 and a tilt of 0.
   *
   * @param bounds        Bounds to base the Camera position out of
   * @param paddingLeft   Padding left of the bounds
   * @param paddingTop    Padding top of the bounds
   * @param paddingRight  Padding right of the bounds
   * @param paddingBottom Padding bottom of the bounds
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, int paddingLeft, int paddingTop,
                                             int paddingRight, int paddingBottom) {
    return new CameraBoundsUpdate(bounds, paddingLeft, paddingTop, paddingRight, paddingBottom);
  }

  /**
   * Returns a CameraUpdate that moves the center of the screen to a latitude and longitude
   * specified by a LatLng object, and moves to the given zoom level.
   *
   * @param latLng Target location to change to
   * @param zoom   Zoom level to change to
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLngZoom(@NonNull LatLng latLng, double zoom) {
    return new CameraPositionUpdate(-1, latLng, -1, zoom);
  }

  /**
   * Returns a CameraUpdate that scrolls the camera over the map,
   * shifting the center of view by the specified number of pixels in the x and y directions.
   *
   * @param xPixel Amount of pixels to scroll to in x direction
   * @param yPixel Amount of pixels to scroll to in y direction
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate scrollBy(float xPixel, float yPixel) {
    return new CameraMoveUpdate(xPixel, yPixel);
  }

  /**
   * Returns a CameraUpdate that shifts the zoom level of the current camera viewpoint.
   *
   * @param amount Amount of zoom level to change with
   * @param focus  Focus point of zoom
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate zoomBy(double amount, Point focus) {
    return new ZoomUpdate(amount, focus.x, focus.y);
  }

  /**
   * Returns a CameraUpdate that shifts the zoom level of the current camera viewpoint.
   *
   * @param amount Amount of zoom level to change with
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate zoomBy(double amount) {
    return new ZoomUpdate(ZoomUpdate.ZOOM_BY, amount);
  }

  /**
   * Returns a CameraUpdate that zooms in on the map by moving the viewpoint's height closer to
   * the Earth's surface. The zoom increment is 1.0.
   *
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate zoomIn() {
    return new ZoomUpdate(ZoomUpdate.ZOOM_IN);
  }

  /**
   * Returns a CameraUpdate that zooms out on the map by moving the viewpoint's height farther
   * away from the Earth's surface. The zoom increment is -1.0.
   *
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate zoomOut() {
    return new ZoomUpdate(ZoomUpdate.ZOOM_OUT);
  }

  /**
   * Returns a CameraUpdate that moves the camera viewpoint to a particular zoom level.
   *
   * @param zoom Zoom level to change to
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate zoomTo(double zoom) {
    return new ZoomUpdate(ZoomUpdate.ZOOM_TO, zoom);
  }

  //
  // CameraUpdate types
  //

  static final class CameraPositionUpdate implements CameraUpdate {

    private final double bearing;
    private final LatLng target;
    private final double tilt;
    private final double zoom;

    CameraPositionUpdate(double bearing, LatLng target, double tilt, double zoom) {
      this.bearing = bearing;
      this.target = target;
      this.tilt = tilt;
      this.zoom = zoom;
    }

    public LatLng getTarget() {
      return target;
    }

    public double getBearing() {
      return bearing;
    }

    public double getTilt() {
      return tilt;
    }

    public double getZoom() {
      return zoom;
    }

    @Override
    public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
      CameraPosition previousPosition = mapboxMap.getCameraPosition();
      if (target == null) {
        return new CameraPosition.Builder(this)
          .target(previousPosition.target)
          .build();
      }
      return new CameraPosition.Builder(this).build();
    }
  }

  static final class CameraBoundsUpdate implements CameraUpdate {

    private LatLngBounds bounds;
    private RectF padding;

    CameraBoundsUpdate(LatLngBounds bounds, RectF padding) {
      this.bounds = bounds;
      this.padding = padding;
    }

    CameraBoundsUpdate(LatLngBounds bounds, int[] padding) {
      this(bounds, new RectF(padding[0], padding[1], padding[2], padding[3]));
    }

    CameraBoundsUpdate(LatLngBounds bounds, int paddingLeft, int paddingTop, int paddingRight, int paddingBottom) {
      this(bounds, new int[] {paddingLeft, paddingTop, paddingRight, paddingBottom});
    }

    public LatLngBounds getBounds() {
      return bounds;
    }

    public RectF getPadding() {
      return padding;
    }

    @Override
    public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
      // Get required objects
      Projection projection = mapboxMap.getProjection();
      UiSettings uiSettings = mapboxMap.getUiSettings();

      // calculate correct padding
      int[] mapPadding = mapboxMap.getPadding();
      RectF latLngPadding = getPadding();
      RectF padding = new RectF(latLngPadding.left + mapPadding[0],
        latLngPadding.top + mapPadding[1],
        latLngPadding.right + mapPadding[2],
        latLngPadding.bottom + mapPadding[3]);

      // Calculate the bounds of the possibly rotated shape with respect to the viewport
      PointF nePixel = new PointF(-Float.MAX_VALUE, -Float.MAX_VALUE);
      PointF swPixel = new PointF(Float.MAX_VALUE, Float.MAX_VALUE);
      float viewportHeight = uiSettings.getHeight();
      for (LatLng latLng : getBounds().toLatLngs()) {
        PointF pixel = projection.toScreenLocation(latLng);
        swPixel.x = Math.min(swPixel.x, pixel.x);
        nePixel.x = Math.max(nePixel.x, pixel.x);
        swPixel.y = Math.min(swPixel.y, viewportHeight - pixel.y);
        nePixel.y = Math.max(nePixel.y, viewportHeight - pixel.y);
      }

      // Calculate width/height
      float width = nePixel.x - swPixel.x;
      float height = nePixel.y - swPixel.y;

      double zoom = 0;
      float minScale = 1;
      // Calculate the zoom level
      if (padding != null) {
        float scaleX = (uiSettings.getWidth() - padding.left - padding.right) / width;
        float scaleY = (uiSettings.getHeight() - padding.top - padding.bottom) / height;
        minScale = scaleX < scaleY ? scaleX : scaleY;
        zoom = calculateZoom(mapboxMap, minScale);
        zoom = MathUtils.clamp(zoom, mapboxMap.getMinZoomLevel(), mapboxMap.getMaxZoomLevel());
      }

      // Calculate the center point
      PointF paddedNEPixel = new PointF(nePixel.x + padding.right / minScale, nePixel.y + padding.top / minScale);
      PointF paddedSWPixel = new PointF(swPixel.x - padding.left / minScale, swPixel.y - padding.bottom / minScale);
      PointF centerPixel = new PointF((paddedNEPixel.x + paddedSWPixel.x) / 2, (paddedNEPixel.y + paddedSWPixel.y) / 2);
      centerPixel.y = viewportHeight - centerPixel.y;
      LatLng center = projection.fromScreenLocation(centerPixel);

      return new CameraPosition.Builder()
        .target(center)
        .zoom(zoom)
        .tilt(0)
        .bearing(0)
        .build();
    }

    /**
     * Calculates a zoom level based on minimum scale and current scale from MapView
     *
     * @param minScale The minimum scale to calculate the zoom level.
     * @return zoom level that fits the MapView.
     */
    public double calculateZoom(MapboxMap mapboxMap, float minScale) {
      return Math.log(mapboxMap.getCameraPosition().zoom * minScale) / Math.log(2);
    }
  }

  static final class CameraMoveUpdate implements CameraUpdate {

    private float x;
    private float y;

    CameraMoveUpdate(float x, float y) {
      this.x = x;
      this.y = y;
    }

    @Override
    public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
      UiSettings uiSettings = mapboxMap.getUiSettings();
      Projection projection = mapboxMap.getProjection();

      // Calculate the new center point
      float viewPortWidth = uiSettings.getWidth();
      float viewPortHeight = uiSettings.getHeight();
      PointF targetPoint = new PointF(viewPortWidth / 2 + x, viewPortHeight / 2 + y);

      // Convert point to LatLng
      LatLng latLng = projection.fromScreenLocation(targetPoint);

      CameraPosition previousPosition = mapboxMap.getCameraPosition();
      return new CameraPosition.Builder()
        .target(latLng != null ? latLng : previousPosition.target)
        .zoom(previousPosition.zoom)
        .tilt(previousPosition.tilt)
        .bearing(previousPosition.bearing)
        .build();
    }
  }

  static final class ZoomUpdate implements CameraUpdate {

    @IntDef( {ZOOM_IN, ZOOM_OUT, ZOOM_BY, ZOOM_TO, ZOOM_TO_POINT})
    @Retention(RetentionPolicy.SOURCE)
    @interface Type {
    }

    static final int ZOOM_IN = 0;
    static final int ZOOM_OUT = 1;
    static final int ZOOM_BY = 2;
    static final int ZOOM_TO = 3;
    static final int ZOOM_TO_POINT = 4;

    @Type
    private final int type;
    private final double zoom;
    private float x;
    private float y;

    ZoomUpdate(@Type int type) {
      this.type = type;
      this.zoom = 0;
    }

    ZoomUpdate(@Type int type, double zoom) {
      this.type = type;
      this.zoom = zoom;
    }

    ZoomUpdate(double zoom, float x, float y) {
      this.type = ZOOM_TO_POINT;
      this.zoom = zoom;
      this.x = x;
      this.y = y;
    }

    public double getZoom() {
      return zoom;
    }

    @Type
    public int getType() {
      return type;
    }

    public float getX() {
      return x;
    }

    public float getY() {
      return y;
    }

    double transformZoom(double currentZoom) {
      switch (getType()) {
        case CameraUpdateFactory.ZoomUpdate.ZOOM_IN:
          currentZoom++;
          break;
        case CameraUpdateFactory.ZoomUpdate.ZOOM_OUT:
          currentZoom--;
          if (currentZoom < 0) {
            currentZoom = 0;
          }
          break;
        case CameraUpdateFactory.ZoomUpdate.ZOOM_TO:
          currentZoom = getZoom();
          break;
        case CameraUpdateFactory.ZoomUpdate.ZOOM_BY:
          currentZoom = currentZoom + getZoom();
          break;
        case CameraUpdateFactory.ZoomUpdate.ZOOM_TO_POINT:
          currentZoom = currentZoom + getZoom();
          break;
      }
      return currentZoom;
    }

    @Override
    public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
      CameraPosition cameraPosition = mapboxMap.getCameraPosition();
      if (getType() != CameraUpdateFactory.ZoomUpdate.ZOOM_TO_POINT) {
        return new CameraPosition.Builder(cameraPosition)
          .zoom(transformZoom(cameraPosition.zoom))
          .build();
      } else {
        return new CameraPosition.Builder(cameraPosition)
          .zoom(transformZoom(cameraPosition.zoom))
          .target(mapboxMap.getProjection().fromScreenLocation(new PointF(getX(), getY())))
          .build();
      }
    }
  }
}
