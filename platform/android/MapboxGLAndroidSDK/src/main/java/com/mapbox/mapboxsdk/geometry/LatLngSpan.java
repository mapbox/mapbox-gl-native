package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

/**
 * A geographical span defined by its latitude and longitude span.
 */
public class LatLngSpan implements Parcelable {

  private double mLatitudeSpan;
  private double mLongitudeSpan;

  private LatLngSpan(@NonNull Parcel in) {
    mLatitudeSpan = in.readDouble();
    mLongitudeSpan = in.readDouble();
  }

  /**
   * Creates a LatLgnSpan.
   *
   * @param latitudeSpan  The span used for latitude.
   * @param longitudeSpan The span used for longitude.
   */
  public LatLngSpan(double latitudeSpan, double longitudeSpan) {
    mLatitudeSpan = latitudeSpan;
    mLongitudeSpan = longitudeSpan;
  }

  /**
   * Returns the latitude span.
   *
   * @return The latitude span.
   */
  public double getLatitudeSpan() {
    return mLatitudeSpan;
  }

  /**
   * Sets the latitude span.
   *
   * @param latitudeSpan The latitude span to set.
   */
  public void setLatitudeSpan(double latitudeSpan) {
    mLatitudeSpan = latitudeSpan;
  }

  /**
   * Returns to longitude span.
   *
   * @return The longitude span.
   */
  public double getLongitudeSpan() {
    return mLongitudeSpan;
  }

  /**
   * Sets the longitude span.
   *
   * @param longitudeSpan The longitude span to set.
   */
  public void setLongitudeSpan(double longitudeSpan) {
    mLongitudeSpan = longitudeSpan;
  }

  /**
   * Indicates whether some other object is "equal to" this one.
   *
   * @param object The object to compare
   * @return True if equal, false if not
   */
  @Override
  public boolean equals(Object object) {
    if (this == object) {
      return true;
    }
    if (object instanceof LatLngSpan) {
      LatLngSpan other = (LatLngSpan) object;
      return mLongitudeSpan == other.getLongitudeSpan()
        && mLatitudeSpan == other.getLatitudeSpan();
    }
    return false;
  }

  /**
   * Inner class responsible for recreating Parcels into objects.
   */
  public static final Parcelable.Creator<LatLngSpan> CREATOR =
    new Parcelable.Creator<LatLngSpan>() {
      @Override
      public LatLngSpan createFromParcel(Parcel in) {
        return new LatLngSpan(in);
      }

      @Override
      public LatLngSpan[] newArray(int size) {
        return new LatLngSpan[size];
      }
    };

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
   * @param out   Parcel in which the object should be written
   * @param flags Additional flags about how the object should be written
   */
  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeDouble(mLatitudeSpan);
    out.writeDouble(mLongitudeSpan);
  }

  /**
   * Returns a hash code value for the object.
   *
   * @return hash code value of this
   */
  @Override
  public int hashCode() {
    int result;
    long temp;
    temp = Double.doubleToLongBits(mLatitudeSpan);
    result = (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(mLongitudeSpan);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    return result;
  }
}
