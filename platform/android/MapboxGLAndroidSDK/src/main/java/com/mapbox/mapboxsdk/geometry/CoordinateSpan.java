package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

/**
 * A geographical span defined by its latitude and longitude span.
 */
public class CoordinateSpan implements Parcelable {

    private double mLatitudeSpan;
    private double mLongitudeSpan;

    public CoordinateSpan(@NonNull Parcel in){
        mLatitudeSpan = in.readDouble();
        mLongitudeSpan = in.readDouble();
    }

    public CoordinateSpan(double latitudeSpan, double longitudeSpan) {
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
        if (o instanceof CoordinateSpan) {
            CoordinateSpan other = (CoordinateSpan) o;
            return mLongitudeSpan == other.getLongitudeSpan()
                    && mLatitudeSpan == other.getLatitudeSpan();
        }
        return false;
    }

    public static final Parcelable.Creator<CoordinateSpan> CREATOR =
            new Parcelable.Creator<CoordinateSpan>() {
                @Override
                public CoordinateSpan createFromParcel(Parcel in) {
                    return new CoordinateSpan(in);
                }

                @Override
                public CoordinateSpan[] newArray(int size) {
                    return new CoordinateSpan[size];
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