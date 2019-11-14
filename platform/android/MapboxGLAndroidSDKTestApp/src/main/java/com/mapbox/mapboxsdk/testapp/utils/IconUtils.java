package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import androidx.annotation.ColorInt;
import androidx.annotation.DrawableRes;
import androidx.annotation.NonNull;
import androidx.core.content.res.ResourcesCompat;
import androidx.core.graphics.drawable.DrawableCompat;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;

public class IconUtils {

  /**
   * Demonstrates converting any Drawable to an Icon, for use as a marker icon.
   */
  public static Icon drawableToIcon(@NonNull Context context, @DrawableRes int id, @ColorInt int colorRes) {
    Drawable vectorDrawable = ResourcesCompat.getDrawable(context.getResources(), id, context.getTheme());
    Bitmap bitmap = Bitmap.createBitmap(vectorDrawable.getIntrinsicWidth(),
      vectorDrawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(bitmap);
    vectorDrawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
    DrawableCompat.setTint(vectorDrawable, colorRes);
    vectorDrawable.draw(canvas);
    return IconFactory.getInstance(context).fromBitmap(bitmap);
  }
}
