package com.mapbox.mapboxsdk.text;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Bitmap;
import android.graphics.PorterDuff;
import android.graphics.Typeface;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.WorkerThread;

/**
 * LocalGlyphRasterizer is the Android-specific platform implementation used
 * by the portable local_glyph_rasterizer.hpp
 */
public class LocalGlyphRasterizer {
  private final Bitmap bitmap;
  @NonNull
  private final Paint paint;
  @NonNull
  private final Canvas canvas;

  LocalGlyphRasterizer() {
    /*
      35x35px dimensions are hardwired to match local_glyph_rasterizer.cpp
      These dimensions are large enough to draw a 24 point character in the middle
      of the bitmap (y: 20) with some buffer around the edge
    */
    bitmap = Bitmap.createBitmap(35, 35, Bitmap.Config.ARGB_8888);

    paint = new Paint();
    paint.setAntiAlias(true);
    paint.setTextSize(24);

    canvas = new Canvas();
    canvas.setBitmap(bitmap);
  }

  /***
   * Uses Android-native drawing code to rasterize a single glyph
   * to a square {@link Bitmap} which can be returned to portable
   * code for transformation into a Signed Distance Field glyph.
   *
   * @param fontFamily Font family string to pass to Typeface.create
   * @param bold If true, use Typeface.BOLD option
   * @param glyphID 16-bit Unicode BMP codepoint to draw
   *
   * @return Return a {@link Bitmap} to be displayed in the requested tile.
   */
  @WorkerThread
  @Keep
  protected Bitmap drawGlyphBitmap(String fontFamily, boolean bold, char glyphID) {
    paint.setTypeface(Typeface.create(fontFamily, bold ? Typeface.BOLD : Typeface.NORMAL));
    canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
    canvas.drawText(String.valueOf(glyphID), 0, 20, paint);
    return bitmap;
  }
}
