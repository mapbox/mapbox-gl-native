package com.mapbox.mapboxsdk.camera;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class CameraPosition {

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

}
