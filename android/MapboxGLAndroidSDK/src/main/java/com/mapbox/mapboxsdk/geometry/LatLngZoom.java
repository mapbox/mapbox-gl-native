package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;

public class LatLngZoom extends LatLng implements Parcelable, Serializable {

    public static final Parcelable.Creator<LatLngZoom> CREATOR = new Parcelable.Creator<LatLngZoom>() {
        public LatLngZoom createFromParcel(Parcel in) {
            return new LatLngZoom(in);
        }

        public LatLngZoom[] newArray(int size) {
            return new LatLngZoom[size];
        }
    };

    private double zoom;

    public LatLngZoom() {
        super();
        this.zoom = 0.0;
    }

    public LatLngZoom(double latitude, double longitude, double zoom) {
        super(latitude, longitude);
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
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        if (!super.equals(o)) {
            return false;
        }

        LatLngZoom that = (LatLngZoom) o;

        return Double.compare(that.zoom, zoom) == 0;

    }

    @Override
    public int hashCode() {
        int result = super.hashCode();
        long temp;
        temp = Double.doubleToLongBits(zoom);
        result = 31 * result + (int) (temp ^ (temp >>> 32));
        return result;
    }

    @Override
    public String toString() {
        return "LatLngZoom [latitude=" + super.getLatitude() + ", longitude=" + super.getLongitude() + ", altitude=" + super.getAltitude() + ", zoom=" + zoom + "]";
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
