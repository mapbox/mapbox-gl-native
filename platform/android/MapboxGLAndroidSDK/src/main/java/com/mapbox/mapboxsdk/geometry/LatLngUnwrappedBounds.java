package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.FloatRange;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.constants.GeometryConstants;

public class LatLngUnwrappedBounds implements Parcelable {

  @Keep
  private final double latitudeNorth;
  @Keep
  private final double latitudeSouth;
  @Keep
  private final double longitudeEast;
  @Keep
  private final double longitudeWest;

  /**
   * Constructs a LatLngUnwrappedBounds from doubles representing a LatLng pair.
   * <p>
   * This values of latNorth and latSouth should be in the range of [-90, 90],
   * see {@link GeometryConstants#MIN_LATITUDE} and {@link GeometryConstants#MAX_LATITUDE},
   * otherwise IllegalArgumentException will be thrown.
   * latNorth should be greater or equal latSouth, otherwise  IllegalArgumentException will be thrown.
   * lonEast should be greater or equal lonWest, otherwise  IllegalArgumentException will be thrown.
   * <p>
   * This method doesn't recalculate most east or most west boundaries.
   * Note that lonEast and lonWest will NOT be wrapped to be in the range of [-180, 180].
   */
  public static LatLngUnwrappedBounds from(
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double latNorth,
    double lonEast,
    @FloatRange(from = GeometryConstants.MIN_LATITUDE, to = GeometryConstants.MAX_LATITUDE) double latSouth,
    double lonWest) {

    checkParams(latNorth, lonEast, latSouth, lonWest);

    return new LatLngUnwrappedBounds(latNorth, lonEast, latSouth, lonWest);
  }

  public static LatLngUnwrappedBounds from(LatLngBounds latLngBounds) {

    if (latLngBounds == null) {
      return  null;
    }

    if (latLngBounds.getLonEast() < latLngBounds.getLonWest()) {
      return new LatLngUnwrappedBounds(latLngBounds.getLatNorth(),
              latLngBounds.getLonEast(),
              latLngBounds.getLatSouth(),
              latLngBounds.getLonWest() - GeometryConstants.LONGITUDE_SPAN);
    }

    return new LatLngUnwrappedBounds(latLngBounds.getLatNorth(),
            latLngBounds.getLonEast(),
            latLngBounds.getLatSouth(),
            latLngBounds.getLonWest());
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

    if (latNorth > GeometryConstants.MAX_LATITUDE || latNorth < GeometryConstants.MIN_LATITUDE
            || latSouth > GeometryConstants.MAX_LATITUDE || latSouth < GeometryConstants.MIN_LATITUDE) {
      throw new IllegalArgumentException("latitude must be between -90 and 90");
    }

    if (latNorth < latSouth) {
      throw new IllegalArgumentException("latNorth cannot be less than latSouth");
    }

    if (lonEast < lonWest) {
      throw new IllegalArgumentException("lonEast cannot be less than lonWest");
    }
  }

  /**
   * Construct a new LatLngUnwrappedBounds based on its corners, given in NESW
   * order.
   *
   * @param northLatitude Northern Latitude
   * @param eastLongitude Eastern Longitude
   * @param southLatitude Southern Latitude
   * @param westLongitude Western Longitude
   */
  @Keep
  LatLngUnwrappedBounds(final double northLatitude, final double eastLongitude,
                        final double southLatitude, final double westLongitude) {
    this.latitudeNorth = northLatitude;
    this.longitudeEast = eastLongitude;
    this.latitudeSouth = southLatitude;
    this.longitudeWest = westLongitude;
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
  @NonNull
  public LatLng getSouthWest() {
    return new LatLng(latitudeSouth, longitudeWest);
  }

  /**
   * Get the latitude-longitude paur if the north east corner of this bounds.
   *
   * @return LatLng of the north east corner
   */
  @NonNull
  public LatLng getNorthEast() {
    return new LatLng(latitudeNorth, longitudeEast);
  }

  /**
   * Get the latitude-longitude pair of the south east corner of this bounds.
   *
   * @return LatLng of the south east corner
   */
  @NonNull
  public LatLng getSouthEast() {
    return new LatLng(latitudeSouth, longitudeEast);
  }

  /**
   * Get the latitude-longitude pair of the north west corner of this bounds.
   *
   * @return LatLng of the north west corner
   */
  @NonNull
  public LatLng getNorthWest() {
    return new LatLng(latitudeNorth, longitudeWest);
  }

  /**
   * Inner class responsible for recreating Parcels into objects.
   */
  public static final Parcelable.Creator<LatLngUnwrappedBounds> CREATOR =
    new Parcelable.Creator<LatLngUnwrappedBounds>() {
      @Override
      public LatLngUnwrappedBounds createFromParcel(@NonNull final Parcel in) {
        return readFromParcel(in);
      }

      @Override
      public LatLngUnwrappedBounds[] newArray(final int size) {
        return new LatLngUnwrappedBounds[size];
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
  public void writeToParcel(@NonNull final Parcel out, final int flags) {
    out.writeDouble(this.latitudeNorth);
    out.writeDouble(this.longitudeEast);
    out.writeDouble(this.latitudeSouth);
    out.writeDouble(this.longitudeWest);
  }

  private static LatLngUnwrappedBounds readFromParcel(final Parcel in) {
    final double northLat = in.readDouble();
    final double eastLon = in.readDouble();
    final double southLat = in.readDouble();
    final double westLon = in.readDouble();
    return new LatLngUnwrappedBounds(northLat, eastLon, southLat, westLon);
  }

}
