package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.util.TypedValue;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.exceptions.ConversionException;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Color utility class.
 */
public class ColorUtils {

  /**
   * Returns a color integer associated as primary color from a theme based on a Context.
   *
   * @param context The context used to style the color attributes.
   * @return The primary color value of current theme in the form 0xAARRGGBB.
   */
  @ColorInt
  public static int getPrimaryColor(@NonNull Context context) {
    try {
      TypedValue typedValue = new TypedValue();
      Resources.Theme theme = context.getTheme();
      int id = context.getResources().getIdentifier("colorPrimary", "attrs", context.getPackageName());
      theme.resolveAttribute(id, typedValue, true);
      return typedValue.data;
    } catch (Exception exception) {
      return getColorCompat(context, R.color.mapbox_blue);
    }
  }

  /**
   * Returns a color integer associated as primary dark color from a theme based on a Context.
   *
   * @param context The context used to style the color attributes.
   * @return The primary dark color value of current theme in the form 0xAARRGGBB.
   */
  @ColorInt
  public static int getPrimaryDarkColor(@NonNull Context context) {
    try {
      TypedValue typedValue = new TypedValue();
      Resources.Theme theme = context.getTheme();
      int id = context.getResources().getIdentifier("colorPrimaryDark", "attrs", context.getPackageName());
      theme.resolveAttribute(id, typedValue, true);
      return typedValue.data;
    } catch (Exception exception) {
      return getColorCompat(context, R.color.mapbox_blue);
    }
  }

  /**
   * Returns a color integer associated as accent color from a theme based on a Context.
   *
   * @param context The context used to style the color attributes.
   * @return The accent color value of current theme in the form 0xAARRGGBB.
   */
  @ColorInt
  public static int getAccentColor(@NonNull Context context) {
    try {
      TypedValue typedValue = new TypedValue();
      Resources.Theme theme = context.getTheme();
      int id = context.getResources().getIdentifier("colorAccent", "attrs", context.getPackageName());
      theme.resolveAttribute(id, typedValue, true);
      return typedValue.data;
    } catch (Exception exception) {
      return getColorCompat(context, R.color.mapbox_gray);
    }
  }

  /**
   * Returns a color state list associated with a theme based on a Context.
   *
   * @param color The color used for tinting.
   * @return A ColorStateList object containing the primary color of a theme
   */
  @NonNull
  public static ColorStateList getSelector(@ColorInt int color) {
    return new ColorStateList(
      new int[][] {
        new int[] {android.R.attr.state_pressed},
        new int[] {}
      },
      new int[] {
        color,
        color
      }
    );
  }

  /**
   * Set a color tint list to the Drawable of an ImageView.
   *
   * @param imageView The view to set the default tint list.
   * @param tintColor The color to tint.
   */
  public static void setTintList(@NonNull ImageView imageView, @ColorInt int tintColor) {
    Drawable originalDrawable = imageView.getDrawable();
    Drawable wrappedDrawable = DrawableCompat.wrap(originalDrawable);
    DrawableCompat.setTintList(wrappedDrawable, getSelector(tintColor));
  }

  static int normalizeColorComponent(String value) {
    return (int) (Float.parseFloat(value) * 255);
  }

  /**
   * Convert an rgba string to a Color int.
   *
   * @param value the String representation of rgba
   * @return the int representation of rgba
   * @throws ConversionException on illegal input
   */
  @ColorInt
  public static int rgbaToColor(String value) {
    Pattern c = Pattern.compile("rgba?\\s*\\(\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,"
      + "?\\s*(\\d+\\.?\\d*)?\\s*\\)");
    Matcher m = c.matcher(value);
    if (m.matches() && m.groupCount() == 3) {
      return Color.rgb(normalizeColorComponent(m.group(1)), normalizeColorComponent(m.group(2)),
        normalizeColorComponent(m.group(3)));
    } else if (m.matches() && m.groupCount() == 4) {
      return Color.argb(normalizeColorComponent(m.group(4)), normalizeColorComponent(m.group(1)),
        normalizeColorComponent(m.group(2)), normalizeColorComponent(m.group(3)));
    } else {
      throw new ConversionException("Not a valid rgb/rgba value");
    }
  }

  private static int getColorCompat(Context context, int id) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      return context.getResources().getColor(id, context.getTheme());
    } else {
      return context.getResources().getColor(id);
    }
  }
}
