package com.mapbox.mapboxsdk.utils;

import android.graphics.Typeface;
import android.os.Build;
import android.support.annotation.RequiresApi;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.log.Logger;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import static com.mapbox.mapboxsdk.constants.MapboxConstants.DEFAULT_FONT;

/**
 * Utility class to select a font from a range of font names based on the availability of fonts on the device.
 */
public class FontUtils {

  private static final String TAG = "Mbgl-FontUtils";
  private static final String TYPEFACE_FONTMAP_FIELD_NAME = "sSystemFontMap";
  private static final List<String> DEFAULT_FONT_STACKS = new ArrayList<>();

  static {
    DEFAULT_FONT_STACKS.add("sans-serif");
    DEFAULT_FONT_STACKS.add("serif");
    DEFAULT_FONT_STACKS.add("monospace");
  }

  private FontUtils() {
    // no instance
  }

  /**
   * Select a font from a range of font names to match the availability of fonts on the device.
   *
   * @param fontNames the range of font names to select from
   * @return the selected fon
   */
  public static String extractValidFont(String... fontNames) {
    if (fontNames == null) {
      return null;
    }

    List<String> validFonts;
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
      validFonts = getDeviceFonts();
    } else {
      validFonts = DEFAULT_FONT_STACKS;
    }

    for (String fontName : fontNames) {
      if (validFonts.contains(fontName)) {
        return fontName;
      }
    }

    Logger.i(TAG, String.format(
      "Couldn't map font family for local ideograph, using %s instead", DEFAULT_FONT)
    );
    return DEFAULT_FONT;
  }

  @SuppressWarnings( {"JavaReflectionMemberAccess", "unchecked"})
  @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
  private static List<String> getDeviceFonts() {
    List<String> fonts = new ArrayList<>();
    try {
      Typeface typeface = Typeface.create(Typeface.DEFAULT, Typeface.NORMAL);
      Field field = Typeface.class.getDeclaredField(TYPEFACE_FONTMAP_FIELD_NAME);
      field.setAccessible(true);
      Map<String, Typeface> fontMap = (Map<String, Typeface>) field.get(typeface);
      fonts.addAll(fontMap.keySet());
    } catch (Exception exception) {
      Logger.e(TAG, "Couldn't load fonts from Typeface", exception);
      MapStrictMode.strictModeViolation("Couldn't load fonts from Typeface", exception);
    }
    return fonts;
  }
}