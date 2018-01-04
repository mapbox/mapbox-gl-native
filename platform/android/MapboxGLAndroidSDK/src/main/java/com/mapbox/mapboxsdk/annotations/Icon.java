package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.util.DisplayMetrics;

import java.nio.ByteBuffer;

/**
 * Icon is the visual representation of a Marker on a MapView.
 *
 * @see Marker
 * @see IconFactory
 */
public class Icon {

  private Bitmap mBitmap;
  private String mId;

  Icon(String id, Bitmap bitmap) {
    mId = id;
    mBitmap = bitmap;
  }

  /**
   * String identifier for this icon.
   *
   * @return String identifier for this icon.
   */
  public String getId() {
    return mId;
  }

  /**
   * Get the bitmap being used for this icon.
   *
   * @return The bitmap being used for the icon.
   */
  public Bitmap getBitmap() {
    if (mBitmap != null && mBitmap.getConfig() != Bitmap.Config.ARGB_8888) {
      mBitmap = mBitmap.copy(Bitmap.Config.ARGB_8888, false);
    }
    return mBitmap;
  }

  /**
   * Get the icon bitmap scale.
   * <p>
   * Requires the bitmap to be set before calling this method.
   * </p>
   *
   * @return the scale of the bitmap
   */
  public float getScale() {
    if (mBitmap == null) {
      throw new IllegalStateException("Required to set a Icon before calling getScale");
    }
    float density = mBitmap.getDensity();
    if (density == Bitmap.DENSITY_NONE) {
      density = DisplayMetrics.DENSITY_DEFAULT;
    }
    return density / DisplayMetrics.DENSITY_DEFAULT;
  }

  /**
   * Get the icon bitmap bytes.
   * <p>
   * Requires the bitmap to be set before calling this method.
   * </p>
   *
   * @return the bytes of the bitmap
   */
  public byte[] toBytes() {
    if (mBitmap == null) {
      throw new IllegalStateException("Required to set a Icon before calling toBytes");
    }
    ByteBuffer buffer = ByteBuffer.allocate(mBitmap.getRowBytes() * mBitmap.getHeight());
    mBitmap.copyPixelsToBuffer(buffer);
    return buffer.array();
  }

  /**
   * Compares this icon object with another icon and determines if they match.
   *
   * @param object Another iconi to compare with this object.
   * @return True if the icon being passed in matches this icon object. Else, false.
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
    return mBitmap.equals(icon.mBitmap) && mId.equals(icon.mId);
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
    if (mBitmap != null) {
      result = mBitmap.hashCode();
    }
    if (mId != null) {
      result = 31 * result + mId.hashCode();
    }
    return result;
  }
}
