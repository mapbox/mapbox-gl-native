package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;

public class ProjectedMeters implements IProjectedMeters, Parcelable, Serializable {

    public static final Creator<ProjectedMeters> CREATOR = new Creator<ProjectedMeters>() {
        public ProjectedMeters createFromParcel(Parcel in) {
            return new ProjectedMeters(in);
        }

        public ProjectedMeters[] newArray(int size) {
            return new ProjectedMeters[size];
        }
    };

    private double northing;
    private double easting;

    public ProjectedMeters(double northing, double easting) {
        this.northing = northing;
        this.easting = easting;
    }

    public ProjectedMeters(ProjectedMeters aProjectedMeters) {
        this.northing = aProjectedMeters.northing;
        this.easting = aProjectedMeters.easting;
    }

    protected ProjectedMeters(Parcel in) {
        northing = in.readDouble();
        easting = in.readDouble();
    }

    @Override
    public double getNorthing() {
        return northing;
    }

    @Override
    public double getEasting() {
        return easting;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        ProjectedMeters projectedMeters = (ProjectedMeters) o;

        return Double.compare(projectedMeters.easting, easting) == 0 && Double.compare(projectedMeters.northing, northing) == 0;

    }

    @Override
    public int hashCode() {
        int result;
        long temp;
        temp = Double.doubleToLongBits(easting);
        result = (int) (temp ^ (temp >>> 32));
        temp = Double.doubleToLongBits(northing);
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public String toString() {
        return "ProjectedMeters [northing=" + northing + ", easting=" + easting + "]";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeDouble(northing);
        out.writeDouble(easting);
    }
}
