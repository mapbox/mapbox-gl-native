package com.mapbox.mapboxgl.geometry;

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;
import java.util.List;

/**
 * A rectangular geographical area defined in latitude and longitude units.
 */
public final class BoundingBox implements Parcelable, Serializable {

    private final double mLatNorth;
    private final double mLatSouth;
    private final double mLonEast;
    private final double mLonWest;

    private final boolean mIsValid;

    /**
     * Construct a new bounding box based on its corners, given in NESW
     * order.
     *
     * @param north Northern Coordinate
     * @param east Eastern Coordinate
     * @param south Southern Coordinate
     * @param west Western Coordinate
     */
    public BoundingBox(final double north, final double east, final double south, final double west) {
        this.mLatNorth = north;
        this.mLonEast = east;
        this.mLatSouth = south;
        this.mLonWest = west;
        this.mIsValid = ((this.mLonWest < this.mLonEast) && (this.mLatNorth > this.mLatSouth));
    }

    /**
     * Construct a new bounding box based on its corners, given in NESW
     * order.
     *
     * @param northEast Coordinate
     * @param southWest Coordinate
     */
    public BoundingBox(final LatLng northEast, final LatLng southWest) {
        this(northEast.getLatitude(), northEast.getLongitude(), southWest.getLatitude(), southWest.getLongitude());
    }

    /**
     * Create a bounding box from another bounding box
     *
     * @param other the other bounding box
     */
    public BoundingBox(final BoundingBox other) {
        this.mLatNorth = other.getLatNorth();
        this.mLonEast = other.getLonEast();
        this.mLatSouth = other.getLatSouth();
        this.mLonWest = other.getLonWest();
        this.mIsValid = other.isValid();
    }

    /**
     * Create a new BoundingBox with no size centered at 0, 0, also known as null island
     */
    public BoundingBox() {
        this(0, 0, 0, 0);
    }

    /**
     * Calculates the centerpoint of this bounding box by simple interpolation and returns
     * it as a point. This is a non-geodesic calculation which is not the geographic center.
     *
     * @return LatLng center of this BoundingBox
     */
    public LatLng getCenter() {
        return new LatLng((this.mLatNorth + this.mLatSouth) / 2,
                (this.mLonEast + this.mLonWest) / 2);
    }

    public double getLatNorth() {
        return this.mLatNorth;
    }

    public double getLatSouth() {
        return this.mLatSouth;
    }

    public double getLonEast() {
        return this.mLonEast;
    }

    public double getLonWest() {
        return this.mLonWest;
    }

    public boolean isValid() {
        return this.mIsValid;
    }

    /**
     * Get the absolute distance, in degrees, between the north and
     * south boundaries of this bounding box
     *
     * @return Span distance
     */
    public double getLatitudeSpan() {
        return Math.abs(this.mLatNorth - this.mLatSouth);
    }

    /**
     * Get the absolute distance, in degrees, between the west and
     * east boundaries of this bounding box
     *
     * @return Span distance
     */
    public double getLongitudeSpan() {
        return Math.abs(this.mLonEast - this.mLonWest);
    }

    @Override
    public String toString() {
        return "N:" + this.mLatNorth + "; E:" + this.mLonEast + "; S:" + this.mLatSouth + "; W:" + this.mLonWest;
    }

    /**
     * Constructs a bounding box that contains all of a list of LatLng
     * objects. Empty lists will yield invalid bounding boxes.
     *
     * @param latLngs List of LatLng objects
     * @return BoundingBox
     */
    public static BoundingBox fromLatLngs(final List<? extends ILatLng> latLngs) {
        double minLat = 90,
                minLon = 180,
                maxLat = -90,
                maxLon = -180;

        for (final ILatLng gp : latLngs) {
            final double latitude = gp.getLatitude();
            final double longitude = gp.getLongitude();

            minLat = Math.min(minLat, latitude);
            minLon = Math.min(minLon, longitude);
            maxLat = Math.max(maxLat, latitude);
            maxLon = Math.max(maxLon, longitude);
        }

        return new BoundingBox(maxLat, maxLon, minLat, minLon);
    }

