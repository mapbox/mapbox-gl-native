package com.mapbox.mapboxsdk.offline;

import android.os.Parcel;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.Geometry;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.turf.TurfMeasurement;

/**
 * An offline region defined by a style URL, geometry, zoom range, and
 * device pixel ratio.
 * <p>
 * Both minZoom and maxZoom must be ≥ 0, and maxZoom must be ≥ minZoom.
 * <p>
 * maxZoom may be ∞, in which case for each tile source, the region will include
 * tiles from minZoom up to the maximum zoom level provided by that source.
 * <p>
 * pixelRatio must be ≥ 0 and should typically be 1.0 or 2.0.
 * <p>
 * if includeIdeographs is false, offline region will not include CJK glyphs
 */
public class OfflineGeometryRegionDefinition implements OfflineRegionDefinition {

  @Keep
  private String styleURL;
  @Nullable
  @Keep
  private Geometry geometry;
  @Keep
  private double minZoom;
  @Keep
  private double maxZoom;
  @Keep
  private float pixelRatio;
  @Keep
  private boolean includeIdeographs;

  /**
   * Constructor to create an OfflineGeometryRegionDefinition from parameters.
   *
   * @param styleURL   the style
   * @param geometry   the geometry
   * @param minZoom    min zoom
   * @param maxZoom    max zoom
   * @param pixelRatio pixel ratio of the device
   */
  @Keep
  public OfflineGeometryRegionDefinition(
      String styleURL, Geometry geometry, double minZoom, double maxZoom, float pixelRatio) {
    this(styleURL, geometry, minZoom, maxZoom, pixelRatio, false);
  }

  /**
   * Constructor to create an OfflineGeometryRegionDefinition from parameters.
   *
   * @param styleURL   the style
   * @param geometry   the geometry
   * @param minZoom    min zoom
   * @param maxZoom    max zoom
   * @param pixelRatio pixel ratio of the device
   * @param includeIdeographs include glyphs for CJK languages
   */
  @Keep
  public OfflineGeometryRegionDefinition(
    String styleURL, Geometry geometry, double minZoom, double maxZoom, float pixelRatio,
    boolean includeIdeographs) {
    // Note: Also used in JNI
    this.styleURL = styleURL;
    this.geometry = geometry;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
    this.pixelRatio = pixelRatio;
    this.includeIdeographs = includeIdeographs;
  }

  /**
   * Constructor to create an OfflineGeometryRegionDefinition from a Parcel.
   *
   * @param parcel the parcel to create the OfflineGeometryRegionDefinition from
   */
  public OfflineGeometryRegionDefinition(Parcel parcel) {
    this.styleURL = parcel.readString();
    this.geometry = Feature.fromJson(parcel.readString()).geometry();
    this.minZoom = parcel.readDouble();
    this.maxZoom = parcel.readDouble();
    this.pixelRatio = parcel.readFloat();
    this.includeIdeographs = parcel.readByte() != 0;
  }

  @Override
  public String getStyleURL() {
    return styleURL;
  }

  @Nullable
  public Geometry getGeometry() {
    return geometry;
  }

  /**
   * Calculates the bounding box for the Geometry it contains
   * to retain backwards compatibility
   *
   * @return the {@link LatLngBounds} or null
   */
  @Nullable
  @Override
  public LatLngBounds getBounds() {
    if (geometry == null) {
      return null;
    }

    double[] bbox = TurfMeasurement.bbox(geometry);
    return LatLngBounds.from(bbox[3], bbox[2], bbox[1], bbox[0]);
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
    return "shaperegion";
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
    dest.writeString(Feature.fromGeometry(geometry).toJson());
    dest.writeDouble(minZoom);
    dest.writeDouble(maxZoom);
    dest.writeFloat(pixelRatio);
    dest.writeByte((byte) (includeIdeographs ? 1 : 0));
  }

  public static final Creator CREATOR = new Creator() {
    public OfflineGeometryRegionDefinition createFromParcel(@NonNull Parcel in) {
      return new OfflineGeometryRegionDefinition(in);
    }

    public OfflineGeometryRegionDefinition[] newArray(int size) {
      return new OfflineGeometryRegionDefinition[size];
    }
  };
}
