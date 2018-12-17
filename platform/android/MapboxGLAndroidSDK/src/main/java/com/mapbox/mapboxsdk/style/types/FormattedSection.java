package com.mapbox.mapboxsdk.style.types;

import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

/**
 * A component of the {@link Formatted}.
 */
@Keep
public class FormattedSection {
  private final String text;
  private final Number fontScale;
  private final String[] fontStack;

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontScale scale of the font, setting to null will fall back to style's default settings
   * @param fontStack main and fallback fonts that are a part of the style,
   *                  setting null will fall back to style's default settings
   */
  public FormattedSection(@NonNull String text, @Nullable Number fontScale, @Nullable String[] fontStack) {
    this.text = text;
    this.fontScale = fontScale;
    this.fontStack = fontStack;
  }

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontScale scale of the font, setting to null will fall back to style's default settings
   */
  public FormattedSection(@NonNull String text, @Nullable Number fontScale) {
    this(text, fontScale, null);
  }

  /**
   * Creates a formatted section.
   *
   * @param text displayed string
   */
  public FormattedSection(@NonNull String text) {
    this(text, null, null);
  }

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontStack main and fallback fonts that are a part of the style,
   *                  setting null will fall back to style's default settings
   */
  public FormattedSection(@NonNull String text, @Nullable String[] fontStack) {
    this(text, null, fontStack);
  }

  /**
   * Returns the displayed text.
   *
   * @return text
   */
  @NonNull
  public String getText() {
    return text;
  }

  /**
   * Returns displayed text's font scale.
   *
   * @return font scale
   */
  @Nullable
  public Number getFontScale() {
    return fontScale;
  }

  /**
   * Returns the font stack with main and fallback fonts.
   *
   * @return font stack
   */
  @Nullable
  public String[] getFontStack() {
    return fontStack;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    FormattedSection that = (FormattedSection) o;

    if (text != null ? !text.equals(that.text) : that.text != null) {
      return false;
    }
    if (fontScale != null ? !fontScale.equals(that.fontScale) : that.fontScale != null) {
      return false;
    }
    // Probably incorrect - comparing Object[] arrays with Arrays.equals
    return Arrays.equals(fontStack, that.fontStack);
  }

  @Override
  public int hashCode() {
    int result = text != null ? text.hashCode() : 0;
    result = 31 * result + (fontScale != null ? fontScale.hashCode() : 0);
    result = 31 * result + Arrays.hashCode(fontStack);
    return result;
  }

  Object[] toArray() {
    Map<String, Object> params = new HashMap<>();
    params.put("font-scale", fontScale);
    params.put("text-font", fontStack);
    return new Object[] {text, params};
  }
}
