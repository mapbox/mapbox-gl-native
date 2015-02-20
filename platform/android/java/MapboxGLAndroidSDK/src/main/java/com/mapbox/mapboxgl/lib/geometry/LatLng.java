package com.mapbox.mapboxgl.lib.geometry;

import android.location.Location;
import android.os.Parcel;
import android.os.Parcelable;
import com.mapbox.mapboxgl.lib.constants.GeoConstants;
import com.mapbox.mapboxgl.lib.constants.MathConstants;
import java.io.Serializable;

public class LatLng implements ILatLng, GeoConstants, MathConstants, Parcelable, Serializable {

    public static final Parcelable.Creator<LatLng> CREATOR = new Parcelable.Creator<LatLng>() {
        public LatLng createFromParcel(Parcel in) {
            return new LatLng(in);
        }

        public LatLng[] newArray(int size) {
            return new LatLng[size];
        }
    };

    private double longitude;
    private double latitude;
    private double altitude = 0f;

    /**
     * Construct a new latitude, longitude point given float arguments
     * @param latitude Latitude in degrees
     * @param longitude Longitude in degress
     */
    public LatLng(double latitude, double longitude) {
        this.latitude = latitude;
        this.longitude = longitude;
    }

    /**
     * Construct a new latitude, longitude, altitude point given float arguments
     * @param latitude Latitude in degrees
     * @param longitude Longitude in degress
     * @param altitude Altitude in meters
     */
    public LatLng(final double latitude, final double longitude, final double altitude) {
        this.latitude = latitude;
        this.longitude = longitude;
        this.altitude = altitude;
    }

    /**
     * Transform a Location into a LatLng point
     * @param location Android Location
     */
    public LatLng(final Location location) {
        this(location.getLatitude(), location.getLongitude(), location.getAltitude());
    }

    /**
     * Clone an existing latitude longitude point
     * @param aLatLng LatLng
     */
    public LatLng(final LatLng aLatLng) {
        this.latitude = aLatLng.latitude;
        this.longitude = aLatLng.longitude;
        this.altitude = aLatLng.altitude;
    }

    protected LatLng(Parcel in) {
        longitude = in.readDouble();
        latitude = in.readDouble();
        altitude = in.readDouble();
    }

    @Override
    public double getLatitude() {
        return latitude;
    }

    @Override
    public double getLongitude() {
        return longitude;
    }

    @Override
    public double getAltitude() {
        return altitude;
    }

    @Override
    public boolean equals(final Object obj) {
        if (obj == null) {
            return false;
        }
        if (obj == this) {
            return true;
        }
        if (!obj.getClass().equals(this.getClass())) {
            return false;
        }
        final LatLng rhs = (LatLng) obj;
        return rhs.latitude == this.latitude
                && rhs.longitude == this.longitude
                && rhs.altitude == this.altitude;
    }

    @Override
    public int hashCode() {
        return (int) (37.0 * (17.0 * latitude * 1E6d + longitude * 1E6d) + altitude);
    }

    @Override
    public String toString() {
        return "LatLng [longitude=" + longitude + ", latitude=" + latitude + "]";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeDouble(longitude);
        out.writeDouble(latitude);
        out.writeDouble(altitude);
    }

    /**
     * Calculate distance between two points
     * @param other Other LatLng to compare to
     * @return distance in meters
     */
    public int distanceTo(final LatLng other) {

        final double a1 = DEG2RAD * this.latitude;
        final double a2 = DEG2RAD * this.longitude;
        final double b1 = DEG2RAD * other.getLatitude();
        final double b2 = DEG2RAD * other.getLongitude();

        final double cosa1 = Math.cos(a1);
        final double cosb1 = Math.cos(b1);

        final double t1 = cosa1 * Math.cos(a2) * cosb1 * Math.cos(b2);
        final double t2 = cosa1 * Math.sin(a2) * cosb1 * Math.sin(b2);
        final double t3 = Math.sin(a1) * Math.sin(b1);
        final double tt = Math.acos(t1 + t2 + t3);

        return (int) (RADIUS_EARTH_METERS * tt);
    }
}
