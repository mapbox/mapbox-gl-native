package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

/**
 * A rectangular geograpical region defined by a south west {@link LatLng} and a north east {@link LatLng}.
 */
public class CoordinateBounds implements Parcelable {

    private LatLng mSouthWest;
    private LatLng mNorthEast;

    public CoordinateBounds(@NonNull Parcel in) {
        mSouthWest = in.readParcelable(LatLng.class.getClassLoader());
        mNorthEast = in.readParcelable(LatLng.class.getClassLoader());
    }

    public CoordinateBounds(@NonNull LatLng southWest, @NonNull LatLng northEast) {
        mSouthWest = southWest;
        mNorthEast = northEast;
    }

    @NonNull
    public LatLng getSouthWest() {
        return mSouthWest;
    }

    public void setSouthWest(@NonNull LatLng mSouthWest) {
        this.mSouthWest = mSouthWest;
    }

    @NonNull
    public LatLng getNorthEast() {
        return mNorthEast;
    }

    public void setNorthEast(@NonNull LatLng mNorthEast) {
        this.mNorthEast = mNorthEast;
    }

    @Override
    public int hashCode() {
        int result;
        long temp;
        temp = mSouthWest.hashCode();
        result = (int) (temp ^ (temp >>> 32));
        temp = mNorthEast.hashCode();
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o instanceof CoordinateBounds) {
            CoordinateBounds other = (CoordinateBounds) o;
            return getNorthEast().equals(other.getNorthEast())
                    && getSouthWest().equals(other.getSouthWest());
        }
        return false;
    }

    @Override
    public String toString() {
        return "CoordinateBounds [mNorthEast[" + getNorthEast() + "], mSouthWest[]" + getSouthWest() + "]";
    }

    public static final Parcelable.Creator<CoordinateBounds> CREATOR =
            new Parcelable.Creator<CoordinateBounds>() {
                @Override
                public CoordinateBounds createFromParcel(Parcel in) {
                    return new CoordinateBounds(in);
                }

                @Override
                public CoordinateBounds[] newArray(int size) {
                    return new CoordinateBounds[size];
                }
            };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int arg1) {
        out.writeParcelable(mSouthWest, arg1);
        out.writeParcelable(mNorthEast, arg1);
    }
}
