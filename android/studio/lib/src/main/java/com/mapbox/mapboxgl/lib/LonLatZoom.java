package com.mapbox.mapboxgl.lib;

import android.os.Parcel;
import android.os.Parcelable;

public class LonLatZoom implements Parcelable {

    public static final Parcelable.Creator<LonLatZoom> CREATOR = new Parcelable.Creator<LonLatZoom>() {
        public LonLatZoom createFromParcel(Parcel in) {
            return new LonLatZoom(in);
        }

        public LonLatZoom[] newArray(int size) {
            return new LonLatZoom[size];
        }
    };

    private double lon;
    private double lat;
    private double zoom;

    public LonLatZoom(double lon, double lat, double zoom) {
        this.lon = lon;
        this.lat = lat;
        this.zoom = zoom;
    }

    public LonLatZoom(LonLat lonLat, double zoom) {
        this.lon = lonLat.getLon();
        this.lat = lonLat.getLat();
        this.zoom = zoom;
    }

    private LonLatZoom(Parcel in) {
        lon = in.readDouble();
        lat = in.readDouble();
        zoom = in.readDouble();
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

    public double getZoom() {
        return zoom;
    }

    public void setZoom(double zoom) {
        this.zoom = zoom;
    }

    public LonLat getLonLat() {
        return new LonLat(lon, lat);
    }

    public void setLonLat(LonLat lonLat) {
        this.lon = lonLat.getLon();
        this.lat = lonLat.getLat();
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
        temp = Double.doubleToLongBits(zoom);
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
        LonLatZoom other = (LonLatZoom) obj;
        if (Double.doubleToLongBits(lat) != Double.doubleToLongBits(other.lat))
            return false;
        if (Double.doubleToLongBits(lon) != Double.doubleToLongBits(other.lon))
            return false;
        if (Double.doubleToLongBits(zoom) != Double
                .doubleToLongBits(other.zoom))
            return false;
        return true;
    }

    @Override
    public String toString() {
        return "LonLatZoom [lon=" + lon + ", lat=" + lat + ", zoom=" + zoom
                + "]";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeDouble(lon);
        out.writeDouble(lat);
        out.writeDouble(zoom);
    }

}
