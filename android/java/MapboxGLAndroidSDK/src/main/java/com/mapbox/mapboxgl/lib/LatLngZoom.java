package com.mapbox.mapboxgl.lib;

import android.os.Parcel;
import android.os.Parcelable;
import com.mapbox.mapboxgl.lib.geometry.LatLng;

public class LatLngZoom extends LatLng implements Parcelable {

    public static final Parcelable.Creator<LatLngZoom> CREATOR = new Parcelable.Creator<LatLngZoom>() {
        public LatLngZoom createFromParcel(Parcel in) {
            return new LatLngZoom(in);
        }

        public LatLngZoom[] newArray(int size) {
            return new LatLngZoom[size];
        }
    };

    private double zoom;

    public LatLngZoom(double lat, double lon, double zoom) {
        super(lat, lon);
        this.zoom = zoom;
    }

    public LatLngZoom(LatLng latLng, double zoom) {
        super(latLng.getLatitude(), latLng.getLongitude());
        this.zoom = zoom;
    }

    private LatLngZoom(Parcel in) {
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
        LatLngZoom other = (LatLngZoom) obj;
        return super.equals(obj) && Double.doubleToLongBits(zoom) == Double.doubleToLongBits(other.zoom);
    }

    @Override
    public String toString() {
        return "LatLngZoom [lat=" + super.getLatitude() + ", lon=" + super.getLongitude() + ", zoom=" + zoom + "]";
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
