package com.mapbox.mapboxsdk.offline;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;

/**
 * An offline region defined by a style URL, geographic bounding box, zoom range, and
 * device pixel ratio.
 * <p>
 * Both minZoom and maxZoom must be &#x2265; 0, and maxZoom must be &#x2265; minZoom.
 * <p>
 * maxZoom may be &#x221E;, in which case for each tile source, the region will include
 * tiles from minZoom up to the maximum zoom level provided by that source.
 * <p>
 * pixelRatio must be &#x2265; 0 and should typically be 1.0 or 2.0.
 * <p>
 * if includeIdeographs is false, offline region will not include CJK glyphs
 */
public class OfflineTilePyramidRegionDefinition implements OfflineRegionDefinition {

  @Keep
  private String styleURL;
  @Keep
  private LatLngBounds bounds;
  @Keep
  private double minZoom;
  @Keep
  private double maxZoom;
  @Keep
  private float pixelRatio;
  @Keep
  private boolean includeIdeographs;

  /**
   * Constructor to create an OfflineTilePyramidDefinition from parameters.
   *
   * @param styleURL   the style
   * @param bounds     the bounds
   * @param minZoom    min zoom
   * @param maxZoom    max zoom
   * @param pixelRatio pixel ratio of the device
   */
  @Keep
  public OfflineTilePyramidRegionDefinition(
      String styleURL, LatLngBounds bounds, double minZoom, double maxZoom, float pixelRatio) {
    this(styleURL, bounds, minZoom, maxZoom, pixelRatio, false);
  }

  /**
   * Constructor to create an OfflineTilePyramidDefinition from parameters.
   *
   * @param styleURL   the style
   * @param bounds     the bounds
   * @param minZoom    min zoom
   * @param maxZoom    max zoom
   * @param pixelRatio pixel ratio of the device
   * @param includeIdeographs include glyphs for CJK languages
   */
  @Keep
  public OfflineTilePyramidRegionDefinition(
    String styleURL, LatLngBounds bounds, double minZoom, double maxZoom, float pixelRatio,
    boolean includeIdeographs) {
    // Note: Also used in JNI
    this.styleURL = styleURL;
    this.bounds = bounds;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
    this.pixelRatio = pixelRatio;
    this.includeIdeographs = includeIdeographs;
  }

  /**
   * Constructor to create an OfflineTilePyramidDefinition from a Parcel.
   *
   * @param parcel the parcel to create the OfflineTilePyramidDefinition from
   */
  public OfflineTilePyramidRegionDefinition(Parcel parcel) {
    this.styleURL = parcel.readString();
    this.bounds = new LatLngBounds.Builder()
      .include(new LatLng(parcel.readDouble(), parcel.readDouble()))
      .include(new LatLng(parcel.readDouble(), parcel.readDouble()))
      .build();
    this.minZoom = parcel.readDouble();
    this.maxZoom = parcel.readDouble();
    this.pixelRatio = parcel.readFloat();
    this.includeIdeographs = parcel.readByte() != 0;
  }


  @Override
  public String getStyleURL() {
    return styleURL;
  }

  @Override
  public LatLngBounds getBounds() {
    return bounds;
  }

  @Override
  public double getMinZoom() {
    return minZoom;
  }

  @Override
  public double getMaxZoom() {
    return maxZoom;
  }

  @Override
  public float getPixelRatio() {
    return pixelRatio;
  }

  @Override
  public boolean getIncludeIdeographs() {
    return includeIdeographs;
  }

  @NonNull
  @Override
  public String getType() {
    return "tileregion";
  }

  /*
   * Parceable
   */

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(@NonNull Parcel dest, int flags) {
    dest.writeString(styleURL);
    dest.writeDouble(bounds.getLatNorth());
    dest.writeDouble(bounds.getLonEast());
    dest.writeDouble(bounds.getLatSouth());
    dest.writeDouble(bounds.getLonWest());
    dest.writeDouble(minZoom);
    dest.writeDouble(maxZoom);
    dest.writeFloat(pixelRatio);
    dest.writeByte((byte) (includeIdeographs ? 1 : 0));
  }

  public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
    public OfflineTilePyramidRegionDefinition createFromParcel(@NonNull Parcel in) {
      return new OfflineTilePyramidRegionDefinition(in);
    }

    public OfflineTilePyramidRegionDefinition[] newArray(int size) {
      return new OfflineTilePyramidRegionDefinition[size];
    }
  };
}
