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

  private final MapState mapState;
  private final float height;
  private final float width;

  private int[] contentPadding = new int[] {0, 0, 0, 0};

  Projection(@NonNull MapState mapState, float width, float height) {
    this.mapState = mapState;
    this.width = width;
    this.height = height;
  }

  void setContentPadding(int[] contentPadding, int[] userLocationViewPadding) {
    this.contentPadding = contentPadding;

    final int[] padding = new int[] {
      contentPadding[0] + userLocationViewPadding[0],
      contentPadding[1] + userLocationViewPadding[1],
      contentPadding[2] + userLocationViewPadding[2],
      contentPadding[3] + userLocationViewPadding[3]
    };

    mapState.queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setContentPadding(padding);
      }
    });
  }

  int[] getContentPadding() {
    return contentPadding;
  }

  public void invalidateContentPadding(int[] userLocationViewPadding) {
    setContentPadding(contentPadding, userLocationViewPadding);
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
  public void getMetersPerPixelAtLatitude(@FloatRange(from = -90, to = 90) final double latitude,
                                          final Callback<Double> listener) {
    mapState.queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final double metersPerPixel = nativeMapView.getMetersPerPixelAtLatitude(latitude);
        mapState.queueUiEvent(new Runnable() {
          @Override
          public void run() {
            listener.onResult(metersPerPixel);
          }
        });
      }
    });
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
  public void fromScreenLocation(final PointF point, final Callback<LatLng> listener) {
    mapState.queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final LatLng latLng = nativeMapView.latLngForPixel(point);
        mapState.queueUiEvent(new Runnable() {
          @Override
          public void run() {
            listener.onResult(latLng);
          }
        });
      }
    });
  }

  /**
   * Gets a projection of the viewing frustum for converting between screen coordinates and
   * geo-latitude/longitude coordinates.
   *
   * @return The projection of the viewing frustum in its current state.
   */
  public void getVisibleRegion(final Callback<VisibleRegion> listener) {
    mapState.queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        LatLngBounds.Builder builder = new LatLngBounds.Builder();

        final float left = contentPadding[0];
        float right = nativeMapView.getWidth() - contentPadding[2];
        float top = contentPadding[1];
        float bottom = nativeMapView.getHeight() - contentPadding[3];

        LatLng topLeft = nativeMapView.latLngForPixel(new PointF(left, top));
        LatLng topRight = nativeMapView.latLngForPixel(new PointF(right, top));
        LatLng bottomRight = nativeMapView.latLngForPixel(new PointF(right, bottom));
        LatLng bottomLeft = nativeMapView.latLngForPixel(new PointF(left, bottom));

        LatLngBounds bounds = builder.include(topLeft)
          .include(topRight)
          .include(bottomRight)
          .include(bottomLeft)
          .build();

        final VisibleRegion visibleRegion = new VisibleRegion(topLeft, topRight, bottomLeft, bottomRight, bounds);
        mapState.queueUiEvent(new Runnable() {
          @Override
          public void run() {
            listener.onResult(visibleRegion);
          }
        });
      }
    });
  }

  /**
   * Returns a screen location that corresponds to a geographical coordinate (LatLng).
   * The screen location is in screen pixels (not display pixels) relative to the top left
   * of the map (not of the whole screen).
   *
   * @param location A LatLng on the map to convert to a screen location.
   * @return A Point representing the screen location in screen pixels.
   */
  public void toScreenLocation(final LatLng location, final Callback<PointF> listener) {
    mapState.queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        final PointF pointF = nativeMapView.pixelForLatLng(location);
        mapState.queueUiEvent(new Runnable() {
          @Override
          public void run() {
            listener.onResult(pointF);
          }
        });
      }
    });
  }

  float getHeight() {
    return height;
  }

  float getWidth() {
    return width;
  }
}
