package com.mapbox.mapboxsdk.maps;

import android.view.Gravity;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import java.util.Arrays;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNull;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class MapboxMapOptionsTest {

    @Test
    public void testSanity() {
        assertNotNull("should not be null", new MapboxMapOptions());
    }

    @Test
    public void testCompassEnabled() {
        assertTrue(new MapboxMapOptions().compassEnabled(true).getCompassEnabled());
        assertFalse(new MapboxMapOptions().compassEnabled(false).getCompassEnabled());
    }

    @Test
    public void testCompassGravity() {
        assertEquals(Gravity.TOP | Gravity.END, new MapboxMapOptions().getCompassGravity());
        assertEquals(Gravity.BOTTOM, new MapboxMapOptions().compassGravity(Gravity.BOTTOM).getCompassGravity());
        assertNotEquals(Gravity.START, new MapboxMapOptions().compassGravity(Gravity.BOTTOM).getCompassGravity());
    }

    @Test
    public void testCompassMargins() {
        assertTrue(Arrays.equals(new int[]{0, 1, 2, 3}, new MapboxMapOptions().compassMargins(new int[]{0, 1, 2, 3}).getCompassMargins()));
        assertFalse(Arrays.equals(new int[]{0, 1, 2, 3}, new MapboxMapOptions().compassMargins(new int[]{0, 0, 0, 0}).getCompassMargins()));
    }

    @Test
    public void testMinZoom() {
        assertEquals(MapboxConstants.MINIMUM_ZOOM, new MapboxMapOptions().getMinZoom());
        assertEquals(5.0f, new MapboxMapOptions().minZoom(5.0f).getMinZoom());
        assertNotEquals(2.0f, new MapboxMapOptions().minZoom(5.0f).getMinZoom());
    }

    @Test
    public void testMaxZoom() {
        assertEquals(MapboxConstants.MAXIMUM_ZOOM, new MapboxMapOptions().getMaxZoom());
        assertEquals(5.0f, new MapboxMapOptions().maxZoom(5.0f).getMaxZoom());
        assertNotEquals(2.0f, new MapboxMapOptions().maxZoom(5.0f).getMaxZoom());
    }

    @Test
    public void testTiltGesturesEnabled() {
        assertTrue(new MapboxMapOptions().tiltGesturesEnabled(true).getTitltGesturesEnabeld());
        assertFalse(new MapboxMapOptions().tiltGesturesEnabled(false).getTitltGesturesEnabeld());
    }

    @Test
    public void testScrollGesturesEnabled() {
        assertTrue(new MapboxMapOptions().scrollGesturesEnabled(true).getScrollGesturesEnabled());
        assertFalse(new MapboxMapOptions().scrollGesturesEnabled(false).getScrollGesturesEnabled());
    }

    @Test
    public void testZoomGesturesEnabled() {
        assertTrue(new MapboxMapOptions().zoomGesturesEnabled(true).getZoomGesturesEnabled());
        assertFalse(new MapboxMapOptions().zoomGesturesEnabled(false).getZoomGesturesEnabled());
    }

    @Test
    public void testRotateGesturesEnabled() {
        assertTrue(new MapboxMapOptions().rotateGesturesEnabled(true).getRotateGesturesEnabled());
        assertFalse(new MapboxMapOptions().rotateGesturesEnabled(false).getRotateGesturesEnabled());
    }

    @Test
    public void testZoomControlsEnabled() {
        assertTrue(new MapboxMapOptions().zoomControlsEnabled(true).getZoomControlsEnabled());
        assertFalse(new MapboxMapOptions().zoomControlsEnabled(false).getZoomControlsEnabled());
    }

    @Test
    public void testStyleUrl() {
        assertEquals(Style.DARK, new MapboxMapOptions().styleUrl(Style.DARK).getStyle());
        assertNotEquals(Style.LIGHT, new MapboxMapOptions().styleUrl(Style.DARK).getStyle());
        assertNull(new MapboxMapOptions().getStyle());
    }

    @Test
    public void testCamera() {
        CameraPosition position = new CameraPosition.Builder().build();
        assertEquals(new CameraPosition.Builder(position).build(), new MapboxMapOptions().camera(position).getCamera());
        assertNotEquals(new CameraPosition.Builder().target(new LatLng(1, 1)), new MapboxMapOptions().camera(position));
        assertNull(new MapboxMapOptions().getCamera());
    }

    @Test
    public void testAccessToken() {
        assertNull(new MapboxMapOptions().getAccessToken());
        assertEquals("test", new MapboxMapOptions().accessToken("test").getAccessToken());
        assertNotEquals("nottest", new MapboxMapOptions().accessToken("test").getStyle());
    }

}

