package com.mapbox.mapboxsdk.utils;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.support.annotation.NonNull;
import android.view.View;

/**
 * Utility class for creating bitmaps
 */
public class BitmapUtils {

  /**
   * Convert a view to a bitmap.
   *
   * @param view the view to convert
   * @return the converted bitmap
   */
  public static Bitmap createBitmapFromView(@NonNull View view) {
    view.setDrawingCacheEnabled(true);
    view.setDrawingCacheQuality(View.DRAWING_CACHE_QUALITY_LOW);
    view.buildDrawingCache();

    if (view.getDrawingCache() == null) {
      return null;
    }

    Bitmap snapshot = Bitmap.createBitmap(view.getDrawingCache());
    view.setDrawingCacheEnabled(false);
    view.destroyDrawingCache();
    return snapshot;
  }

  /**
   * Create a bitmap from a background and a foreground bitmap
   *
   * @param background The bitmap placed in the background
   * @param foreground The bitmap placed in the foreground
   * @return the merged bitmap
   */
  public static Bitmap mergeBitmap(@NonNull Bitmap background, @NonNull Bitmap foreground) {
    Bitmap result = Bitmap.createBitmap(background.getWidth(), background.getHeight(), background.getConfig());
    Canvas canvas = new Canvas(result);
    canvas.drawBitmap(background, 0f, 0f, null);
    canvas.drawBitmap(foreground, 10, 10, null);
    return result;
  }

}
