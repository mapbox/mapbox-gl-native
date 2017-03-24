package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;
import com.mapbox.services.android.telemetry.constants.GeoConstants;

import java.util.ArrayList;
import java.util.List;

/**
 * A geographical area representing a latitude/longitude aligned rectangle.
 * <p>
 * This class does not wrap values to the world bounds.
 * </p>
 */
public class LatLngBounds implements Parcelable {

  private final double mLatNorth;
  private final double mLatSouth;
  private final double mLonEast;
  private final double mLonWest;

  /**
   * Construct a new LatLngBounds based on its corners, given in NESW
   * order.
   *
   * @param northLatitude Northern Latitude
   * @param eastLongitude Eastern Longitude
   * @param southLatitude Southern Latitude
   * @param westLongitude Western Longitude
   */
  LatLngBounds(final double northLatitude, final double eastLongitude, final double southLatitude,
               final double westLongitude) {
    this.mLatNorth = northLatitude;
    this.mLonEast = eastLongitude;
    this.mLatSouth = southLatitude;
    this.mLonWest = westLongitude;
  }

  /**
   * Returns the world bounds.
   *
   * @return the bounds representing the world
   */
  public static LatLngBounds world() {
    return new LatLngBounds.Builder()
      .include(new LatLng(GeoConstants.MAX_LATITUDE, GeoConstants.MAX_LONGITUDE))
      .include(new LatLng(GeoConstants.MIN_LATITUDE, GeoConstants.MIN_LONGITUDE))
      .build();
  }

  /**
   * Calculates the centerpoint of this LatLngBounds by simple interpolation and returns
   * it as a point. This is a non-geodesic calculation which is not the geographic center.
   *
   * @return LatLng center of this LatLngBounds
   */
  public LatLng getCenter() {
    return new LatLng((this.mLatNorth + this.mLatSouth) / 2,
      (this.mLonEast + this.mLonWest) / 2);
  }

  /**
   * Get the north latitude value of this bounds.
   *
   * @return double latitude value for north
   */
  public double getLatNorth() {
    return this.mLatNorth;
  }

  /**
   * Get the south latitude value of this bounds.
   *
   * @return double latitude value for south
   */
  public double getLatSouth() {
    return this.mLatSouth;
  }

  /**
   * Get the east longitude value of this bounds.
   *
   * @return double longitude value for east
   */
  public double getLonEast() {
    return this.mLonEast;
  }

  /**
   * Get the west longitude value of this bounds.
   *
   * @return double longitude value for west
   */
  public double getLonWest() {
    return this.mLonWest;
  }

  /**
   * Get the latitude-longitude pair of the south west corner of this bounds.
   *
   * @return LatLng of the south west corner
   */
  public LatLng getSouthWest() {
    return new LatLng(mLatSouth, mLonWest);
  }

  /**
   * Get the latitude-longitude paur if the north east corner of this bounds.
   *
   * @return LatLng of the north east corner
   */
  public LatLng getNorthEast() {
    return new LatLng(mLatNorth, mLonEast);
  }

  /**
   * Get the latitude-longitude pair of the south east corner of this bounds.
   *
   * @return LatLng of the south east corner
   */
  public LatLng getSouthEast() {
    return new LatLng(mLatSouth, mLonEast);
  }

  /**
   * Get the latitude-longitude pair of the north west corner of this bounds.
   *
   * @return LatLng of the north west corner
   */
  public LatLng getNorthWest() {
    return new LatLng(mLatNorth, mLonWest);
  }

  /**
   * Get the area spanned by this LatLngBounds
   *
   * @return LatLngSpan area
   */
  public LatLngSpan getSpan() {
    return new LatLngSpan(getLatitudeSpan(), getLongitudeSpan());
  }

  /**
   * Get the absolute distance, in degrees, between the north and
   * south boundaries of this LatLngBounds
   *
   * @return Span distance
   */
  public double getLatitudeSpan() {
    return Math.abs(this.mLatNorth - this.mLatSouth);
  }

