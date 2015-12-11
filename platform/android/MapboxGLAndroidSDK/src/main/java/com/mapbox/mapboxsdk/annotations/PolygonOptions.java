package com.mapbox.mapboxsdk.annotations;


import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

public final class PolygonOptions implements Parcelable {

    public static final Parcelable.Creator<PolygonOptions> CREATOR
            = new Parcelable.Creator<PolygonOptions>() {
        public PolygonOptions createFromParcel(Parcel in) {
            return new PolygonOptions(in);
        }

        public PolygonOptions[] newArray(int size) {
            return new PolygonOptions[size];
        }
    };

    private PolygonOptions(Parcel in) {
        polygon = new Polygon();
        ArrayList<LatLng> pointsList = new ArrayList<>();
        in.readList(pointsList, LatLng.class.getClassLoader());
        addAll(pointsList);
        alpha(in.readFloat());
        fillColor(in.readInt());
        strokeColor(in.readInt());
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeList(getPoints());
        out.writeFloat(getAlpha());
        out.writeInt(getFillColor());
        out.writeInt(getStrokeColor());
    }

    private Polygon polygon;

    public PolygonOptions() {
        polygon = new Polygon();
    }

    public PolygonOptions add(LatLng point) {
        polygon.addPoint(point);
        return this;
    }

    public PolygonOptions add(LatLng... points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolygonOptions addAll(Iterable<LatLng> points) {
        for (LatLng point : points) {
            add(point);
        }
        return this;
    }

    public PolygonOptions alpha(float alpha) {
        polygon.setAlpha(alpha);
        return this;
    }

    public float getAlpha() {
        return polygon.getAlpha();
    }

    /**
     * Sets the color of the polygon.
     *
     * @param color - the color in ARGB format
     * @return PolygonOptions - the options object
     */
    public PolygonOptions fillColor(int color) {
        polygon.setFillColor(color);
        return this;
    }

    public int getFillColor() {
        return polygon.getFillColor();
    }

    /**
     * Do not use this method. Used internally by the SDK.
     */
    public Polygon getPolygon() {
        return polygon;
    }

    /**
     * Sets the color of the stroke of the polygon.
     *
     * @param color - the color in ARGB format
     * @return PolygonOptions - the options object
     */
    public PolygonOptions strokeColor(int color) {
        polygon.setStrokeColor(color);
        return this;
    }

    public int getStrokeColor() {
        return polygon.getStrokeColor();
    }

    public List<LatLng> getPoints() {
        // the getter gives us a copy, which is the safe thing to do...
        return polygon.getPoints();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        PolygonOptions polygon = (PolygonOptions) o;

        if (Float.compare(polygon.getAlpha(), getAlpha()) != 0) return false;
        if (getFillColor() != polygon.getFillColor()) return false;
        if (getStrokeColor() != polygon.getStrokeColor()) return false;
        return !(getPoints() != null ? !getPoints().equals(polygon.getPoints()) : polygon.getPoints() != null);
    }

    @Override
    public int hashCode() {
        int result = 1;
        result = 31 * result + (getAlpha() != +0.0f ? Float.floatToIntBits(getAlpha()) : 0);
        result = 31 * result + getFillColor();
        result = 31 * result + getStrokeColor();
        result = 31 * result + (getPoints() != null ? getPoints().hashCode() : 0);
        return result;
    }
}
