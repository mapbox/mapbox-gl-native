package com.mapbox.mapboxgl.lib;

import android.os.Parcel;
import android.os.Parcelable;

public class LonLatZoom extends LonLat implements Parcelable {

    public static final Parcelable.Creator<LonLatZoom> CREATOR = new Parcelable.Creator<LonLatZoom>() {
        public LonLatZoom createFromParcel(Parcel in) {
            return new LonLatZoom(in);
        }

        public LonLatZoom[] newArray(int size) {
            return new LonLatZoom[size];
        }
    };

    private double zoom;

    public LonLatZoom(double lon, double lat, double zoom) {
        super(lon, lat);
        this.zoom = zoom;
    }

    public LonLatZoom(LonLat lonLat, double zoom) {
        super(lonLat.getLon(), lonLat.getLat());
        this.zoom = zoom;
    }

    private LonLatZoom(Parcel in) {
        super(in);
        zoom = in.readDouble();
    }

    public double getZoom() {
        return zoom;
    }

    public void setZoom(double zoom) {
        this.zoom = zoom;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        long temp;
        temp = super.hashCode();
        result = prime * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        LonLatZoom other = (LonLatZoom) obj;
        return super.equals(obj) && Double.doubleToLongBits(zoom) == Double.doubleToLongBits(other.zoom);
    }

    @Override
    public String toString() {
        return "LonLatZoom [lon=" + super.getLon() + ", lat=" + super.getLat() + ", zoom=" + zoom
                + "]";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        super.writeToParcel(out, flags);
        out.writeDouble(zoom);
    }

}