  /**
   * Get the absolute distance, in degrees, between the west and
   * east boundaries of this LatLngBounds
   *
   * @return Span distance
   */
  public double getLongitudeSpan() {
    return Math.abs(this.mLonEast - this.mLonWest);
  }


  /**
   * Validate if LatLngBounds is empty, determined if absolute distance is
   *
   * @return boolean indicating if span is empty
   */
  public boolean isEmptySpan() {
    return getLongitudeSpan() == 0.0 || getLatitudeSpan() == 0.0;
  }

  @Override
  public String toString() {
    return "N:" + this.mLatNorth + "; E:" + this.mLonEast + "; S:" + this.mLatSouth + "; W:" + this.mLonWest;
  }

  /**
   * Constructs a LatLngBounds that contains all of a list of LatLng
   * objects. Empty lists will yield invalid LatLngBounds.
   *
   * @param latLngs List of LatLng objects
   * @return LatLngBounds
   */
  static LatLngBounds fromLatLngs(final List<? extends ILatLng> latLngs) {
    double minLat = 90;
    double minLon = 180;
    double maxLat = -90;
    double maxLon = -180;

    for (final ILatLng gp : latLngs) {
      final double latitude = gp.getLatitude();
      final double longitude = gp.getLongitude();

      minLat = Math.min(minLat, latitude);
      minLon = Math.min(minLon, longitude);
      maxLat = Math.max(maxLat, latitude);
      maxLon = Math.max(maxLon, longitude);
    }

    return new LatLngBounds(maxLat, maxLon, minLat, minLon);
  }

  /**
   * Return an array of LatLng objects resembling this bounds.
   *
   * @return an array of 2 LatLng objects.
   */
  public LatLng[] toLatLngs() {
    return new LatLng[] {getNorthEast(), getSouthWest()};
  }

  /**
   * Constructs a LatLngBounds from current bounds with an additional latitude-longitude pair.
   *
   * @param latLng the latitude lognitude pair to include in the bounds.
   * @return the newly constructed bounds
   */
  public LatLngBounds include(LatLng latLng) {
    return new LatLngBounds.Builder()
      .include(getNorthEast())
      .include(getSouthWest())
      .include(latLng)
      .build();
  }

  /**
   * Determines whether this LatLngBounds matches another one via LatLng.
   *
   * @param o another object
   * @return a boolean indicating whether the LatLngBounds are equal
   */
  @Override
  public boolean equals(final Object o) {
    if (this == o) {
      return true;
    }
    if (o instanceof LatLngBounds) {
      LatLngBounds other = (LatLngBounds) o;
      return mLatNorth == other.getLatNorth()
        && mLatSouth == other.getLatSouth()
        && mLonEast == other.getLonEast()
        && mLonWest == other.getLonWest();
    }
    return false;
  }

  /**
   * Determines whether this LatLngBounds contains a point.
   *
   * @param latLng the point which may be contained
   * @return true, if the point is contained within the bounds
   */
  public boolean contains(final ILatLng latLng) {
    final double latitude = latLng.getLatitude();
    final double longitude = latLng.getLongitude();
    return ((latitude <= this.mLatNorth)
      && (latitude >= this.mLatSouth))
      && ((longitude <= this.mLonEast)
      && (longitude >= this.mLonWest));
  }

  /**
   * Determines whether this LatLngBounds contains another bounds.
   *
   * @param other the bounds which may be contained
   * @return true, if the bounds is contained within the bounds
   */
  public boolean contains(final LatLngBounds other) {
    return contains(other.getNorthEast()) && contains(other.getSouthWest());
  }

  /**
   * Returns a new LatLngBounds that stretches to contain both this and another LatLngBounds.
   *
   * @param bounds LatLngBounds to add
   * @return LatLngBounds
   */
  public LatLngBounds union(LatLngBounds bounds) {
    return union(bounds.getLatNorth(), bounds.getLonEast(), bounds.getLatSouth(), bounds.getLonWest());
  }

