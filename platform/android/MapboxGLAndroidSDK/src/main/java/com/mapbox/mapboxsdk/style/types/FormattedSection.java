package com.mapbox.mapboxsdk.style.types;

import androidx.annotation.ColorInt;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.mapbox.mapboxsdk.utils.ColorUtils;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

/**
 * A component of the {@link Formatted}.
 */
@Keep
public class FormattedSection {
  private String text;
  private Number fontScale;
  private String[] fontStack;
  private String textColor;

  /**
   * Creates a formatted section.
   *
   * @param text displayed string
   */
  public FormattedSection(@NonNull String text) {
    this(text, null, null, null);
  }

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontScale scale of the font, setting to null will fall back to style's default settings
   * @param fontStack main and fallback fonts that are a part of the style,
   *                  setting null will fall back to style's default settings.
   *                  The requested font stack has to be a part of the used style.
   *                  For more information see
   *                  <a href="https://www.mapbox.com/help/define-font-stack/">the documentation</a>.
   * @param textColor text color, setting to null will fall back to style's default settings.
   *                  Value of red, green, blue components must range between 0 and 255,
   *                  an alpha component must range between 0 and 1.
   *                  <p>
   *                  For more information see
   *                  <a href="https://docs.mapbox.com/mapbox-gl-js/style-spec/#types-color">the documentation</a>.
   */
  public FormattedSection(@NonNull String text, @Nullable Number fontScale, @Nullable String[] fontStack,
                          @Nullable String textColor) {
    this.text = text;
    this.fontScale = fontScale;
    this.fontStack = fontStack;
    this.textColor = textColor;
  }

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontScale scale of the font, setting to null will fall back to style's default settings
   * @param fontStack main and fallback fonts that are a part of the style,
   *                  setting null will fall back to style's default settings
   * @deprecated use {@link #FormattedSection(String)} and setters
   * or {@link #FormattedSection(String, Number, String[], String)} instead
   */
  @Deprecated
  public FormattedSection(@NonNull String text, @Nullable Number fontScale, @Nullable String[] fontStack) {
    this(text, fontScale, fontStack, null);
  }

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontScale scale of the font, setting to null will fall back to style's default settings
   * @deprecated use {@link #FormattedSection(String)} and setters
   * or {@link #FormattedSection(String, Number, String[], String)} instead
   */
  @Deprecated
  public FormattedSection(@NonNull String text, @Nullable Number fontScale) {
    this(text, fontScale, null, null);
  }

  /**
   * Creates a formatted section.
   *
   * @param text      displayed string
   * @param fontStack main and fallback fonts that are a part of the style,
   *                  setting null will fall back to style's default settings
   * @deprecated use {@link #FormattedSection(String)} and setters
   * or {@link #FormattedSection(String, Number, String[], String)} instead
   */
  @Deprecated
  public FormattedSection(@NonNull String text, @Nullable String[] fontStack) {
    this(text, null, fontStack, null);
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

  /**
   * Returns the text color.
   *
   * @return text color
   */
  public String getTextColor() {
    return textColor;
  }

  /**
   * Set font scale. Setting to null will fall back to style's default settings.
   *
   * @param fontScale fontScale
   */
  public void setFontScale(@Nullable Number fontScale) {
    // called from JNI
    this.fontScale = fontScale;
  }

  /**
   * Set main and fallback fonts that are a part of the style. Setting null will fall back to style's default settings.
   * <p>
   * The requested font stack has to be a part of the used style.
   * For more information see <a href="https://www.mapbox.com/help/define-font-stack/">the documentation</a>.
   *
   * @param fontStack fontStack
   */
  public void setFontStack(@Nullable String[] fontStack) {
    // called from JNI
    this.fontStack = fontStack;
  }

  /**
   * Set text color. Setting to null will fall back to style's default settings.
   * Value of red, green, blue components must range between 0 and 255,
   * an alpha component must range between 0 and 1.
   * <p>
   * For more information see
   * <a href="https://docs.mapbox.com/mapbox-gl-js/style-spec/#types-color">the documentation</a>.
   *
   * @param textColor text color
   */
  public void setTextColor(@Nullable String textColor) {
    this.textColor = textColor;
  }

  /**
   * Set the text color.
   *
   * @param textColor text color.
   */
  public void setTextColor(@ColorInt int textColor) {
    this.textColor = ColorUtils.colorToRgbaString(textColor);
  }

  void setTextColor(@NonNull Object textColor) {
    // called from JNI
    setTextColor((String) textColor);
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
    if (!Arrays.equals(fontStack, that.fontStack)) {
      return false;
    }
    return textColor != null ? textColor.equals(that.textColor) : that.textColor == null;
  }

  @Override
  public int hashCode() {
    int result = text != null ? text.hashCode() : 0;
    result = 31 * result + (fontScale != null ? fontScale.hashCode() : 0);
    result = 31 * result + Arrays.hashCode(fontStack);
    result = 31 * result + (textColor != null ? textColor.hashCode() : 0);
    return result;
  }

  Object[] toArray() {
    Map<String, Object> params = new HashMap<>();
    params.put("font-scale", fontScale);
    params.put("text-font", fontStack);
    params.put("text-color", textColor);
    return new Object[] {text, params};
  }

  @Override
  public String toString() {
    return "FormattedSection{"
      + "text='" + text + '\''
      + ", fontScale=" + fontScale
      + ", fontStack=" + Arrays.toString(fontStack)
      + ", textColor='" + textColor + '\''
      + '}';
  }
}
