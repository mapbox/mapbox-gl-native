package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.FloatRange;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.constants.GeometryConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;

import java.util.ArrayList;
import java.util.List;

/**
 * A geographical area representing a latitude/longitude aligned rectangle.
 * <p>
 * This class does not wrap values to the world bounds.
 * </p>
 */
public class LatLngBounds implements Parcelable {

  @Keep
  private final double latitudeNorth;
  @Keep
  private final double latitudeSouth;
  @Keep
  private final double longitudeEast;
  @Keep
  private final double longitudeWest;

  /**
   * Construct a new LatLngBounds based on its corners, given in NESW
   * order.
   * <p>
   * If eastern longitude is smaller than the western one, bounds will include antimeridian.
   * For example, if the NE point is (10, -170) and the SW point is (-10, 170), then bounds will span over 20 degrees
   * and cross the antimeridian.
   *
   * @param northLatitude Northern Latitude
   * @param eastLongitude Eastern Longitude
   * @param southLatitude Southern Latitude
   * @param westLongitude Western Longitude
   */
  @Keep
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
    return LatLngBounds.from(
      GeometryConstants.MAX_LATITUDE, GeometryConstants.MAX_LONGITUDE,
      GeometryConstants.MIN_LATITUDE, GeometryConstants.MIN_LONGITUDE);
  }

  /**
   * Calculates the centerpoint of this LatLngBounds by simple interpolation and returns
   * it as a point. This is a non-geodesic calculation which is not the geographic center.
   *
   * @return LatLng center of this LatLngBounds
   */
  public LatLng getCenter() {
    double latCenter = (this.latitudeNorth + this.latitudeSouth) / 2.0;
    double longCenter;

    if (this.longitudeEast >= this.longitudeWest) {
      longCenter = (this.longitudeEast + this.longitudeWest) / 2;
    } else {
      double halfSpan = (GeometryConstants.LONGITUDE_SPAN + this.longitudeEast - this.longitudeWest) / 2.0;
      longCenter = this.longitudeWest + halfSpan;
      if (longCenter >= GeometryConstants.MAX_LONGITUDE) {
        longCenter = this.longitudeEast - halfSpan;
      }
    }

    return new LatLng(latCenter, longCenter);
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
    double longSpan = Math.abs(this.longitudeEast - this.longitudeWest);
    if (this.longitudeEast >= this.longitudeWest) {
      return longSpan;
    }

    // shortest span contains antimeridian
    return GeometryConstants.LONGITUDE_SPAN - longSpan;
  }

  static double getLongitudeSpan(final double longEast, final double longWest) {
    double longSpan = Math.abs(longEast - longWest);
    if (longEast >= longWest) {
      return longSpan;
    }

    // shortest span contains antimeridian
    return GeometryConstants.LONGITUDE_SPAN - longSpan;
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
  static LatLngBounds fromLatLngs(final List<? extends LatLng> latLngs) {
    double minLat = GeometryConstants.MAX_LATITUDE;
    double maxLat = GeometryConstants.MIN_LATITUDE;

    double eastLon = latLngs.get(0).getLongitude();
    double westLon = latLngs.get(1).getLongitude();
    double lonSpan = Math.abs(eastLon - westLon);
    if (lonSpan < GeometryConstants.LONGITUDE_SPAN / 2) {
      if (eastLon < westLon) {
        double temp = eastLon;
        eastLon = westLon;
        westLon = temp;
      }
    } else {
      lonSpan = GeometryConstants.LONGITUDE_SPAN - lonSpan;
      if (westLon < eastLon) {
        double temp = eastLon;
        eastLon = westLon;
        westLon = temp;
      }
    }

    for (final LatLng gp : latLngs) {
      final double latitude = gp.getLatitude();
      minLat = Math.min(minLat, latitude);
      maxLat = Math.max(maxLat, latitude);

      final double longitude = gp.getLongitude();
      if (!containsLongitude(eastLon, westLon, longitude)) {
        final double eastSpan = getLongitudeSpan(longitude, westLon);
        final double westSpan = getLongitudeSpan(eastLon, longitude);
        if (eastSpan <= westSpan) {
          eastLon = longitude;
        } else {
          westLon = longitude;
        }
      }
    }

    return new LatLngBounds(maxLat, eastLon, minLat, westLon);
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
   * This values of latNorth and latSouth should be in the range of [-90, 90],
   * see {@link GeometryConstants#MIN_LATITUDE} and {@link GeometryConstants#MAX_LATITUDE},
   * otherwise IllegalArgumentException will be thrown.
   * latNorth should be greater or equal latSouth, otherwise  IllegalArgumentException will be thrown.
   * <p>
   * This method doesn't recalculate most east or most west boundaries.
   * Note that lonEast and lonWest will be wrapped to be in the range of [-180, 180],
   * see {@link GeometryConstants#MIN_LONGITUDE} and {@link GeometryConstants#MAX_LONGITUDE}
   * </p>
   */
  public static LatLngBounds from(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double latNorth,
    double lonEast,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double latSouth,
    double lonWest) {

    checkParams(latNorth, lonEast, latSouth, lonWest);

    lonEast = LatLng.wrap(lonEast, GeometryConstants.MIN_LONGITUDE, GeometryConstants.MAX_LONGITUDE);
    lonWest = LatLng.wrap(lonWest, GeometryConstants.MIN_LONGITUDE, GeometryConstants.MAX_LONGITUDE);

    return new LatLngBounds(latNorth, lonEast, latSouth, lonWest);
  }

  private static void checkParams(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double latNorth,
    double lonEast,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double latSouth,
    double lonWest) {

    if (Double.isNaN(latNorth) || Double.isNaN(latSouth)) {
      throw new IllegalArgumentException("latitude must not be NaN");
    }

    if (Double.isNaN(lonEast) || Double.isNaN(lonWest)) {
      throw new IllegalArgumentException("longitude must not be NaN");
    }

    if (Double.isInfinite(lonEast) || Double.isInfinite(lonWest)) {
      throw new IllegalArgumentException("longitude must not be infinite");
    }

    if (latNorth > GeometryConstants.MAX_LATITUDE || latNorth < GeometryConstants.MIN_LATITUDE
      || latSouth > GeometryConstants.MAX_LATITUDE || latSouth < GeometryConstants.MIN_LATITUDE) {
      throw new IllegalArgumentException("latitude must be between -90 and 90");
    }

    if (latNorth < latSouth) {
      throw new IllegalArgumentException("latNorth cannot be less than latSouth");
    }
  }

  private static double lat_(int z, int y) {
    double n = Math.PI - 2.0 * Math.PI * y / Math.pow(2.0, z);
    return Math.toDegrees(Math.atan(0.5 * (Math.exp(n) - Math.exp(-n))));
  }

  private static double lon_(int z, int x) {
    return x / Math.pow(2.0, z) * 360.0 - GeometryConstants.MAX_LONGITUDE;
  }

  /**
   * Constructs a LatLngBounds from a Tile identifier.
   * <p>
   * Returned bounds will have latitude in the range of Mercator projection.
   *
   * @param z Tile zoom level.
   * @param x Tile X coordinate.
   * @param y Tile Y coordinate.
   * @see GeometryConstants#MIN_MERCATOR_LATITUDE
   * @see GeometryConstants#MAX_MERCATOR_LATITUDE
   */
  public static LatLngBounds from(int z, int x, int y) {
    return new LatLngBounds(lat_(z, y), lon_(z, x + 1), lat_(z, y + 1), lon_(z, x));
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


  private boolean containsLatitude(final double latitude) {
    return (latitude <= this.latitudeNorth)
      && (latitude >= this.latitudeSouth);
  }

  private boolean containsLongitude(final double longitude) {
    return containsLongitude(this.longitudeEast, this.longitudeWest, longitude);
  }

  static boolean containsLongitude(final double eastLon, final double westLon, final double longitude) {
    if (eastLon >= westLon) {
      return (longitude <= eastLon)
        && (longitude >= westLon);
    }
    return (longitude <= eastLon) || (longitude >= westLon);
  }

  /**
   * Determines whether this LatLngBounds contains a point.
   *
   * @param latLng the point which may be contained
   * @return true, if the point is contained within the bounds
   */
  public boolean contains(final LatLng latLng) {
    return containsLatitude(latLng.getLatitude())
      && containsLongitude(latLng.getLongitude());
  }

  /**
   * Determines whether this LatLngBounds contains another bounds.
   *
   * @param other the bounds which may be contained
   * @return true, if the bounds is contained within the bounds
   */
  public boolean contains(final LatLngBounds other) {
    return contains(other.getNorthEast())
      && contains(other.getSouthWest());
  }

  /**
   * Returns a new LatLngBounds that stretches to contain both this and another LatLngBounds.
   *
   * @param bounds LatLngBounds to add
   * @return LatLngBounds
   */
  @NonNull
  public LatLngBounds union(@NonNull LatLngBounds bounds) {
    return unionNoParamCheck(bounds.getLatNorth(), bounds.getLonEast(), bounds.getLatSouth(), bounds.getLonWest());
  }

  /**
   * Returns a new LatLngBounds that stretches to include another LatLngBounds,
   * given by corner points.
   * <p>
   * This values of northLat and southLat should be in the range of [-90, 90],
   * see {@link GeometryConstants#MIN_LATITUDE} and {@link GeometryConstants#MAX_LATITUDE},
   * otherwise IllegalArgumentException will be thrown.
   * northLat should be greater or equal southLat, otherwise  IllegalArgumentException will be thrown.
   * <p>
   * This method doesn't recalculate most east or most west boundaries.
   * Note that eastLon and westLon will be wrapped to be in the range of [-180, 180],
   * see {@link GeometryConstants#MIN_LONGITUDE} and {@link GeometryConstants#MAX_LONGITUDE}
   *
   * @param northLat Northern Latitude corner point
   * @param eastLon  Eastern Longitude corner point
   * @param southLat Southern Latitude corner point
   * @param westLon  Western Longitude corner point
   * @return LatLngBounds
   */
  @NonNull
  public LatLngBounds union(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double northLat,
    double eastLon,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double southLat,
    double westLon) {
    checkParams(northLat, eastLon, southLat, westLon);
    return unionNoParamCheck(northLat, eastLon, southLat, westLon);
  }

  @NonNull
  private LatLngBounds unionNoParamCheck(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double northLat,
    double eastLon,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double southLat,
    double westLon) {
    northLat = (this.latitudeNorth < northLat) ? northLat : this.latitudeNorth;
    southLat = (this.latitudeSouth > southLat) ? southLat : this.latitudeSouth;
    eastLon = LatLng.wrap(eastLon, GeometryConstants.MIN_LONGITUDE, GeometryConstants.MAX_LONGITUDE);
    westLon = LatLng.wrap(westLon, GeometryConstants.MIN_LONGITUDE, GeometryConstants.MAX_LONGITUDE);

    // longitudes match
    if (this.longitudeEast == eastLon && this.longitudeWest == westLon) {
      return new LatLngBounds(northLat, eastLon, southLat, westLon);
    }

    boolean eastInThis = containsLongitude(this.longitudeEast, this.longitudeWest, eastLon);
    boolean westInThis = containsLongitude(this.longitudeEast, this.longitudeWest, westLon);
    boolean thisEastInside = containsLongitude(eastLon, westLon, this.longitudeEast);
    boolean thisWestInside = containsLongitude(eastLon, westLon, this.longitudeWest);

    // two intersections on each end - covers entire longitude
    if (eastInThis && westInThis && thisEastInside && thisWestInside) {
      return new LatLngBounds(northLat, GeometryConstants.MAX_LONGITUDE, southLat, GeometryConstants.MIN_LONGITUDE);
    }

    if (eastInThis) {
      if (westInThis) {
        return new LatLngBounds(northLat, this.longitudeEast, southLat, this.longitudeWest);
      }
      return new LatLngBounds(northLat, this.longitudeEast, southLat, westLon);
    }

    if (thisEastInside) {
      if (thisWestInside) {
        return new LatLngBounds(northLat, eastLon, southLat, westLon);
      }
      return new LatLngBounds(northLat, eastLon, southLat, this.longitudeWest);
    }

    // bounds do not intersect, find where they will form shortest union
    if (LatLngSpan.getLongitudeSpan(eastLon, this.longitudeWest)
      < LatLngSpan.getLongitudeSpan(this.longitudeEast, westLon)) {
      return new LatLngBounds(northLat,
        eastLon,
        southLat,
        this.longitudeWest);
    }

    return new LatLngBounds(northLat,
      this.longitudeEast,
      southLat,
      westLon);
  }

  /**
   * Returns a new LatLngBounds that is the intersection of this with another box
   *
   * @param box LatLngBounds to intersect with
   * @return LatLngBounds
   */
  @Nullable
  public LatLngBounds intersect(@NonNull LatLngBounds box) {
    return intersectNoParamCheck(box.getLatNorth(), box.getLonEast(), box.getLatSouth(), box.getLonWest());
  }

  /**
   * Returns a new LatLngBounds that is the intersection of this with another LatLngBounds
   * <p>
   * This values of northLat and southLat should be in the range of [-90, 90],
   * see {@link GeometryConstants#MIN_LATITUDE} and {@link GeometryConstants#MAX_LATITUDE},
   * otherwise IllegalArgumentException will be thrown.
   * northLat should be greater or equal southLat, otherwise  IllegalArgumentException will be thrown.
   * <p>
   * This method doesn't recalculate most east or most west boundaries.
   * Note that eastLon and westLon will be wrapped to be in the range of [-180, 180],
   * see {@link GeometryConstants#MIN_LONGITUDE} and {@link GeometryConstants#MAX_LONGITUDE}
   *
   * @param northLat Northern Latitude corner point
   * @param eastLon  Eastern Longitude corner point
   * @param southLat Southern Latitude corner point
   * @param westLon  Western Longitude corner point
   * @return LatLngBounds
   */
  @Nullable
  public LatLngBounds intersect(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double northLat,
    double eastLon,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double southLat,
    double westLon) {

    checkParams(northLat, eastLon, southLat, westLon);

    return intersectNoParamCheck(northLat, eastLon, southLat, westLon);
  }

  @Nullable
  private LatLngBounds intersectNoParamCheck(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double northLat,
    double eastLon,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double southLat,
    double westLon) {

    double maxsouthLat = Math.max(getLatSouth(), Math.min(GeometryConstants.MAX_LATITUDE, southLat));
    double minnorthLat = Math.min(getLatNorth(), Math.max(GeometryConstants.MIN_LATITUDE, northLat));
    if (minnorthLat < maxsouthLat) {
      return null;
    }

    eastLon = LatLng.wrap(eastLon, GeometryConstants.MIN_LONGITUDE, GeometryConstants.MAX_LONGITUDE);
    westLon = LatLng.wrap(westLon, GeometryConstants.MIN_LONGITUDE, GeometryConstants.MAX_LONGITUDE);

    // longitudes match
    if (this.longitudeEast == eastLon && this.longitudeWest == westLon) {
      return new LatLngBounds(minnorthLat, eastLon, maxsouthLat, westLon);
    }

    boolean eastInThis = containsLongitude(this.longitudeEast, this.longitudeWest, eastLon);
    boolean westInThis = containsLongitude(this.longitudeEast, this.longitudeWest, westLon);
    boolean thisEastInside = containsLongitude(eastLon, westLon, this.longitudeEast);
    boolean thisWestInside = containsLongitude(eastLon, westLon, this.longitudeWest);

    // two intersections : find the one that has longest span
    if (eastInThis && westInThis && thisEastInside && thisWestInside) {

      if (getLongitudeSpan(eastLon, this.longitudeWest) > getLongitudeSpan(this.longitudeEast, westLon)) {
        return new LatLngBounds(minnorthLat, eastLon, maxsouthLat, this.longitudeWest);
      }

      return new LatLngBounds(minnorthLat, this.longitudeEast, maxsouthLat, westLon);
    }

    if (eastInThis) {
      if (westInThis) {
        return new LatLngBounds(minnorthLat, eastLon, maxsouthLat, westLon);
      }
      return new LatLngBounds(minnorthLat, eastLon, maxsouthLat, this.longitudeWest);
    }

    if (thisEastInside) {
      if (thisWestInside) {
        return new LatLngBounds(minnorthLat, this.longitudeEast, maxsouthLat, this.longitudeWest);
      }
      return new LatLngBounds(minnorthLat, this.longitudeEast, maxsouthLat, westLon);
    }

    return null;
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
      + ((longitudeWest + 180) * 1000000000));
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
    final double northLat = in.readDouble();
    final double eastLon = in.readDouble();
    final double southLat = in.readDouble();
    final double westLon = in.readDouble();
    return new LatLngBounds(northLat, eastLon, southLat, westLon);
  }

  /**
   * Builder for composing LatLngBounds objects.
   */
  public static final class Builder {

    private final List<LatLng> latLngList = new ArrayList<>();

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
      latLngList.addAll(latLngs);
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
