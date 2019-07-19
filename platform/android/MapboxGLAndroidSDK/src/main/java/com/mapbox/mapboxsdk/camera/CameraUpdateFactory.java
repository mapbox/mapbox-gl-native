package com.mapbox.mapboxsdk.camera;

import android.graphics.Point;
import android.graphics.PointF;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.Arrays;

/**
 * Factory for creating CameraUpdate objects.
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
   * Returns a CameraUpdate that transforms the camera such that the specified
   * latitude/longitude bounds are centered on screen at the greatest possible zoom level while maintaining
   * current camera position bearing and tilt values.
   * <p>
   * You can specify padding, in order to inset the bounding box from the map view's edges.
   * </p>
   *
   * @param bounds  Bounds to match Camera position with
   * @param padding Padding added to the bounds
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, int padding) {
    return newLatLngBounds(bounds, padding, padding, padding, padding);
  }

  /**
   * Returns a CameraUpdate that transforms the camera such that the specified
   * latitude/longitude bounds are centered on screen at the greatest possible zoom level while using
   * provided bearing and tilt values.
   * <p>
   * You can specify padding, in order to inset the bounding box from the map view's edges.
   * </p>
   *
   * @param bounds  Bounds to match Camera position with
   * @param bearing Bearing to take in account when generating the bounds
   * @param tilt    Tilt to take in account when generating the bounds
   * @param padding Padding added to the bounds
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, double bearing, double tilt, int padding) {
    return newLatLngBounds(bounds, bearing, tilt, padding, padding, padding, padding);
  }

  /**
   * Returns a CameraUpdate that transforms the camera such that the specified
   * latitude/longitude bounds are centered on screen at the greatest possible zoom level while maintaining
   * current camera position bearing and tilt values.
   * <p>
   * You can specify padding, in order to inset the bounding box from the map view's edges.
   * </p>
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
    return new CameraBoundsUpdate(bounds, null, null, paddingLeft, paddingTop, paddingRight, paddingBottom);
  }

  /**
   * Returns a CameraUpdate that transforms the camera such that the specified
   * latitude/longitude bounds are centered on screen at the greatest possible zoom level while using
   * provided bearing and tilt values.
   * <p>
   * You can specify padding, in order to inset the bounding box from the map view's edges.
   * </p>
   *
   * @param bounds        Bounds to base the Camera position out of
   * @param bearing       Bearing to take in account when generating the bounds
   * @param tilt          Tilt to take in account when generating the bounds
   * @param paddingLeft   Padding left of the bounds
   * @param paddingTop    Padding top of the bounds
   * @param paddingRight  Padding right of the bounds
   * @param paddingBottom Padding bottom of the bounds
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, double bearing, double tilt,
                                             int paddingLeft, int paddingTop, int paddingRight, int paddingBottom) {
    return new CameraBoundsUpdate(bounds, bearing, tilt, paddingLeft, paddingTop, paddingRight, paddingBottom);
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

  /**
   * Returns a CameraUpdate that moves the camera viewpoint to a particular bearing.
   *
   * @param bearing Bearing to change to
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate bearingTo(double bearing) {
    return new CameraPositionUpdate(bearing, null, -1, -1);
  }

  /**
   * Returns a CameraUpdate that moves the camera viewpoint to a particular tilt.
   *
   * @param tilt Tilt to change to
   * @return CameraUpdate Final Camera Position
   */
  public static CameraUpdate tiltTo(double tilt) {
    return new CameraPositionUpdate(-1, null, tilt, -1);
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

    @Override
    public boolean equals(@Nullable Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      CameraPositionUpdate that = (CameraPositionUpdate) o;

      if (Double.compare(that.bearing, bearing) != 0) {
        return false;
      }
      if (Double.compare(that.tilt, tilt) != 0) {
        return false;
      }
      if (Double.compare(that.zoom, zoom) != 0) {
        return false;
      }
      return target != null ? target.equals(that.target) : that.target == null;
    }

    @Override
    public int hashCode() {
      int result;
      long temp;
      temp = Double.doubleToLongBits(bearing);
      result = (int) (temp ^ (temp >>> 32));
      result = 31 * result + (target != null ? target.hashCode() : 0);
      temp = Double.doubleToLongBits(tilt);
      result = 31 * result + (int) (temp ^ (temp >>> 32));
      temp = Double.doubleToLongBits(zoom);
      result = 31 * result + (int) (temp ^ (temp >>> 32));
      return result;
    }

    @Override
    public String toString() {
      return "CameraPositionUpdate{"
        + "bearing=" + bearing
        + ", target=" + target
        + ", tilt=" + tilt
        + ", zoom=" + zoom
        + '}';
    }
  }

  static final class CameraBoundsUpdate implements CameraUpdate {

    private final LatLngBounds bounds;
    private final int[] padding;
    private final Double bearing;
    private final Double tilt;

    CameraBoundsUpdate(LatLngBounds bounds, Double bearing, Double tilt, int[] padding) {
      this.bounds = bounds;
      this.padding = padding;
      this.bearing = bearing;
      this.tilt = tilt;
    }

    CameraBoundsUpdate(LatLngBounds bounds, Double bearing, Double tilt, int paddingLeft,
                       int paddingTop, int paddingRight, int paddingBottom) {
      this(bounds, bearing, tilt, new int[] {paddingLeft, paddingTop, paddingRight, paddingBottom});
    }

    public LatLngBounds getBounds() {
      return bounds;
    }

    public int[] getPadding() {
      return padding;
    }

    @Override
    public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
      if (bearing == null && tilt == null) {
        // use current camera position tilt and bearing
        return mapboxMap.getCameraForLatLngBounds(bounds, padding);
      } else {
        // use provided tilt and bearing
        assert bearing != null;
        assert tilt != null;
        return mapboxMap.getCameraForLatLngBounds(bounds, padding, bearing, tilt);
      }
    }

    @Override
    public boolean equals(@Nullable Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      CameraBoundsUpdate that = (CameraBoundsUpdate) o;

      if (!bounds.equals(that.bounds)) {
        return false;
      }
      return Arrays.equals(padding, that.padding);
    }

    @Override
    public int hashCode() {
      int result = bounds.hashCode();
      result = 31 * result + Arrays.hashCode(padding);
      return result;
    }

    @Override
    public String toString() {
      return "CameraBoundsUpdate{"
        + "bounds=" + bounds
        + ", padding=" + Arrays.toString(padding)
        + '}';
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
      int[] padding = mapboxMap.getPadding();

      // we inverse the map padding, is reapplied when using moveTo/easeTo or animateTo
      PointF targetPoint = new PointF(
        (viewPortWidth - padding[0] + padding[1]) / 2 + x,
        (viewPortHeight + padding[1] - padding[3]) / 2 + y
      );

      LatLng latLng = projection.fromScreenLocation(targetPoint);
      CameraPosition previousPosition = mapboxMap.getCameraPosition();
      CameraPosition position =
        new CameraPosition.Builder()
          .target(latLng)
          .zoom(previousPosition.zoom)
          .tilt(previousPosition.tilt)
          .bearing(previousPosition.bearing)
          .build();
      return position;
    }

    @Override
    public boolean equals(@Nullable Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      CameraMoveUpdate that = (CameraMoveUpdate) o;

      if (Float.compare(that.x, x) != 0) {
        return false;
      }
      return Float.compare(that.y, y) == 0;
    }

    @Override
    public int hashCode() {
      int result = (x != +0.0f ? Float.floatToIntBits(x) : 0);
      result = 31 * result + (y != +0.0f ? Float.floatToIntBits(y) : 0);
      return result;
    }

    @Override
    public String toString() {
      return "CameraMoveUpdate{"
        + "x=" + x
        + ", y=" + y
        + '}';
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

    @Override
    public boolean equals(@Nullable Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      ZoomUpdate that = (ZoomUpdate) o;

      if (type != that.type) {
        return false;
      }
      if (Double.compare(that.zoom, zoom) != 0) {
        return false;
      }
      if (Float.compare(that.x, x) != 0) {
        return false;
      }
      return Float.compare(that.y, y) == 0;
    }

    @Override
    public int hashCode() {
      int result;
      long temp;
      result = type;
      temp = Double.doubleToLongBits(zoom);
      result = 31 * result + (int) (temp ^ (temp >>> 32));
      result = 31 * result + (x != +0.0f ? Float.floatToIntBits(x) : 0);
      result = 31 * result + (y != +0.0f ? Float.floatToIntBits(y) : 0);
      return result;
    }

    @Override
    public String toString() {
      return "ZoomUpdate{"
        + "type=" + type
        + ", zoom=" + zoom
        + ", x=" + x
        + ", y=" + y
        + '}';
    }
  }
}
