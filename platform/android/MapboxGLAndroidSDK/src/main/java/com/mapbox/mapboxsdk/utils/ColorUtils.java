package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.Color;
import android.os.Build;
import androidx.annotation.ColorInt;
import androidx.annotation.NonNull;
import androidx.core.widget.ImageViewCompat;
import android.util.TypedValue;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.exceptions.ConversionException;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Locale;
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
    ImageViewCompat.setImageTintList(imageView, getSelector(tintColor));
  }

  /**
   * Convert an rgba string to a Color int.
   * <p>
   * R, G, B color components have to be in the [0-255] range, while alpha has to be in the [0.0-1.0] range.
   * For example: "rgba(255, 128, 0, 0.7)".
   *
   * @param value the String representation of rgba
   * @return the int representation of rgba
   * @throws ConversionException on illegal input
   */
  @ColorInt
  public static int rgbaToColor(@NonNull String value) {
    // we need to accept and floor float values as well, as those can come from core
    Pattern c = Pattern.compile("rgba?\\s*\\(\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,\\s*(\\d+\\.?\\d*)\\s*,"
      + "?\\s*(\\d+\\.?\\d*)?\\s*\\)");
    Matcher m = c.matcher(value);
    if (m.matches() && m.groupCount() == 3) {
      return Color.rgb((int) Float.parseFloat(m.group(1)), (int) Float.parseFloat(m.group(2)),
        (int) Float.parseFloat(m.group(3)));
    } else if (m.matches() && m.groupCount() == 4) {
      return Color.argb((int) (Float.parseFloat(m.group(4)) * 255), (int) Float.parseFloat(m.group(1)),
        (int) Float.parseFloat(m.group(2)), (int) Float.parseFloat(m.group(3)));
    } else {
      throw new ConversionException("Not a valid rgb/rgba value");
    }
  }

  /**
   * Converts Android color int to "rbga(r, g, b, a)" String equivalent.
   * <p>
   * Alpha value will be converted from 0-255 range to 0-1.
   * </p>
   *
   * @param color Android color int
   * @return String rgba color
   */
  public static String colorToRgbaString(@ColorInt int color) {
    NumberFormat numberFormat = NumberFormat.getNumberInstance(Locale.US);
    DecimalFormat decimalFormat = (DecimalFormat) numberFormat;
    decimalFormat.applyPattern("#.###");
    String alpha = decimalFormat.format(((float) ((color >> 24) & 0xFF)) / 255.0f);
    return String.format(Locale.US, "rgba(%d, %d, %d, %s)",
      (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, alpha);
  }

  /**
   * Converts Android color int to rgba float array.
   * <p>
   * Returned RGB values range from 0 to 255.
   * Alpha value ranges from 0-1.
   * </p>
   *
   * @param color Android color int
   * @return float rgba array, rgb values range from 0-255, alpha from 0-1
   */
  public static float[] colorToRgbaArray(@ColorInt int color) {
    return new float[] {
      (color >> 16) & 0xFF,            // r (0-255)
      (color >> 8) & 0xFF,             // g (0-255)
      color & 0xFF,                    // b (0-255)
      ((color >> 24) & 0xFF) / 255.0f  // a (0-1)
    };
  }

  /**
   * Converts Android color int to GL rgba float array.
   * <p>
   * Returned values range from 0-1.
   * </p>
   *
   * @param color Android color int
   * @return float rgba array, values range from 0 to 1
   */
  public static float[] colorToGlRgbaArray(@ColorInt int color) {
    return new float[] {
      ((color >> 16) & 0xFF) / 255.0f, // r (0-1)
      ((color >> 8) & 0xFF) / 255.0f,  // g (0-1)
      (color & 0xFF) / 255.0f,         // b (0-1)
      ((color >> 24) & 0xFF) / 255.0f  // a (0-1)
    };
  }

  private static int getColorCompat(@NonNull Context context, int id) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      return context.getResources().getColor(id, context.getTheme());
    } else {
      return context.getResources().getColor(id);
    }
  }
}
