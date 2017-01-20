package com.mapbox.mapboxsdk.maps.widgets.scaleview;

import android.graphics.PointF;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.Projection;


class MapScaleModel {

    private final int[] meters = {
            150,350,700, 1500, 2500, 5000, 10000, 20000, 40000, 100000, 150000, 350000, 700000, 1500000, 2500000};

    private final LatLng src = new LatLng(31, 121);

    private int maxWidth;

    private double zoom = -1;
    private Scale scale;

    MapScaleModel() {
    }

    void setMaxWidth(int width) {
        maxWidth = width;
    }

    Scale setProjection(Projection projection, CameraPosition cameraPosition) {
        if (zoom == cameraPosition.zoom) return scale;

        int distance = 0;
        int distanceIndex = meters.length;
        double screenDistance = maxWidth + 1;

        while (screenDistance > maxWidth && distanceIndex > 0) {
            distance = meters[--distanceIndex];

            LatLng dest = DistanceUtils.translatePoint(src, distance, 120);

            PointF pointSrc = projection.toScreenLocation(src);
            PointF pointDest = projection.toScreenLocation(dest);

            screenDistance = Math.sqrt(Math.pow(pointSrc.x - pointDest.x, 2) + Math.pow(pointSrc.y - pointDest.y, 2));
        }

        zoom = cameraPosition.zoom;
        scale = new Scale(text(distance), (float) screenDistance);

        return scale;
    }

    private String text(int distance) {
        if (distance < 1000) return distance + " m";
        else return distance / 1000 + " km";
    }
}
