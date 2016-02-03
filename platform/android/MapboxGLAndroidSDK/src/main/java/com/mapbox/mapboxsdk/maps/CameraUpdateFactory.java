package com.mapbox.mapboxsdk.maps;

import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.RectF;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.utils.MathUtils;

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
        return new CameraPositionUpdate(cameraPosition.bearing, cameraPosition.target, cameraPosition.tilt, cameraPosition.zoom);
    }

    /**
     * Returns a CameraUpdate that moves the center of the screen to a latitude and longitude
     * specified by a LatLng object. This centers the camera on the LatLng object.
     *
     * @param latLng
     * @return
     */
    public static CameraUpdate newLatLng(@NonNull LatLng latLng) {
        return new CameraPositionUpdate(-1, latLng, -1, -1);
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
        return newLatLngBounds(bounds, padding, padding, padding, padding);
    }


    /**
     * Returns a CameraUpdate that transforms the camera such that the specified latitude/longitude
     * bounds are centered on screen at the greatest possible zoom level.
     * You can specify padding, in order to inset the bounding box from the map view's edges.
     * The returned CameraUpdate has a bearing of 0 and a tilt of 0.
     *
     * @param bounds
     * @param paddingLeft
     * @param paddingTop
     * @param paddingRight
     * @param paddingBottom
     * @return
     */
    public static CameraUpdate newLatLngBounds(@NonNull LatLngBounds bounds, int paddingLeft, int paddingTop, int paddingRight, int paddingBottom) {
        return new CameraBoundsUpdate(bounds, paddingLeft, paddingTop, paddingRight, paddingBottom);
    }

    /**
     * Returns a CameraUpdate that moves the center of the screen to a latitude and longitude specified by a LatLng object, and moves to the given zoom level.
     *
     * @param latLng
     * @param zoom
     * @return
     */
    public static CameraUpdate newLatLngZoom(@NonNull LatLng latLng, float zoom) {
        return new CameraPositionUpdate(-1, latLng, -1, zoom);
    }

    /**
     * Returns a CameraUpdate that scrolls the camera over the map, shifting the center of view by the specified number of pixels in the x and y directions.
     *
     * @param xPixel
     * @param yPixel
     * @return
     */
    public static CameraUpdate scrollBy(float xPixel, float yPixel) {
        return new CameraMoveUpdate(xPixel, yPixel);
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

    public static class CameraPositionUpdate implements CameraUpdate {

        private final float bearing;
        private final LatLng target;
        private final float tilt;
        private final float zoom;

        CameraPositionUpdate(float bearing, LatLng target, float tilt, float zoom) {
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

        @Override
        public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
            return new CameraPosition.Builder(this).build();
        }
    }

    public static class CameraBoundsUpdate implements CameraUpdate {

        private LatLngBounds bounds;
        private RectF padding;

        public CameraBoundsUpdate(LatLngBounds bounds, RectF padding) {
            this.bounds = bounds;
            this.padding = padding;
        }

        public CameraBoundsUpdate(LatLngBounds bounds, int[] padding) {
            this(bounds, new RectF(padding[0], padding[1], padding[2], padding[3]));
        }

        public CameraBoundsUpdate(LatLngBounds bounds, int paddingLeft, int paddingTop, int paddingRight, int paddingBottom) {
            this(bounds, new int[]{paddingLeft, paddingTop, paddingRight, paddingBottom});
        }

        public LatLngBounds getBounds() {
            return bounds;
        }

        public RectF getPadding() {
            return padding;
        }

        @Override
        public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
            MapView mapView = mapboxMap.getMapView();
            RectF padding = getPadding();

            // Calculate the bounds of the possibly rotated shape with respect to the viewport.
            PointF nePixel = new PointF(-10000, -10000);
            PointF swPixel = new PointF(1000, 10000);
            float viewportHeight = mapView.getHeight();
            for (LatLng latLng : getBounds().toLatLngs()) {
                PointF pixel = mapView.toScreenLocation(latLng);
                swPixel.x = Math.min(swPixel.x, pixel.x);
                nePixel.x = Math.max(nePixel.x, pixel.x);
                swPixel.y = Math.min(swPixel.y, viewportHeight - pixel.y);
                nePixel.y = Math.max(nePixel.y, viewportHeight - pixel.y);
            }

            float width = nePixel.x - swPixel.x;
            float height = nePixel.y - swPixel.y;

            // Calculate the zoom level.
            float scaleX = (mapView.getWidth() - padding.left - padding.right) / width;
            float scaleY = (mapView.getHeight() - padding.top - padding.bottom) / height;
            float minScale = scaleX < scaleY ? scaleX : scaleY;
            double zoom = Math.log(mapView.getScale() * minScale) / Math.log(2);
            zoom = MathUtils.clamp(zoom, (float) mapView.getMinZoom(), (float) mapView.getMaxZoom());

            // Calculate the center point of a virtual bounds that is extended in all directions by padding.
            PointF paddedNEPixel = new PointF(nePixel.x + padding.right / minScale, nePixel.y + padding.top / minScale);
            PointF paddedSWPixel = new PointF(swPixel.x - padding.left / minScale, swPixel.y - padding.bottom / minScale);
            PointF centerPixel = new PointF((paddedNEPixel.x + paddedSWPixel.x) / 2, (paddedNEPixel.y + paddedSWPixel.y) / 2);

            centerPixel.y = viewportHeight - centerPixel.y;

            LatLng center = mapboxMap.getProjection().fromScreenLocation(centerPixel);

            return new CameraPosition.Builder()
                    .target(center)
                    .zoom((float) zoom)
                    .tilt(0)
                    .bearing(0)
                    .build();
        }
    }

    public static class CameraMoveUpdate implements CameraUpdate {

        private float x;
        private float y;

        public CameraMoveUpdate(float x, float y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
            MapView mapView = mapboxMap.getMapView();

            // Calculate the new center point
            float viewPortWidth = mapView.getWidth();
            float viewPortHeight = mapView.getHeight();
            PointF targetPoint = new PointF(viewPortWidth / 2 + x, viewPortHeight / 2 + y);

            // Convert point to LatLng
            LatLng latLng = mapView.fromScreenLocation(targetPoint);

            CameraPosition cameraPosition = mapboxMap.getCameraPosition();
            return new CameraPosition.Builder()
                    .target(latLng)
                    .zoom(cameraPosition.zoom)
                    .tilt(cameraPosition.tilt)
                    .bearing(cameraPosition.bearing)
                    .build();
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

        public float transformZoom(float currentZoom) {
            switch (getType()) {
                case CameraUpdateFactory.ZoomUpdate.ZOOM_IN:
                    currentZoom++;
                    break;
                case CameraUpdateFactory.ZoomUpdate.ZOOM_OUT:
                    currentZoom--;
                    if (currentZoom < 0) {
                        currentZoom = 0;
                    }
                    break;
                case CameraUpdateFactory.ZoomUpdate.ZOOM_TO:
                    currentZoom = getZoom();
                    break;
                case CameraUpdateFactory.ZoomUpdate.ZOOM_BY:
                    currentZoom = currentZoom + getZoom();
                    break;
                case CameraUpdateFactory.ZoomUpdate.ZOOM_TO_POINT:
                    currentZoom = currentZoom + getZoom();
                    break;
            }
            return currentZoom;
        }

        @Override
        public CameraPosition getCameraPosition(@NonNull MapboxMap mapboxMap) {
            CameraPosition cameraPosition = mapboxMap.getCameraPosition();
            if (getType() != CameraUpdateFactory.ZoomUpdate.ZOOM_TO_POINT) {
                return new CameraPosition.Builder(cameraPosition)
                        .zoom(transformZoom(cameraPosition.zoom))
                        .build();
            } else {
                return new CameraPosition.Builder(cameraPosition)
                        .zoom(transformZoom(cameraPosition.zoom))
                        .target(mapboxMap.getProjection().fromScreenLocation(new PointF(getX(), getY())))
                        .build();
            }
        }
    }
}
