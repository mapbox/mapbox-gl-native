package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Parcel;
import android.os.Parcelable;

public class PointD implements Parcelable {
    public double x;
    public double y;


    public PointD(double x, double y) {
        this.x = x;
        this.y = y;
    }

    public PointD(PointD src) {
        this.x = src.x;
        this.y = src.y;
    }

    private PointD(Parcel in) {
        x = in.readDouble();
        y = in.readDouble();
    }

    /**
     * Set the point's x and y coordinates
     */
    public void set(double x, double y) {
        this.x = x;
        this.y = y;
    }

    /**
     * Negate the point's coordinates
     */
    public final void negate() {
        x = -x;
        y = -y;
    }

    /**
     * Offset the point's coordinates by dx, dy
     */
    public final void offset(double dx, double dy) {
        x += dx;
        y += dy;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof PointD)) return false;

        PointD pointD = (PointD) o;

        if (Double.compare(pointD.x, x) != 0) return false;
        if (Double.compare(pointD.y, y) != 0) return false;

        return true;
    }

    @Override
    public int hashCode() {
        int result;
        long temp;
        temp = Double.doubleToLongBits(x);
        result = (int) (temp ^ (temp >>> 32));
        temp = Double.doubleToLongBits(y);
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public String toString() {
        return "PointD{" +
                "x=" + x +
                ", y=" + y +
                '}';
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeDouble(x);
        dest.writeDouble(y);
    }

    public static final Creator<PointD> CREATOR = new Creator<PointD>() {
        @Override
        public PointD createFromParcel(Parcel source) {
            return new PointD(source);
        }

        @Override
        public PointD[] newArray(int size) {
            return new PointD[size];
        }
    };
}
