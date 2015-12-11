package com.mapbox.mapboxsdk.annotations;

import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public final class PolylineOptions implements Parcelable {


    public static final Parcelable.Creator<PolylineOptions> CREATOR
            = new Parcelable.Creator<PolylineOptions>() {
        public PolylineOptions createFromParcel(Parcel in) {
            return new PolylineOptions(in);
        }

        public PolylineOptions[] newArray(int size) {
            return new PolylineOptions[size];
        }
    };

    private PolylineOptions(Parcel in) {
        polyline = new Polyline();
        ArrayList<LatLng> pointsList = new ArrayList<>();
        in.readList(pointsList, LatLng.class.getClassLoader());
        addAll(pointsList);
        alpha(in.readFloat());
        color(in.readInt());
        width(in.readFloat());
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeList(getPoints());
        out.writeFloat(getAlpha());
        out.writeInt(getColor());
        out.writeFloat(getWidth());
    }

    private Polyline polyline;

    public PolylineOptions() {
        polyline = new Polyline();
    }

    public PolylineOptions add(LatLng point) {
        polyline.addPoint(point);
        return this;
    }

    public PolylineOptions add(LatLng... points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolylineOptions addAll(Iterable<LatLng> points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolylineOptions alpha(float alpha) {
        polyline.setAlpha(alpha);
        return this;
    }

    public float getAlpha() {
        return polyline.getAlpha();
    }

    /**
     * Sets the color of the polyline.
     *
     * @param color - the color in ARGB format
     */
    public PolylineOptions color(int color) {
        polyline.setColor(color);
        return this;
    }

    public int getColor() {
        return polyline.getColor();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public Polyline getPolyline() {
        return polyline;
    }

    public float getWidth() {
        return polyline.getWidth();
    }

    /**
     * Sets the width of the polyline.
     *
     * @param width in pixels
     * @return a new PolylineOptions
     */
    public PolylineOptions width(float width) {
        polyline.setWidth(width);
        return this;
    }

    public List<LatLng> getPoints() {
        // the getter gives us a copy, which is the safe thing to do...
        return polyline.getPoints();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        PolylineOptions polyline = (PolylineOptions) o;

        if (Float.compare(polyline.getAlpha(), getAlpha()) != 0) return false;
        if (getColor() != polyline.getColor()) return false;
        if (Float.compare(polyline.getWidth(), getWidth()) != 0) return false;
        return !(getPoints() != null ? !getPoints().equals(polyline.getPoints()) : polyline.getPoints() != null);
    }

    @Override
    public int hashCode() {
        int result = 1;
        result = 31 * result + (getAlpha() != +0.0f ? Float.floatToIntBits(getAlpha()) : 0);
        result = 31 * result + getColor();
        result = 31 * result + (getWidth() != +0.0f ? Float.floatToIntBits(getWidth()) : 0);
        result = 31 * result + (getPoints() != null ? getPoints().hashCode() : 0);
        return result;
    }
}
