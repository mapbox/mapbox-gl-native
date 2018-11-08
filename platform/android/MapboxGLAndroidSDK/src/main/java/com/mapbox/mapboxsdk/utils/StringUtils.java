package com.mapbox.mapboxsdk.utils;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;

import java.text.Normalizer;

/**
 * String utility class used by core from jni.
 */
@Keep
class StringUtils {

  /**
   * Normalises String input and strip diacritics from it.
   *
   * @return normalised String with stripped diacritics.
   */
  @Keep
  @NonNull
  static String unaccent(@NonNull String value) {
    return Normalizer.normalize(value, Normalizer.Form.NFD)
      .replaceAll("(\\p{InCombiningDiacriticalMarks}"
        + "|\\p{InCombiningDiacriticalMarksForSymbols}"
        + "|\\p{InCombiningDiacriticalMarksSupplement})+", "");
  }
}
