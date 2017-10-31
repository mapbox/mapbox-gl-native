package com.mapbox.mapboxsdk.snapshotter;

import android.graphics.Bitmap;
import android.graphics.PointF;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * A completed snapshot.
 *
 * @see MapSnapshotter
 */
public class MapSnapshot {

  private long nativePtr = 0;
  private Bitmap bitmap;
  private String[] attributions;

  /**
   * Created from native side
   */
  private MapSnapshot(long nativePtr, Bitmap bitmap, String[] attributions) {
    this.nativePtr = nativePtr;
    this.bitmap = bitmap;
    this.attributions = attributions;
  }

  /**
   * @return the bitmap
   */
  public Bitmap getBitmap() {
    return bitmap;
  }

  /**
   * Calculate the point in pixels on the Image from geographical coordinates.
   *
   * @param latLng the geographical coordinates
   * @return the point on the image
   */
  public native PointF pixelForLatLng(LatLng latLng);

  /**
   * @return The attributions for the sources of this snapshot.
   */
  protected String[] getAttributions() {
    return attributions;
  }

  // Unused, needed for peer binding
  private native void initialize();

  protected native void finalize();
}
