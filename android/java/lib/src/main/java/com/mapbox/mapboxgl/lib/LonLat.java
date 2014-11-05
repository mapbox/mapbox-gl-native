package com.mapbox.mapboxgl.lib;

import android.os.Parcel;
import android.os.Parcelable;

public class LonLat implements Parcelable {

    public static final Parcelable.Creator<LonLat> CREATOR = new Parcelable.Creator<LonLat>() {
        public LonLat createFromParcel(Parcel in) {
            return new LonLat(in);
        }

        public LonLat[] newArray(int size) {
            return new LonLat[size];
        }
    };

    private double lon;
    private double lat;

    public LonLat(double lon, double lat) {
        this.lon = lon;
        this.lat = lat;
    }

    private LonLat(Parcel in) {
        lon = in.readDouble();
        lat = in.readDouble();
    }

    public double getLon() {
        return lon;
    }

    public void setLon(double lon) {
        this.lon = lon;
    }

    public double getLat() {
        return lat;
    }

    public void setLat(double lat) {
        this.lat = lat;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        long temp;
        temp = Double.doubleToLongBits(lat);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        temp = Double.doubleToLongBits(lon);
        result = prime * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        LonLat other = (LonLat) obj;
        if (Double.doubleToLongBits(lat) != Double.doubleToLongBits(other.lat))
            return false;
        if (Double.doubleToLongBits(lon) != Double.doubleToLongBits(other.lon))
            return false;
        return true;
    }

    @Override
    public String toString() {
        return "LonLat [lon=" + lon + ", lat=" + lat + "]";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeDouble(lon);
        out.writeDouble(lat);
    }

}
