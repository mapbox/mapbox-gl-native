package com.mapbox.mapboxsdk.camera;

import android.support.annotation.NonNull;

public class CameraUpdateFactory {

    /**
     * Returns a CameraUpdate that moves the camera to a specified CameraPosition.
     * @param cameraPosition Camera Position to change to
     * @return CameraUpdate Final Camera Position data
     */
    public static CameraUpdate newCameraPosition (@NonNull CameraPosition cameraPosition) {
        return new CameraUpdate(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
    }
}
