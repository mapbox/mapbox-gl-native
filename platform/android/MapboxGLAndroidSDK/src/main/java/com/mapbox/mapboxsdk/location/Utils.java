package com.mapbox.mapboxsdk.location;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.LayerDrawable;
import android.location.Location;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.INSTANT_LOCATION_TRANSITION_THRESHOLD;

public final class Utils {

  private static final String TAG = "Mbgl-com.mapbox.mapboxsdk.location.Utils";

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

  /**
   * Normalizes an angle to be in the [0, 360] range.
   *
   * @param angle the provided angle
   * @return the normalized angle
   */
  public static float normalize(float angle) {
    return (angle % 360 + 360) % 360;
  }

  static Bitmap generateShadow(Drawable drawable, float elevation) {
    int width = drawable.getIntrinsicWidth();
    int height = drawable.getIntrinsicHeight();
    Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(bitmap);
    drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
    try {
      drawable.draw(canvas);
    } catch (IllegalArgumentException ex) {
      if (ex.getMessage().equals("radius must be > 0") && Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
        Logger.w(TAG,
          "Location's shadow gradient drawable has a radius <= 0px, resetting to 1px in order to avoid crashing");
        ensureShadowGradientRadius(drawable);
        return generateShadow(drawable, elevation);
      } else {
        throw ex;
      }
    }
    bitmap = Bitmap.createScaledBitmap(bitmap,
      toEven(width + elevation), toEven(height + elevation), false);
    return bitmap;
  }

  /**
   * We need to ensure that the radius of any {@link GradientDrawable} is greater than 0 for API levels < 21.
   *
   * @see <a href=https://github.com/mapbox/mapbox-gl-native/issues/15026>mapbox-gl-native-#15026</a>
   */
  private static void ensureShadowGradientRadius(Drawable drawable) {
    if (drawable instanceof GradientDrawable) {
      ((GradientDrawable) drawable).setGradientRadius(1);
    } else if (drawable instanceof LayerDrawable) {
      LayerDrawable layerDrawable = (LayerDrawable) drawable;
      for (int i = 0; i < layerDrawable.getNumberOfLayers(); i++) {
        Drawable layers = layerDrawable.getDrawable(i);
        if (layers instanceof GradientDrawable) {
          ((GradientDrawable) layers).setGradientRadius(1);
        }
      }
    }
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
