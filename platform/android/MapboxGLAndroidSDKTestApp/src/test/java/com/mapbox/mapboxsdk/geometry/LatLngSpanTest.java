package com.mapbox.mapboxsdk.geometry;

import com.mapbox.mapboxsdk.geometry.LatLngSpan;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class LatLngSpanTest {

    private static final double DELTA = 1e-15;

    @Test
    public void testSanity() {
        LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
        assertNotNull("latLngSpan should not be null", latLngSpan);
    }

    @Test
    public void testLatitudeConstructor() {
        double latitude = 1.23;
        LatLngSpan latLngSpan = new LatLngSpan(latitude, 0.0);
        assertEquals("latitude in constructor", latLngSpan.getLatitudeSpan(), latitude, DELTA);
    }

    @Test
    public void testLongitudeConstructor() {
        double longitude = 1.23;
        LatLngSpan latLngSpan = new LatLngSpan(0.0, longitude);
        assertEquals("latitude in constructor", latLngSpan.getLongitudeSpan(), longitude, DELTA);
    }

    @Test
    public void testLatitudeMethod() {
        double latitude = 1.23;
        LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
        latLngSpan.setLatitudeSpan(latitude);
        assertEquals("latitude in constructor", latLngSpan.getLatitudeSpan(), latitude, DELTA);
    }

    @Test
    public void testLongitudeMethod() {
        double longitude = 1.23;
        LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
        latLngSpan.setLongitudeSpan(longitude);
        assertEquals("latitude in constructor", latLngSpan.getLongitudeSpan(), longitude, DELTA);
    }

}
