package com.mapbox.mapboxsdk.maps;

import android.view.Gravity;

import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

public class UiSettingsTest {

    @InjectMocks
    MapView mMapView = mock(MapView.class);

    @Test
    public void testSanity() {
        UiSettings uiSettings = new UiSettings(mMapView);
        assertNotNull("uiSettings should not be null", uiSettings);
    }

    @Test
    public void testCompassEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setCompassEnabled(true);
        assertEquals("Compass should be enabled", true, uiSettings.isCompassEnabled());
    }

    @Test
    public void testCompassDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setCompassEnabled(false);
        assertEquals("Compass should be disabled", false, uiSettings.isCompassEnabled());
    }

    @Test
    public void testCompassGravity() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setCompassGravity(Gravity.LEFT);
        assertEquals("Compass gravity should be same", Gravity.LEFT, uiSettings.getCompassGravity());
    }

    @Test
    public void testCompassMargins() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setCompassMargins(1, 2, 3, 4);
        assertTrue("Compass margin left should be same", uiSettings.getCompassMarginLeft() == 1);
        assertTrue("Compass margin top should be same", uiSettings.getCompassMarginTop() == 2);
        assertTrue("Compass margin right should be same", uiSettings.getCompassMarginRight() == 3);
        assertTrue("Compass margin bottom should be same", uiSettings.getCompassMarginBottom() == 4);
    }

    @Test
    public void testLogoEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setLogoEnabled(true);
        assertEquals("Logo should be enabled", true, uiSettings.isLogoEnabled());
    }

    @Test
    public void testLogoDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setLogoEnabled(false);
        assertEquals("Logo should be disabled", false, uiSettings.isLogoEnabled());
    }

    @Test
    public void testLogoGravity() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setLogoGravity(Gravity.RIGHT);
        assertEquals("Logo gravity should be same", Gravity.RIGHT, uiSettings.getLogoGravity());
    }

    @Test
    public void testLogoMargins() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setLogoMargins(1, 2, 3, 4);
        assertTrue("Compass margin left should be same", uiSettings.getLogoMarginLeft() == 1);
        assertTrue("Compass margin top should be same", uiSettings.getLogoMarginTop() == 2);
        assertTrue("Compass margin right should be same", uiSettings.getLogoMarginRight() == 3);
        assertTrue("Compass margin bottom should be same", uiSettings.getLogoMarginBottom() == 4);
    }

    @Test
    public void testAttributionEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setAttributionEnabled(true);
        assertEquals("Attribution should be enabled", true, uiSettings.isAttributionEnabled());
    }

    @Test
    public void testAttributionDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setAttributionEnabled(false);
        assertEquals("Attribution should be disabled", false, uiSettings.isLogoEnabled());
    }

    @Test
    public void testAttributionGravity() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setAttributionGravity(Gravity.RIGHT);
        assertEquals("Attribution gravity should be same", Gravity.RIGHT, uiSettings.getAttributionGravity());
    }

    @Test
    public void testAttributionMargins() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setAttributionMargins(1, 2, 3, 4);
        assertTrue("Attribution margin left should be same", uiSettings.getAttributionMarginLeft() == 1);
        assertTrue("Attribution margin top should be same", uiSettings.getAttributionMarginTop() == 2);
        assertTrue("Attribution margin right should be same", uiSettings.getAttributionMarginRight() == 3);
        assertTrue("Attribution margin bottom should be same", uiSettings.getAttributionMarginBottom() == 4);
    }

    @Test
    public void testRotateGesturesEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setRotateGesturesEnabled(true);
        assertEquals("Rotate gesture should be enabled", true, uiSettings.isRotateGesturesEnabled());
    }

    @Test
    public void testRotateGesturesDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setRotateGesturesEnabled(false);
        assertEquals("Rotate gesture should be disabled", false, uiSettings.isRotateGesturesEnabled());
    }

    @Test
    public void testTiltGesturesEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setTiltGesturesEnabled(true);
        assertEquals("Tilt gesture should be enabled", true, uiSettings.isTiltGesturesEnabled());
    }

    @Test
    public void testTiltGesturesDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setTiltGesturesEnabled(false);
        assertEquals("Tilt gesture should be disabled", false, uiSettings.isTiltGesturesEnabled());
    }

    @Test
    public void testZoomGesturesEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setZoomGesturesEnabled(true);
        assertEquals("Zoom gesture should be enabled", true, uiSettings.isZoomGesturesEnabled());
    }

    @Test
    public void testZoomGesturesDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setZoomGesturesEnabled(false);
        assertEquals("Zoom gesture should be disabled", false, uiSettings.isZoomGesturesEnabled());
    }

    @Test
    public void testZoomControlsEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setZoomControlsEnabled(true);
        assertEquals("Zoom controls should be enabled", true, uiSettings.isZoomControlsEnabled());
    }

    @Test
    public void testZoomControlsDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setZoomControlsEnabled(false);
        assertEquals("Zoom controls should be disabled", false, uiSettings.isZoomControlsEnabled());
    }

    @Test
    public void testScrollGesturesEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setScrollGesturesEnabled(true);
        assertEquals("Scroll gesture should be enabled", true, uiSettings.isScrollGesturesEnabled());
    }

    @Test
    public void testScrollGesturesDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setScrollGesturesEnabled(false);
        assertEquals("Scroll gesture should be disabled", false, uiSettings.isScrollGesturesEnabled());
    }

    @Test
    public void testAllGesturesEnabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setAllGesturesEnabled(true);
        assertEquals("Rotate gesture should be enabled", true, uiSettings.isRotateGesturesEnabled());
        assertEquals("Tilt gesture should be enabled", true, uiSettings.isTiltGesturesEnabled());
        assertEquals("Zoom gesture should be enabled", true, uiSettings.isZoomGesturesEnabled());
        assertEquals("Scroll gesture should be enabled", true, uiSettings.isScrollGesturesEnabled());
    }

    @Test
    public void testAllGesturesDisabled() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.setAllGesturesEnabled(false);
        assertEquals("Rotate gesture should be enabled", false, uiSettings.isRotateGesturesEnabled());
        assertEquals("Tilt gesture should be disabled", false, uiSettings.isTiltGesturesEnabled());
        assertEquals("Zoom gesture should be disabled", false, uiSettings.isZoomGesturesEnabled());
        assertEquals("Scroll gesture should be disabled", false, uiSettings.isScrollGesturesEnabled());
    }


    @Test
    public void testInvalidate() {
        UiSettings uiSettings = new UiSettings(mMapView);
        uiSettings.invalidate();
    }

}