package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.PorterDuff;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.os.Build;
import android.support.annotation.ColorInt;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;

import com.mapbox.mapboxsdk.maps.MapboxMap;

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

  static Bitmap getBitmapFromDrawable(Drawable drawable) {
    if (drawable instanceof BitmapDrawable) {
      return ((BitmapDrawable) drawable).getBitmap();
    } else {
      // width and height are equal for all assets since they are ovals.
      Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(),
        drawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
      Canvas canvas = new Canvas(bitmap);
      drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
      drawable.draw(canvas);
      return bitmap;
    }
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

  static Drawable getDrawable(@NonNull Context context, @DrawableRes int drawableRes,
                              @ColorInt Integer tintColor) {
    Drawable drawable = ContextCompat.getDrawable(context, drawableRes);
    if (tintColor == null) {
      return drawable;
    }

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
      drawable.setTint(tintColor);
    } else {
      drawable.mutate().setColorFilter(tintColor, PorterDuff.Mode.SRC_IN);
    }
    return drawable;
  }

  static float calculateZoomLevelRadius(@NonNull MapboxMap mapboxMap, @Nullable Location location) {
    if (location == null) {
      return 0;
    }
    double metersPerPixel = mapboxMap.getProjection().getMetersPerPixelAtLatitude(
      location.getLatitude());
    return (float) (location.getAccuracy() * (1 / metersPerPixel));
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
