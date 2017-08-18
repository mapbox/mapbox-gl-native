package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

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

  private final double latitudeNorth;
  private final double latitudeSouth;
  private final double longitudeEast;
  private final double longitudeWest;

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
    this.latitudeNorth = northLatitude;
    this.longitudeEast = eastLongitude;
    this.latitudeSouth = southLatitude;
    this.longitudeWest = westLongitude;
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
    return new LatLng((this.latitudeNorth + this.latitudeSouth) / 2,
      (this.longitudeEast + this.longitudeWest) / 2);
  }

  /**
   * Get the north latitude value of this bounds.
   *
   * @return double latitude value for north
   */
  public double getLatNorth() {
    return this.latitudeNorth;
  }

  /**
   * Get the south latitude value of this bounds.
   *
   * @return double latitude value for south
   */
  public double getLatSouth() {
    return this.latitudeSouth;
  }

  /**
   * Get the east longitude value of this bounds.
   *
   * @return double longitude value for east
   */
  public double getLonEast() {
    return this.longitudeEast;
  }

  /**
   * Get the west longitude value of this bounds.
   *
   * @return double longitude value for west
   */
  public double getLonWest() {
    return this.longitudeWest;
  }

  /**
   * Get the latitude-longitude pair of the south west corner of this bounds.
   *
   * @return LatLng of the south west corner
   */
  public LatLng getSouthWest() {
    return new LatLng(latitudeSouth, longitudeWest);
  }

  /**
   * Get the latitude-longitude paur if the north east corner of this bounds.
   *
   * @return LatLng of the north east corner
   */
  public LatLng getNorthEast() {
    return new LatLng(latitudeNorth, longitudeEast);
  }

  /**
   * Get the latitude-longitude pair of the south east corner of this bounds.
   *
   * @return LatLng of the south east corner
   */
  public LatLng getSouthEast() {
    return new LatLng(latitudeSouth, longitudeEast);
  }

  /**
   * Get the latitude-longitude pair of the north west corner of this bounds.
   *
   * @return LatLng of the north west corner
   */
  public LatLng getNorthWest() {
    return new LatLng(latitudeNorth, longitudeWest);
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
    return Math.abs(this.latitudeNorth - this.latitudeSouth);
  }

  /**
   * Get the absolute distance, in degrees, between the west and
   * east boundaries of this LatLngBounds
   *
   * @return Span distance
   */
  public double getLongitudeSpan() {
    return Math.abs(this.longitudeEast - this.longitudeWest);
  }


  /**
   * Validate if LatLngBounds is empty, determined if absolute distance is
   *
   * @return boolean indicating if span is empty
   */
  public boolean isEmptySpan() {
    return getLongitudeSpan() == 0.0 || getLatitudeSpan() == 0.0;
  }

  /**
   * Returns a string representaton of the object.
   *
   * @return the string representation
   */
  @Override
  public String toString() {
    return "N:" + this.latitudeNorth + "; E:" + this.longitudeEast + "; S:" + this.latitudeSouth
      + "; W:" + this.longitudeWest;
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
   * Constructs a LatLngBounds from doubles representing a LatLng pair.
   * <p>
   * This method doesn't recalculate most east or most west boundaries.
   * </p>
   */
  public static LatLngBounds from(double latNorth, double lonEast, double latSouth, double lonWest) {
    return new LatLngBounds(latNorth, lonEast, latSouth, lonWest);
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
      return latitudeNorth == other.getLatNorth()
        && latitudeSouth == other.getLatSouth()
        && longitudeEast == other.getLonEast()
        && longitudeWest == other.getLonWest();
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
    return ((latitude <= this.latitudeNorth)
      && (latitude >= this.latitudeSouth))
      && ((longitude <= this.longitudeEast)
      && (longitude >= this.longitudeWest));
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
    return new LatLngBounds((this.latitudeNorth < lonNorth) ? lonNorth : this.latitudeNorth,
      (this.longitudeEast < latEast) ? latEast : this.longitudeEast,
      (this.latitudeSouth > lonSouth) ? lonSouth : this.latitudeSouth,
      (this.longitudeWest > latWest) ? latWest : this.longitudeWest);
  }

  /**
   * Returns a new LatLngBounds that is the intersection of this with another box
   *
   * @param box LatLngBounds to intersect with
   * @return LatLngBounds
   */
  @Nullable
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

  /**
   * Inner class responsible for recreating Parcels into objects.
   */
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

  /**
   * Returns a hash code value for the object.
   *
   * @return the hash code
   */
  @Override
  public int hashCode() {
    return (int) ((latitudeNorth + 90)
      + ((latitudeSouth + 90) * 1000)
      + ((longitudeEast + 180) * 1000000)
      + ((longitudeEast + 180) * 1000000000));
  }

  /**
   * Describe the kinds of special objects contained in this Parcelable instance's marshaled representation.
   *
   * @return a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
   */
  @Override
  public int describeContents() {
    return 0;
  }

  /**
   * Flatten this object in to a Parcel.
   *
   * @param out   The Parcel in which the object should be written.
   * @param flags Additional flags about how the object should be written
   */
  @Override
  public void writeToParcel(final Parcel out, final int flags) {
    out.writeDouble(this.latitudeNorth);
    out.writeDouble(this.longitudeEast);
    out.writeDouble(this.latitudeSouth);
    out.writeDouble(this.longitudeWest);
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

    private List<LatLng> latLngList;

    /**
     * Constructs a builder to compose LatLng objects to a LatLngBounds.
     */
    public Builder() {
      latLngList = new ArrayList<>();
    }

    /**
     * Builds a new LatLngBounds.
     * <p>
     * Throws an {@link InvalidLatLngBoundsException} when no LatLngBounds can be created.
     * </p>
     *
     * @return the build LatLngBounds
     */
    public LatLngBounds build() {
      if (latLngList.size() < 2) {
        throw new InvalidLatLngBoundsException(latLngList.size());
      }
      return LatLngBounds.fromLatLngs(latLngList);
    }

    /**
     * Adds a LatLng object to the LatLngBounds.Builder.
     *
     * @param latLngs the List of LatLng objects to be added
     * @return this
     */
    public Builder includes(List<LatLng> latLngs) {
      for (LatLng point : latLngs) {
        latLngList.add(point);
      }
      return this;
    }

    /**
     * Adds a LatLng object to the LatLngBounds.Builder.
     *
     * @param latLng the LatLng to be added
     * @return this
     */
    public Builder include(@NonNull LatLng latLng) {
      latLngList.add(latLng);
      return this;
    }
  }
}
