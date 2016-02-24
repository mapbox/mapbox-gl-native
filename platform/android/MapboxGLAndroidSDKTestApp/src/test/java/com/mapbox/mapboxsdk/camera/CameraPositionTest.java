package com.mapbox.mapboxsdk.camera;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MathConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MathUtils;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class CameraPositionTest {

    @Test
    public void testSanity() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertNotNull("cameraPosition should not be null", cameraPosition);
    }

    @Test
    public void testToString() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("toString should match", "Target: LatLng [longitude=2.0, latitude=1.0, altitude=0.0], Zoom:3.0, Bearing:5.0, Tilt:4.0", cameraPosition.toString());
    }

    @Test
    public void testHashcode() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("toString should match", -1007681505, cameraPosition.hashCode());
    }

    @Test
    public void testRadiantBuilder() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition.Builder builder = new CameraPosition.Builder(true);
        builder.target(latLng);
        builder.zoom(3);
        builder.tilt(4);
        builder.bearing(5);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("CameraPosition should match", cameraPosition, builder.build());
    }

    @Test
    public void testDegreesRadiantBuilder() {
        LatLng latLng = new LatLng(1, 2);
        float tilt = 4;
        float bearing = 5;
        float bearingRadiant = (float) (-bearing * MathConstants.DEG2RAD);
        float tiltRadiant = (float) (MathUtils.clamp(tilt, MapboxConstants.MINIMUM_TILT, MapboxConstants.MAXIMUM_TILT) * MathConstants.DEG2RAD);

        CameraPosition.Builder degreeBuilder = new CameraPosition.Builder(false);
        degreeBuilder.target(latLng);
        degreeBuilder.zoom(3);
        degreeBuilder.tilt(tilt);
        degreeBuilder.bearing(bearing);

        CameraPosition.Builder radiantBuilder = new CameraPosition.Builder(true);
        radiantBuilder.target(latLng);
        radiantBuilder.zoom(3);
        radiantBuilder.tilt(tiltRadiant);
        radiantBuilder.bearing(bearingRadiant);
        assertEquals("CameraPosition should match", radiantBuilder.build(), degreeBuilder.build());
    }

    @Test
    public void testZoomUpdateBuilder() {
        float zoomLevel = 5;
        CameraPosition.Builder builder = new CameraPosition.Builder(
                (CameraUpdateFactory.ZoomUpdate) CameraUpdateFactory.zoomTo(zoomLevel));
        assertEquals("zoom should match", zoomLevel, builder.build().zoom, 0);
    }
}
