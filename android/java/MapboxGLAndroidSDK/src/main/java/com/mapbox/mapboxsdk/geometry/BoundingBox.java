package com.mapbox.mapboxsdk.geometry;

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
     * @param northLatitude Northern Latitude
     * @param eastLongitude   Eastern Longitude
     * @param southLatitude Southern Latitude
     * @param westLongitude   Western Longitude
     */
    public BoundingBox(final double northLatitude, final double eastLongitude, final double southLatitude, final double westLongitude) {
        this.mLatNorth = northLatitude;
        this.mLonEast = eastLongitude;
        this.mLatSouth = southLatitude;
        this.mLonWest = westLongitude;
        this.mIsValid = ((this.mLonWest < this.mLonEast) && (this.mLatNorth > this.mLatSouth));
    }

    /**
     * Construct a new bounding box based on its corners, given in NESW order.
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
     * Get the area spanned by this bounding box
     *
     * @return CoordinateSpan area
     */
    public CoordinateSpan getSpan() {
        return new CoordinateSpan(getLatitudeSpan(), getLongitudeSpan());
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


    /**
     * Validate if bounding box is empty, determined if absolute distance is
     *
     * @return boolean indicating if span is empty
     */
    public boolean isEmpty() {
        return getLongitudeSpan() == 0.0 || getLatitudeSpan() == 0.0;
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
     * @param o another object
     * @return a boolean indicating whether the bounding boxes are equal
     */
    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o instanceof BoundingBox) {
            BoundingBox other = (BoundingBox) o;
            return mLatNorth == other.getLatNorth()
                    && mLatSouth == other.getLatSouth()
                    && mLonEast == other.getLonEast()
                    && mLonWest == other.getLonWest();
        }
        return false;
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
        return ((latitude < this.mLatNorth)
                && (latitude > this.mLatSouth))
                && ((longitude < this.mLonEast)
                && (longitude > this.mLonWest));
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
     * @param lonNorth Northern Longitude
     * @param latEast  Eastern Latitude
     * @param lonSouth Southern Longitude
     * @param latWest  Western Longitude
     * @return BoundingBox
     */
    public BoundingBox union(final double lonNorth, final double latEast, final double lonSouth, final double latWest) {
        return new BoundingBox((this.mLatNorth < lonNorth) ? lonNorth : this.mLatNorth,
                (this.mLonEast < latEast) ? latEast : this.mLonEast,
                (this.mLatSouth > lonSouth) ? lonSouth : this.mLatSouth,
                (this.mLonWest > latWest) ? latWest : this.mLonWest);
    }

    /**
     * Returns a new BoundingBox that is the intersection of this with another box
     *
     * @param box BoundingBox to intersect with
     * @return BoundingBox
     */
    public BoundingBox intersect(BoundingBox box) {
        double minLatWest = Math.max(getLonWest(), box.getLonWest());
        double maxLatEast = Math.min(getLonEast(), box.getLonEast());
        if (maxLatEast > minLatWest) {
            double minLonSouth = Math.max(getLatSouth(), box.getLatSouth());
            double maxLonNorth = Math.min(getLatNorth(), box.getLatNorth());
            if (maxLonNorth > minLonSouth) {
                return new BoundingBox(maxLonNorth, maxLatEast, minLonSouth, minLatWest);
            }
        }
        return null;
    }

    /**
     * Returns a new BoundingBox that is the intersection of this with another box
     *
     * @param northLongitude Northern Longitude
     * @param eastLatitude   Eastern Latitude
     * @param southLongitude Southern Longitude
     * @param westLatitude   Western Latitude
     * @return BoundingBox
     */
    public BoundingBox intersect(double northLongitude, double eastLatitude, double southLongitude, double westLatitude) {
        return intersect(new BoundingBox(northLongitude, eastLatitude, southLongitude, westLatitude));
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
        final double lonNorth = in.readDouble();
        final double latEast = in.readDouble();
        final double lonSouth = in.readDouble();
        final double latWest = in.readDouble();
        return new BoundingBox(lonNorth, latEast, lonSouth, latWest);
    }
}
