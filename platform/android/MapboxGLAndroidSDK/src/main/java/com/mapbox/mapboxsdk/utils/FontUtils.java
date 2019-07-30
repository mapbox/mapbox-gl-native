package com.mapbox.mapboxsdk.utils;

import android.graphics.Typeface;

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

    List<String> validFonts = getAvailableFonts();
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

  private static List<String> getAvailableFonts() {
    List<String> fonts = new ArrayList<>();
    try {
      Typeface typeface = Typeface.create(Typeface.DEFAULT, Typeface.NORMAL);
      Field f = Typeface.class.getDeclaredField("sSystemFontMap");
      f.setAccessible(true);
      Map<String, Typeface> fontMap = (Map<String, Typeface>) f.get(typeface);
      fonts.addAll(fontMap.keySet());
    } catch (Exception exception) {
      Logger.e(TAG,"Couldn't load fonts from Typeface", exception);
      MapStrictMode.strictModeViolation("Couldn't load fonts from Typeface", exception);
    }
    return fonts;
  }
}