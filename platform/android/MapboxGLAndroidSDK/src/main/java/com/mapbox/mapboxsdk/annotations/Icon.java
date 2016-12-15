package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.maps.MapView;

/**
 * Icon is the visual representation of a {@link Marker} on a {@link MapView}.
 *
 * @see Marker
 */
public class Icon {
  private Bitmap bitmap;
  private String id;

  Icon(String id, Bitmap bitmap) {
    this.id = id;
    this.bitmap = bitmap;
  }

  /**
   * {@link String} identifier for this {@link Icon}.
   *
   * @return {@link String} identifier for this {@link Icon}.
   */
  public String getId() {
    return id;
  }

  /**
   * Get the {@link Bitmap} being used for this {@link Icon}.
   *
   * @return The {@link Bitmap} being used for the {@link Icon}.
   */
  public Bitmap getBitmap() {
    return bitmap;
  }

  /**
   * Compares this {@link Icon} object with another {@link Icon} and determines if they match.
   *
   * @param object Another {@link Icon} to compare with this object.
   * @return True if the {@link Icon} being passed in matches this {@link Icon} object. Else,
   * false.
   */
  @Override
  public boolean equals(Object object) {
    if (this == object) {
      return true;
    }
    if (object == null || getClass() != object.getClass()) {
      return false;
    }

    Icon icon = (Icon) object;

    if (!bitmap.equals(icon.bitmap)) {
      return false;
    }
    return id.equals(icon.id);
  }

  /**
   * Gives an integer which can be used as the bucket number for storing elements of the set/map.
   * This bucket number is the address of the element inside the set/map. There's no guarantee
   * that this hash value will be consistent between different Java implementations, or even
   * between different execution runs of the same program.
   *
   * @return integer value you can use for storing element.
   */
  @Override
  public int hashCode() {
    int result = 0;
    if (bitmap != null) {
      result = bitmap.hashCode();
    }
    if (id != null) {
      result = 31 * result + id.hashCode();
    }
    return result;
  }
}
