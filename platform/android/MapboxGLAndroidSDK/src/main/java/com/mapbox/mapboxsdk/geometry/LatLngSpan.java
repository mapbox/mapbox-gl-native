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

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o instanceof LatLngSpan) {
      LatLngSpan other = (LatLngSpan) o;
      return mLongitudeSpan == other.getLongitudeSpan()
        && mLatitudeSpan == other.getLatitudeSpan();
    }
    return false;
  }

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

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel out, int arg1) {
    out.writeDouble(mLatitudeSpan);
    out.writeDouble(mLongitudeSpan);
  }
}