package com.mapbox.mapboxsdk.style.types;

import androidx.annotation.Keep;
import androidx.annotation.Nullable;

import java.util.Arrays;

/**
 * Represents a string broken into sections annotated with separate formatting options.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#types-formatted">Style specification</a>
 */
@Keep
public class Formatted {
  private final FormattedSection[] formattedSections;

  /**
   * Create a new formatted text.
   *
   * @param formattedSections sections with formatting options
   */
  public Formatted(FormattedSection... formattedSections) {
    this.formattedSections = formattedSections;
  }

  /**
   * Returns sections with separate formatting options that are a part of this formatted text.
   *
   * @return formatted sections
   */
  public FormattedSection[] getFormattedSections() {
    return formattedSections;
  }

  public Object[] toArray() {
    Object[] sections = new Object[formattedSections.length];
    for (int i = 0; i < formattedSections.length; i++) {
      sections[i] = formattedSections[i].toArray();
    }
    return sections;
  }

  @Override
  public boolean equals(@Nullable Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    Formatted formatted = (Formatted) o;

    return Arrays.equals(formattedSections, formatted.formattedSections);
  }

  @Override
  public int hashCode() {
    return Arrays.hashCode(formattedSections);
  }

  @Override
  public String toString() {
    return "Formatted{"
      + "formattedSections="
      + Arrays.toString(formattedSections)
      + '}';
  }
}
