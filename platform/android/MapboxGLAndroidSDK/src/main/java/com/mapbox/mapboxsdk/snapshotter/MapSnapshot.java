package com.mapbox.mapboxsdk.snapshotter;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.support.annotation.Keep;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * A completed snapshot.
 *
 * @see MapSnapshotter
 */
public class MapSnapshot {

  @Keep
  private long nativePtr = 0;
  private Bitmap bitmap;
  private String[] attributions;
  private boolean showLogo;

  /**
   * Created from native side
   */
  @Keep
  private MapSnapshot(long nativePtr, Bitmap bitmap, String[] attributions, boolean showLogo) {
    this.nativePtr = nativePtr;
    this.bitmap = bitmap;
    this.attributions = attributions;
    this.showLogo = showLogo;
  }

  /**
   * @return the large
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
  @Keep
  public native PointF pixelForLatLng(LatLng latLng);

  /**
   * Calculate geographical coordinates from a point in pixels on the Image
   *
   * @param pointF the point in pixels
   * @return the geographical coordinates
   */
  @Keep
  public native LatLng latLngForPixel(PointF pointF);

  /**
   * @return The attributions for the sources of this snapshot.
   */
  protected String[] getAttributions() {
    return attributions;
  }

  /**
   * @return Flag indicating to show the Mapbox logo.
   */
  boolean isShowLogo() {
    return showLogo;
  }

  // Unused, needed for peer binding
  @Keep
  private native void initialize();

  @Keep
  protected native void finalize();
}
