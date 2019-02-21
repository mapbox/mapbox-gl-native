package com.mapbox.mapboxsdk.location;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.INSTANT_LOCATION_TRANSITION_THRESHOLD;

public final class Utils {

  private Utils() {
    // Class should not be initialized
  }

  /**
   * Util for finding the shortest path from the current rotated degree to the new degree.
   *
   * @param heading         the new position of the rotation
   * @param previousHeading the current position of the rotation
   * @return the shortest degree of rotation possible
   */
  public static float shortestRotation(float heading, float previousHeading) {
    double diff = previousHeading - heading;
    if (diff > 180.0f) {
      heading += 360.0f;
    } else if (diff < -180.0f) {
      heading -= 360.f;
    }
    return heading;
  }

  static Bitmap generateShadow(Drawable drawable, float elevation) {
    int width = drawable.getIntrinsicWidth();
    int height = drawable.getIntrinsicHeight();
    Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(bitmap);
    drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
    drawable.draw(canvas);
    bitmap = Bitmap.createScaledBitmap(bitmap,
      toEven(width + elevation), toEven(height + elevation), false);
    return bitmap;
  }

  static float calculateZoomLevelRadius(@NonNull MapboxMap mapboxMap, @Nullable Location location) {
    if (location == null) {
      return 0;
    }
    double metersPerPixel = mapboxMap.getProjection().getMetersPerPixelAtLatitude(location.getLatitude());
    return (float) (location.getAccuracy() * (1 / metersPerPixel));
  }

  static boolean immediateAnimation(@NonNull Projection projection, @NonNull LatLng current, @NonNull LatLng target) {
    double metersPerPixel = projection.getMetersPerPixelAtLatitude((current.getLatitude() + target.getLatitude()) / 2);
    double distance = current.distanceTo(target);
    return distance / metersPerPixel > INSTANT_LOCATION_TRANSITION_THRESHOLD;
  }

  /**
   * Casts the value to an even integer.
   */
  private static int toEven(float value) {
    int i = (int) (value + .5f);
    if (i % 2 == 1) {
      return i - 1;
    }
    return i;
  }
}
