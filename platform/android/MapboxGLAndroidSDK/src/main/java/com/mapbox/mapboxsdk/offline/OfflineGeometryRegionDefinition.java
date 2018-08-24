package com.mapbox.mapboxsdk.offline;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.Keep;

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
 */
public class OfflineGeometryRegionDefinition implements OfflineRegionDefinition, Parcelable {

  @Keep
  private String styleURL;
  @Keep
  private Geometry geometry;
  @Keep
  private double minZoom;
  @Keep
  private double maxZoom;
  @Keep
  private float pixelRatio;

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
    // Note: Also used in JNI
    this.styleURL = styleURL;
    this.geometry = geometry;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
    this.pixelRatio = pixelRatio;
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
  }

  /*
   * Getters
   */

  public String getStyleURL() {
    return styleURL;
  }

  public Geometry getGeometry() {
    return geometry;
  }

  /**
   * Calculates the bounding box for the Geometry it contains
   * to retain backwards compatibility
   * @return the {@link LatLngBounds} or null
   */
  @Override
  public LatLngBounds getBounds() {
    if (geometry == null) {
      return null;
    }

    double[] bbox = TurfMeasurement.bbox(geometry);
    return LatLngBounds.from(bbox[3], bbox[2], bbox[1], bbox[0]);
  }

  public double getMinZoom() {
    return minZoom;
  }

  public double getMaxZoom() {
    return maxZoom;
  }

  public float getPixelRatio() {
    return pixelRatio;
  }

  /*
   * Parceable
   */

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
    dest.writeString(styleURL);
    dest.writeString(Feature.fromGeometry(geometry).toJson());
    dest.writeDouble(minZoom);
    dest.writeDouble(maxZoom);
    dest.writeFloat(pixelRatio);
  }

  public static final Creator CREATOR = new Creator() {
    public OfflineGeometryRegionDefinition createFromParcel(Parcel in) {
      return new OfflineGeometryRegionDefinition(in);
    }

    public OfflineGeometryRegionDefinition[] newArray(int size) {
      return new OfflineGeometryRegionDefinition[size];
    }
  };
}
