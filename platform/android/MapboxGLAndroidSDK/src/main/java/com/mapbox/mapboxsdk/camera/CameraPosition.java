package com.mapbox.mapboxsdk.camera;

import android.content.res.TypedArray;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.FloatRange;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MathConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MathUtils;

/**
 * Resembles the position, angle, zoom and tilt of the user's viewpoint.
 */
public final class CameraPosition implements Parcelable {

    public static final Parcelable.Creator<CameraPosition> CREATOR
            = new Parcelable.Creator<CameraPosition>() {
        public CameraPosition createFromParcel(Parcel in) {
            double bearing = in.readDouble();
            LatLng target = in.readParcelable(LatLng.class.getClassLoader());
            double tilt = in.readDouble();
            double zoom = in.readDouble();
            return new CameraPosition(target, zoom, tilt, bearing);
        }

        public CameraPosition[] newArray(int size) {
            return new CameraPosition[size];
        }
    };

    /**
     * Direction that the camera is pointing in, in degrees clockwise from north.
     */
    public final double bearing;

    /**
     * The location that the camera is pointing at.
     */
    public final LatLng target;

    /**
     * The angle, in degrees, of the camera angle from the nadir (directly facing the Earth). See tilt(float) for details of restrictions on the range of values.
     */
    public final double tilt;

    /**
     * Zoom level near the center of the screen. See zoom(float) for the definition of the camera's zoom level.
     */
    public final double zoom;

    /**
     * Constructs a CameraPosition.
     *
     * @param target  The target location to align with the center of the screen.
     * @param zoom    Zoom level at target. See zoom(float) for details of restrictions.
     * @param tilt    The camera angle, in degrees, from the nadir (directly down). See tilt(float) for details of restrictions.
     * @param bearing Direction that the camera is pointing in, in degrees clockwise from north. This value will be normalized to be within 0 degrees inclusive and 360 degrees exclusive.
     * @throws NullPointerException     if target is null
     * @throws IllegalArgumentException if tilt is outside the range of 0 to 90 degrees inclusive.
     */
    CameraPosition(LatLng target, double zoom, double tilt, double bearing) {
        this.target = target;
        this.bearing = bearing;
        this.tilt = tilt;
        this.zoom = zoom;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeDouble(bearing);
        out.writeParcelable(target, flags);
        out.writeDouble(tilt);
        out.writeDouble(zoom);
    }

    @Override
    public String toString() {
        return "Target: " + target + ", Zoom:" + zoom + ", Bearing:" + bearing + ", Tilt:" + tilt;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        CameraPosition cameraPosition = (CameraPosition) o;
        if (target != null && !target.equals(cameraPosition.target)) {
            return false;
        } else if (zoom != cameraPosition.zoom) {
            return false;
        } else if (tilt != cameraPosition.tilt) {
            return false;
        } else if (bearing != cameraPosition.bearing) {
            return false;
        }
        return true;
    }

    @Override
    public int hashCode() {
        int result = 1;
        result = 31 * result + (target != null ? target.hashCode() : 0);
        return result;
    }

    /**
     * Builder for composing {@link CameraPosition} objects.
     */
    public static final class Builder {

        private double bearing = -1;
        private LatLng target = null;
        private double tilt = -1;
        private double zoom = -1;
        private boolean isRadian;

        /**
         * Creates an empty builder.
         */
        public Builder() {
            super();
        }

        /**
         * Creates a builder for building CameraPosition objects using radians.
         *
         * @param isRadian true if heading is in radians
         */
        public Builder(boolean isRadian) {
            this.isRadian = isRadian;
        }

        /**
         * Create Builder with an existing CameraPosition data.
         *
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
         * Create Builder with an existing CameraPosition data.
         *
         * @param typedArray TypedArray containgin attribute values
         */
        public Builder(TypedArray typedArray) {
            super();
            if (typedArray != null) {
                this.bearing = typedArray.getFloat(R.styleable.MapView_direction, 0.0f);
                double lat = typedArray.getFloat(R.styleable.MapView_center_latitude, 0.0f);
                double lng = typedArray.getFloat(R.styleable.MapView_center_longitude, 0.0f);
                this.target = new LatLng(lat, lng);
                this.tilt = typedArray.getFloat(R.styleable.MapView_tilt, 0.0f);
                this.zoom = typedArray.getFloat(R.styleable.MapView_zoom, 0.0f);
            }
        }

        /**
         * Create Builder from an existing CameraPositionUpdate update.
         *
         * @param update Update containing camera options
         */
        public Builder(CameraUpdateFactory.CameraPositionUpdate update) {
            super();
            if (update != null) {
                this.bearing = update.getBearing();
                this.target = update.getTarget();
                this.tilt = update.getTilt();
                this.zoom = update.getZoom();
            }
        }

        /**
         * Create Builder from an existing CameraPositionUpdate update.
         *
         * @param update Update containing camera options
         */
        public Builder(CameraUpdateFactory.ZoomUpdate update) {
            super();
            if (update != null) {
                this.zoom = update.getZoom();
            }
        }

        /**
         * Create Builder from an exisiting array of doubles.
         *
         * @param values Values containing target, bearing, tilt and zoom
         */
        public Builder(double[] values) {
            super();
            if (values != null && values.length == 5) {
                this.target = new LatLng(values[0], values[1]);
                this.bearing = (float) values[2];
                this.tilt = (float) values[3];
                this.zoom = (float) values[4];
            }
        }

        /**
         * Sets the direction that the camera is pointing in, in degrees clockwise from north.
         *
         * @param bearing Bearing
         * @return Builder
         */
        public Builder bearing(double bearing) {
            if (isRadian) {
                this.bearing = bearing;
            } else {
                // converting degrees to radians
                this.bearing = (float) (-bearing * MathConstants.DEG2RAD);
            }
            return this;
        }

        /**
         * Builds a CameraPosition.
         *
         * @return CameraPosition
         */
        public CameraPosition build() {
            return new CameraPosition(target, zoom, tilt, bearing);
        }

        /**
         * Sets the location that the camera is pointing at.
         *
         * @param location Location
         * @return Builder
         */
        public Builder target(LatLng location) {
            this.target = location;
            return this;
        }

        /**
         * Set the tilt
         *
         * @param tilt Tilt value
         * @return Builder
         */
        @FloatRange(from = 0.0, to = 60.0)
        public Builder tilt(double tilt) {
            if (isRadian) {
                this.tilt = tilt;
            } else {
                // converting degrees to radians
                this.tilt = (float) (MathUtils.clamp(tilt, MapboxConstants.MINIMUM_TILT, MapboxConstants.MAXIMUM_TILT) * MathConstants.DEG2RAD);
            }
            return this;
        }

        /**
         * Set the zoom
         *
         * @param zoom Zoom value
         * @return Builder
         */
        public Builder zoom(double zoom) {
            this.zoom = zoom;
            return this;
        }
    }
}