  /**
   * Returns a new LatLngBounds that stretches to include another LatLngBounds,
   * given by corner points.
   *
   * @param lonNorth Northern Longitude
   * @param latEast  Eastern Latitude
   * @param lonSouth Southern Longitude
   * @param latWest  Western Longitude
   * @return BoundingBox
   */
  public LatLngBounds union(final double lonNorth, final double latEast, final double lonSouth, final double latWest) {
    return new LatLngBounds((this.mLatNorth < lonNorth) ? lonNorth : this.mLatNorth,
      (this.mLonEast < latEast) ? latEast : this.mLonEast,
      (this.mLatSouth > lonSouth) ? lonSouth : this.mLatSouth,
      (this.mLonWest > latWest) ? latWest : this.mLonWest);
  }

  /**
   * Returns a new LatLngBounds that is the intersection of this with another box
   *
   * @param box LatLngBounds to intersect with
   * @return LatLngBounds
   */
  public LatLngBounds intersect(LatLngBounds box) {
    double minLatWest = Math.max(getLonWest(), box.getLonWest());
    double maxLatEast = Math.min(getLonEast(), box.getLonEast());
    if (maxLatEast > minLatWest) {
      double minLonSouth = Math.max(getLatSouth(), box.getLatSouth());
      double maxLonNorth = Math.min(getLatNorth(), box.getLatNorth());
      if (maxLonNorth > minLonSouth) {
        return new LatLngBounds(maxLonNorth, maxLatEast, minLonSouth, minLatWest);
      }
    }
    return null;
  }

  /**
   * Returns a new LatLngBounds that is the intersection of this with another LatLngBounds
   *
   * @param northLatitude Northern Longitude
   * @param eastLongitude Eastern Latitude
   * @param southLatitude Southern Longitude
   * @param westLongitude Western Latitude
   * @return LatLngBounds
   */
  public LatLngBounds intersect(double northLatitude, double eastLongitude, double southLatitude,
                                double westLongitude) {
    return intersect(new LatLngBounds(northLatitude, eastLongitude, southLatitude, westLongitude));
  }

  public static final Parcelable.Creator<LatLngBounds> CREATOR =
    new Parcelable.Creator<LatLngBounds>() {
      @Override
      public LatLngBounds createFromParcel(final Parcel in) {
        return readFromParcel(in);
      }

      @Override
      public LatLngBounds[] newArray(final int size) {
        return new LatLngBounds[size];
      }
    };

  @Override
  public int hashCode() {
    return (int) ((mLatNorth + 90)
      + ((mLatSouth + 90) * 1000)
      + ((mLonEast + 180) * 1000000)
      + ((mLonEast + 180) * 1000000000));
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(final Parcel out, final int arg1) {
    out.writeDouble(this.mLatNorth);
    out.writeDouble(this.mLonEast);
    out.writeDouble(this.mLatSouth);
    out.writeDouble(this.mLonWest);
  }

  private static LatLngBounds readFromParcel(final Parcel in) {
    final double lonNorth = in.readDouble();
    final double latEast = in.readDouble();
    final double lonSouth = in.readDouble();
    final double latWest = in.readDouble();
    return new LatLngBounds(lonNorth, latEast, lonSouth, latWest);
  }

  /**
   * Builder for composing LatLngBounds objects.
   */
  public static final class Builder {

    private List<LatLng> mLatLngList;

    public Builder() {
      mLatLngList = new ArrayList<>();
    }

    public LatLngBounds build() {
      if (mLatLngList.size() < 2) {
        throw new InvalidLatLngBoundsException(mLatLngList.size());
      }
      return LatLngBounds.fromLatLngs(mLatLngList);
    }

    public Builder includes(List<LatLng> latLngs) {
      for (LatLng point : latLngs) {
        mLatLngList.add(point);
      }
      return this;
    }

    public Builder include(@NonNull LatLng latLng) {
      mLatLngList.add(latLng);
      return this;
    }
  }
}
