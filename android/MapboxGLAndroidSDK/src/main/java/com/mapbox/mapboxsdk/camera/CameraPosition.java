package com.mapbox.mapboxsdk.camera;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.FloatRange;
import com.mapbox.mapboxsdk.geometry.LatLng;

public final class CameraPosition implements Parcelable {

    public static final Parcelable.Creator<CameraPosition> CREATOR
            = new Parcelable.Creator<CameraPosition>() {
        public CameraPosition createFromParcel(Parcel in) {
            float bearing = in.readFloat();
            LatLng target = in.readParcelable(LatLng.class.getClassLoader());
            float tilt = in.readFloat();
            float zoom = in.readFloat();
            return new CameraPosition(target, zoom, tilt, bearing);
        }

        public CameraPosition[] newArray(int size) {
            return new CameraPosition[size];
        }
    };


    /**
     * Direction that the camera is pointing in, in degrees clockwise from north.
     */
    public final float bearing;

    /**
     * The location that the camera is pointing at.
     */
    public final LatLng target;

    /**
     * The angle, in degrees, of the camera angle from the nadir (directly facing the Earth). See tilt(float) for details of restrictions on the range of values.
     */
    public final float tilt;

    /**
     * Zoom level near the center of the screen. See zoom(float) for the definition of the camera's zoom level.
     */
    public final float zoom;

    /**
     *
     * Constructs a CameraPosition.
     * @param target The target location to align with the center of the screen.
     * @param zoom Zoom level at target. See zoom(float) for details of restrictions.
     * @param tilt The camera angle, in degrees, from the nadir (directly down). See tilt(float) for details of restrictions.
     * @param bearing Direction that the camera is pointing in, in degrees clockwise from north. This value will be normalized to be within 0 degrees inclusive and 360 degrees exclusive.
     * @throws NullPointerException if target is null
     * @throws IllegalArgumentException if tilt is outside the range of 0 to 90 degrees inclusive.
     */
    public CameraPosition (LatLng target, float zoom, float tilt, float bearing) throws NullPointerException, IllegalArgumentException{
        super();
        if (target == null) {
            throw new NullPointerException("target is NULL");
        }
        this.target = target;

        if (tilt < 0.0f || tilt > 90.0f) {
            throw new IllegalArgumentException("tilt is outside of 0 to 90 degrees range");
        }
        this.tilt = tilt;

        this.bearing = bearing;
        this.zoom = zoom;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeFloat(bearing);
        out.writeParcelable(target, flags);
        out.writeFloat(tilt);
        out.writeFloat(zoom);
    }

    /**
     * Builds camera position.
     */
    public static final class Builder {

        private float bearing;
        private LatLng target;
        private float tilt;
        private float zoom;

        /**
         * Creates an empty builder.
         */
        public Builder() {
            super();
        }

        /**
         * Create Builder with an existing CameraPosition data.
         * @param previous Existing CameraPosition values to use
         */
        public Builder(CameraPosition previous) {
            super();
            if (previous != null) {
                this.bearing = previous.bearing;
                this.target = previous.target;
                this.tilt = previous.tilt;
                this.zoom = previous.zoom;
            }
        }

        /**
         * Sets the direction that the camera is pointing in, in degrees clockwise from north.
         * @param bearing Bearing
         * @return Builder
         */
        public Builder bearing (float bearing) {
            this.bearing = bearing;
            return this;
        }

        /**
         * Builds a CameraPosition.
         * @return CameraPosition
         */
        public CameraPosition build() {
            return new CameraPosition(target, zoom, tilt, bearing);
        }

        /**
         * Sets the location that the camera is pointing at.
         * @param location Location
         * @return Builder
         */
        public Builder target(LatLng location) {
            this.target = location;
            return this;
        }

        /**
         * Set the tilt
         * @param tilt Tilt value
         * @return Builder
         */
        @FloatRange(from = 0.0, to = 60.0)
        public Builder tilt(float tilt) {
            this.tilt = tilt;
            return this;
        }

        /**
         * Set the zoom
         * @param zoom Zoom value
         * @return Builder
         */
        public Builder zoom(float zoom) {
            this.zoom = zoom;
            return this;
        }
    }
}
