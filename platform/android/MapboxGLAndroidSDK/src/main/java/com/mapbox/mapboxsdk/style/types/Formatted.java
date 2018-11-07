package com.mapbox.mapboxsdk.style.types;

import android.support.annotation.Keep;
import android.support.annotation.Nullable;
import android.support.annotation.VisibleForTesting;

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
  @VisibleForTesting(otherwise = VisibleForTesting.PROTECTED)
  public Formatted(FormattedSection[] formattedSections) {
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
}
