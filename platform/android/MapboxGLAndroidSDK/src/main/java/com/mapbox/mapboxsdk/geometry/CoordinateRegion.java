package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

/**
 * A geographical region defined by a {@link LatLng} and a {@link CoordinateSpan}.
 */
public class CoordinateRegion implements Parcelable{

    private LatLng mCenter;
    private CoordinateSpan mSpan;

    public CoordinateRegion(@NonNull Parcel in){
        mCenter = in.readParcelable(LatLng.class.getClassLoader());
        mSpan = in.readParcelable(CoordinateSpan.class.getClassLoader());
    }

    public CoordinateRegion(@NonNull LatLng center, @NonNull CoordinateSpan span) {
        mCenter = center;
        mSpan = span;
    }

    public LatLng getCenter() {
        return mCenter;
    }

    public void setCenter(@NonNull LatLng center) {
        mCenter = center;
    }

    public CoordinateSpan getSpan() {
        return mSpan;
    }

    public void setSpan(@NonNull CoordinateSpan span) {
        this.mSpan = span;
    }

    public static final Parcelable.Creator<CoordinateRegion> CREATOR =
            new Parcelable.Creator<CoordinateRegion>() {
                @Override
                public CoordinateRegion createFromParcel(Parcel in) {
                    return new CoordinateRegion(in);
                }

                @Override
                public CoordinateRegion[] newArray(int size) {
                    return new CoordinateRegion[size];
                }
            };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int arg1) {
        out.writeParcelable(mCenter, arg1);
        out.writeParcelable(mSpan, arg1);
    }
}