    /**
     * Determines whether this bounding box matches another one via coordinates.
     *
     * @param other another bounding box
     * @return a boolean indicating whether the bounding boxes are equal
     */
    public boolean equals(final BoundingBox other) {

        return other != null && (other == this || mLatNorth == other.getLatNorth() && mLatSouth == other.getLatSouth() && mLonEast == other.getLonEast() && mLonWest == other.getLonWest());

    }

    /**
     * Determines whether this bounding box contains a point and the point
     * does not touch its boundary.
     *
     * @param pGeoPoint the point which may be contained
     * @return true, if the point is contained within the box.
     */
    public boolean contains(final ILatLng pGeoPoint) {
        final double latitude = pGeoPoint.getLatitude();
        final double longitude = pGeoPoint.getLongitude();
        return ((latitude < this.mLatNorth) && (latitude > this.mLatSouth)) && ((longitude
                < this.mLonEast) && (longitude > this.mLonWest));
    }

    /**
     * Returns a new BoundingBox that stretches to contain both this and another BoundingBox.
     *
     * @param box BoundingBox to add
     * @return BoundingBox
     */
    public BoundingBox union(BoundingBox box) {
        return union(box.getLatNorth(), box.getLonEast(), box.getLatSouth(), box.getLonWest());
    }

    /**
     * Returns a new BoundingBox that stretches to include another bounding box,
     * given by corner points.
     *
     * @param pLatNorth Northern Coordinate
     * @param pLonEast Eastern Coordinate
     * @param pLatSouth Southern Coordinate
     * @param pLonWest Western Coordinate
     * @return BoundingBox
     */
    public BoundingBox union(final double pLatNorth, final double pLonEast, final double pLatSouth,
                             final double pLonWest) {
        return new BoundingBox((this.mLatNorth < pLatNorth) ? pLatNorth : this.mLatNorth,
                (this.mLonEast < pLonEast) ? pLonEast : this.mLonEast,
                (this.mLatSouth > pLatSouth) ? pLatSouth : this.mLatSouth,
                (this.mLonWest > pLonWest) ? pLonWest : this.mLonWest);
    }

    /**
     * Returns a new BoundingBox that is the intersection of this with another box
     *
     * @param box BoundingBox to intersect with
     * @return BoundingBox
     */
    public BoundingBox intersect(BoundingBox box) {
        double maxLonWest = Math.max(this.mLonWest, box.getLonWest());
        double minLonEast = Math.min(this.mLonEast, box.getLonEast());
        double maxLatNorth = Math.min(this.mLatNorth, box.getLatNorth());
        double minLatSouth = Math.max(this.mLatSouth, box.getLatSouth());
        return new BoundingBox(maxLatNorth, minLonEast, minLatSouth, maxLonWest);
    }

    /**
     * Returns a new BoundingBox that is the intersection of this with another box
     *
     * @param north Northern Coordinate
     * @param east Eastern Coordinate
     * @param south Southern Coordinate
     * @param west Western Coordinate
     * @return BoundingBox
     */
    public BoundingBox intersect(double north, double east, double south, double west) {
        return intersect(new BoundingBox(north, east, south, west));
    }

    public static final Parcelable.Creator<BoundingBox> CREATOR =
            new Parcelable.Creator<BoundingBox>() {
                @Override
                public BoundingBox createFromParcel(final Parcel in) {
                    return readFromParcel(in);
                }

                @Override
                public BoundingBox[] newArray(final int size) {
                    return new BoundingBox[size];
                }
            };

    @Override
    public int hashCode() {
        return (int) ((mLatNorth + 90)
                + ((mLatSouth + 90) * 1000)
                + ((mLonEast + 180) * 1000000)
                + ((mLonEast + 180) * 1000000000));
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(final Parcel out, final int arg1) {
        out.writeDouble(this.mLatNorth);
        out.writeDouble(this.mLonEast);
        out.writeDouble(this.mLatSouth);
        out.writeDouble(this.mLonWest);
    }

    private static BoundingBox readFromParcel(final Parcel in) {
        final double latNorth = in.readDouble();
        final double lonEast = in.readDouble();
        final double latSouth = in.readDouble();
        final double lonWest = in.readDouble();
        return new BoundingBox(latNorth, lonEast, latSouth, lonWest);
    }
}
