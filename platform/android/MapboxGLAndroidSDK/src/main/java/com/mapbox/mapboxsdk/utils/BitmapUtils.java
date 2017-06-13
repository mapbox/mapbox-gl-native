package com.mapbox.mapboxsdk.utils;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.support.annotation.NonNull;
import android.view.View;

public class BitmapUtils {

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

  public static Bitmap mergeBitmap(@NonNull Bitmap background, @NonNull Bitmap foreground) {
    Bitmap result = Bitmap.createBitmap(background.getWidth(), background.getHeight(), background.getConfig());
    Canvas canvas = new Canvas(result);
    canvas.drawBitmap(background, 0f, 0f, null);
    canvas.drawBitmap(foreground, 10, 10, null);
    return result;
  }

}
