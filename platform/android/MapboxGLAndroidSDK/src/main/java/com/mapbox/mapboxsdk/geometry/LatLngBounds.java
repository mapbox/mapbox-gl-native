package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;

import java.util.ArrayList;
import java.util.List;

/**
 * A geographical area representing a latitude/longitude aligned rectangle.
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
   * Calculates the centerpoint of this LatLngBounds by simple interpolation and returns
   * it as a point. This is a non-geodesic calculation which is not the geographic center.
   *
   * @return LatLng center of this LatLngBounds
   */
  public LatLng getCenter() {
    return new LatLng((this.mLatNorth + this.mLatSouth) / 2,
      (this.mLonEast + this.mLonWest) / 2);
  }

  public double getLatNorth() {
    return this.mLatNorth;
  }

  public double getLatSouth() {
    return this.mLatSouth;
  }

  public double getLonEast() {
    return this.mLonEast;
  }

  public double getLonWest() {
    return this.mLonWest;
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

  public LatLng[] toLatLngs() {
    return new LatLng[] {new LatLng(mLatNorth, mLonEast), new LatLng(mLatSouth, mLonWest)};
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
   * Determines whether this LatLngBounds contains a point and the point
   * does not touch its boundary.
   *
   * @param latLng the point which may be contained
   * @return true, if the point is contained within the box.
   */
  public boolean contains(final ILatLng latLng) {
    final double latitude = latLng.getLatitude();
    final double longitude = latLng.getLongitude();
    return ((latitude < this.mLatNorth)
      && (latitude > this.mLatSouth))
      && ((longitude < this.mLonEast)
      && (longitude > this.mLonWest));
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
