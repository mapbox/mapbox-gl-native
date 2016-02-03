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

    public LatLngSpan(@NonNull Parcel in){
        mLatitudeSpan = in.readDouble();
        mLongitudeSpan = in.readDouble();
    }

    public LatLngSpan(double latitudeSpan, double longitudeSpan) {
        mLatitudeSpan = latitudeSpan;
        mLongitudeSpan = longitudeSpan;
    }

    public double getLatitudeSpan() {
        return mLatitudeSpan;
    }

    public void setLatitudeSpan(double latitudeSpan) {
        mLatitudeSpan = latitudeSpan;
    }

    public double getLongitudeSpan() {
        return mLongitudeSpan;
    }

    public void setLongitudeSpan(double longitudeSpan) {
        mLongitudeSpan = longitudeSpan;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
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