package com.mapbox.mapboxsdk.camera;

import android.graphics.Point;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

public class CameraUpdateFactory {

    /**
     * Returns a CameraUpdate that moves the camera to a specified CameraPosition.
     *
     * @param cameraPosition Camera Position to change to
     * @return CameraUpdate Final Camera Position data
     */
    public static CameraUpdate newCameraPosition(@NonNull CameraPosition cameraPosition) {
        return new PositionCameraUpdate(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
    }

    /**
     * Returns a CameraUpdate that moves the center of the screen to a latitude and longitude
     * specified by a LatLng object. This centers the camera on the LatLng object.
     *
     * @param latLng
     * @return
     */
    public static CameraUpdate newLatLng(@NonNull LatLng latLng) {
        return new PositionCameraUpdate(-1, latLng, -1, -1);
    }

    /**
     * Returns a CameraUpdate that transforms the camera such that the specified latitude/longitude
     * bounds are centered on screen at the greatest possible zoom level.
     * You can specify padding, in order to inset the bounding box from the map view's edges.
     * The returned CameraUpdate has a bearing of 0 and a tilt of 0.
     *
     * @param bounds
     * @param padding
     * @return
     */
    public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, int padding) {
        throw new UnsupportedOperationException("Not implemented yet");
    }

    /**
     * Returns a CameraUpdate that transforms the camera such that the specified latitude/longitude
     * bounds are centered on screen within a bounding box of specified dimensions at the greatest
     * possible zoom level. You can specify additional padding, to further restrict the size of
     * the bounding box. The returned CameraUpdate has a bearing of 0 and a tilt of 0.
     *
     * @param bounds
     * @param width
     * @param height
     * @param padding
     * @return
     */
    public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, int width, int height, int padding) {
        throw new UnsupportedOperationException("Not implemented yet");
    }

    /**
     * Returns a CameraUpdate that moves the center of the screen to a latitude and longitude specified by a LatLng object, and moves to the given zoom level.
     *
     * @param latLng
     * @param zoom
     * @return
     */
    public static CameraUpdate newLatLngZoom(@NonNull LatLng latLng, float zoom) {
        return new PositionCameraUpdate(-1, latLng, -1, zoom);
    }

    /**
     * Returns a CameraUpdate that scrolls the camera over the map, shifting the center of view by the specified number of pixels in the x and y directions.
     *
     * @param xPixel
     * @param yPixel
     * @return
     */
    public static CameraUpdate scrollBy(float xPixel, float yPixel) {
        throw new UnsupportedOperationException("Not implemented yet");
    }

    /**
     * Returns a CameraUpdate that shifts the zoom level of the current camera viewpoint.
     *
     * @param amount
     * @param focus
     * @return
     */
    public static CameraUpdate zoomBy(float amount, Point focus) {
        return new ZoomUpdate(amount, focus.x, focus.y);
    }

    /**
     * Returns a CameraUpdate that shifts the zoom level of the current camera viewpoint.
     *
     * @param amount
     * @return
     */
    public static CameraUpdate zoomBy(float amount) {
        return new ZoomUpdate(ZoomUpdate.ZOOM_BY, amount);
    }

    /**
     * Returns a CameraUpdate that zooms in on the map by moving the viewpoint's height closer to the Earth's surface. The zoom increment is 1.0.
     *
     * @return
     */
    public static CameraUpdate zoomIn() {
        return new ZoomUpdate(ZoomUpdate.ZOOM_IN);
    }

    /**
     * Returns a CameraUpdate that zooms out on the map by moving the viewpoint's height farther away from the Earth's surface. The zoom increment is -1.0.
     *
     * @return
     */
    public static CameraUpdate zoomOut() {
        return new ZoomUpdate(ZoomUpdate.ZOOM_OUT);
    }

    /**
     * Returns a CameraUpdate that moves the camera viewpoint to a particular zoom level.
     *
     * @param zoom
     * @return
     */
    public static CameraUpdate zoomTo(float zoom) {
        return new ZoomUpdate(ZoomUpdate.ZOOM_TO, zoom);
    }

    //
    // CameraUpdate types
    //

    public static class PositionCameraUpdate implements CameraUpdate {

        private final float bearing;
        private final LatLng target;
        private final float tilt;
        private final float zoom;

        PositionCameraUpdate(float bearing, LatLng target, float tilt, float zoom) {
            this.bearing = bearing;
            this.target = target;
            this.tilt = tilt;
            this.zoom = zoom;
        }

        public LatLng getTarget() {
            return target;
        }

        public float getBearing() {
            return bearing;
        }

        public float getTilt() {
            return tilt;
        }

        public float getZoom() {
            return zoom;
        }
    }

    public static class ZoomUpdate implements CameraUpdate {

        @IntDef({ZOOM_IN, ZOOM_OUT, ZOOM_BY, ZOOM_TO, ZOOM_TO_POINT})
        @Retention(RetentionPolicy.SOURCE)
        public @interface Type {
        }

        public static final int ZOOM_IN = 0;
        public static final int ZOOM_OUT = 1;
        public static final int ZOOM_BY = 2;
        public static final int ZOOM_TO = 3;
        public static final int ZOOM_TO_POINT = 4;

        @Type
        private final int type;
        private final float zoom;
        private float x;
        private float y;

        ZoomUpdate(@Type int type) {
            this.type = type;
            this.zoom = 0;
        }

        ZoomUpdate(@Type int type, float zoom) {
            this.type = type;
            this.zoom = zoom;
        }

        ZoomUpdate(float zoom, float x, float y) {
            this.type = ZOOM_TO_POINT;
            this.zoom = zoom;
            this.x = x;
            this.y = y;
        }

        public float getZoom() {
            return zoom;
        }

        @Type
        public int getType() {
            return type;
        }

        public float getX() {
            return x;
        }

        public float getY() {
            return y;
        }
    }
}
