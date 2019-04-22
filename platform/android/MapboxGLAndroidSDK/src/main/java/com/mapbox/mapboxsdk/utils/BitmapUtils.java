package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.PorterDuff;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.annotation.ColorInt;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.VisibleForTesting;
import android.view.View;

import java.io.ByteArrayOutputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

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

  /**
   * Extract an underlying bitmap from a drawable
   *
   * @param sourceDrawable The source drawable
   * @return The underlying bitmap
   */
  @Nullable
  public static Bitmap getBitmapFromDrawable(@Nullable Drawable sourceDrawable) {
    if (sourceDrawable == null) {
      return null;
    }

    if (sourceDrawable instanceof BitmapDrawable) {
      return ((BitmapDrawable) sourceDrawable).getBitmap();
    } else {
      //copying drawable object to not manipulate on the same reference
      Drawable.ConstantState constantState = sourceDrawable.getConstantState();
      if (constantState == null) {
        return null;
      }
      Drawable drawable = constantState.newDrawable().mutate();

      Bitmap bitmap = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(),
        Bitmap.Config.ARGB_8888);
      Canvas canvas = new Canvas(bitmap);
      drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
      drawable.draw(canvas);
      return bitmap;
    }
  }

  /**
   * Create a byte array out of drawable
   *
   * @param drawable The source drawable
   * @return The byte array of source drawable
   */
  @Nullable
  public static byte[] getByteArrayFromDrawable(@Nullable Drawable drawable) {
    if (drawable == null) {
      return null;
    }

    Bitmap bitmap = getBitmapFromDrawable(drawable);
    if (bitmap == null) {
      return null;
    }
    ByteArrayOutputStream stream = new ByteArrayOutputStream();
    bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream);
    return stream.toByteArray();
  }

  /**
   * Decode byte array to drawable object
   *
   * @param context Context to obtain {@link android.content.res.Resources}
   * @param array   The source byte array
   * @return The drawable created from source byte array
   */
  @Nullable
  public static Drawable getDrawableFromByteArray(@NonNull Context context, @Nullable byte[] array) {
    if (array == null) {
      return null;
    }
    Bitmap compass = BitmapFactory.decodeByteArray(array, 0, array.length);
    return new BitmapDrawable(context.getResources(), compass);
  }

  /**
   * Get a drawable from a resource.
   *
   * @param context     Context to obtain {@link android.content.res.Resources}
   * @param drawableRes Drawable resource
   * @return The drawable created from the resource
   */
  @Nullable
  public static Drawable getDrawableFromRes(@NonNull Context context, @DrawableRes int drawableRes) {
    return getDrawableFromRes(context, drawableRes, null);
  }

  /**
   * Get a tinted drawable from a resource.
   *
   * @param context     Context to obtain {@link android.content.res.Resources}
   * @param drawableRes Drawable resource
   * @param tintColor   Tint color
   * @return The drawable created from the resource
   */
  @Nullable
  public static Drawable getDrawableFromRes(@NonNull Context context, @DrawableRes int drawableRes,
                                            @Nullable @ColorInt Integer tintColor) {
    Drawable drawable = context.getResources().getDrawable(drawableRes);
    if (drawable == null) {
      return null;
    }

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

  /**
   * Validates if the bytes of a bitmap matches another
   *
   * @param bitmap the bitmap to be compared against
   * @param other  the bitmap to compare with
   * @return true if equal
   */
  @VisibleForTesting
  public static boolean equals(Bitmap bitmap, Bitmap other) {
    ByteBuffer buffer = ByteBuffer.allocate(bitmap.getHeight() * bitmap.getRowBytes());
    bitmap.copyPixelsToBuffer(buffer);

    ByteBuffer bufferOther = ByteBuffer.allocate(other.getHeight() * other.getRowBytes());
    other.copyPixelsToBuffer(bufferOther);

    return Arrays.equals(buffer.array(), bufferOther.array());
  }

}
