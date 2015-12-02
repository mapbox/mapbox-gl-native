package com.mapbox.mapboxsdk.camera;

import com.mapbox.mapboxsdk.geometry.LatLng;

public final class CameraUpdate {

    /**
     * Direction that the camera is pointing in, in degrees clockwise from north.
     */
    private final float bearing;

    /**
     * The location that the camera is pointing at.
     */
    private final LatLng target;

    /**
     * The angle, in degrees, of the camera angle from the nadir (directly facing the Earth). See tilt(float) for details of restrictions on the range of values.
     */
    private final float tilt;

    /**
     * Zoom level near the center of the screen. See zoom(float) for the definition of the camera's zoom level.
     */
    private final float zoom;

    /**
     * Package Private Constructor to only be used CameraUpdateFactory
     * @param bearing Final Bearing
     * @param target Final Target
     * @param tilt Final Tilt
     * @param zoom Final Zoom
     */
    CameraUpdate(float bearing, LatLng target, float tilt, float zoom) {
        this.bearing = bearing;
        this.target = target;
        this.tilt = tilt;
        this.zoom = zoom;
    }

    public float getBearing() {
        return bearing;
    }

    public LatLng getTarget() {
        return target;
    }

    public float getTilt() {
        return tilt;
    }

    public float getZoom() {
        return zoom;
    }
}
