package com.mapbox.mapboxsdk.maps;

import android.util.Log;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.lang.reflect.Constructor;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class MapboxMapTest {

    private MapboxMap mMapboxMap;

    @InjectMocks
    MapView mMapView = mock(MapView.class);

    @Mock
    MapboxMap.OnMarkerClickListener mOnMarkerClickListener;

    @Before
    public void beforeTest() {
        MockitoAnnotations.initMocks(this);
        mMapboxMap = new MapboxMap(mMapView);
    }

    @Test
    public void testSanity() {
        assertNotNull("mMapboxMap should not be null", mMapboxMap);
    }

    @Test
    public void testCompassEnabled() {
        mMapboxMap.setCompassEnabled(true);
        assertTrue("CompassEnabled should be true", mMapboxMap.isCompassEnabled());
    }

    @Test
    public void testCompassDisabled() {
        mMapboxMap.setCompassEnabled(false);
        assertFalse("CompassbEnabled should be false", mMapboxMap.isCompassEnabled());
    }

    @Test
    public void testScrollEnabled() {
        mMapboxMap.setScrollEnabled(true);
        assertTrue("ScrollEnabled should be true", mMapboxMap.isScrollEnabled());
    }

    @Test
    public void testScrollDisabled() {
        mMapboxMap.setScrollEnabled(false);
        assertFalse("ScrollEnabled should be false", mMapboxMap.isScrollEnabled());
    }

    @Test
    public void testRotateEnabled() {
        mMapboxMap.setRotateEnabled(true);
        assertTrue("RotateEnabled should be true", mMapboxMap.isRotateEnabled());
    }

    @Test
    public void testRotateDisabled() {
        mMapboxMap.setRotateEnabled(false);
        assertFalse("RotateDisabled should be false", mMapboxMap.isRotateEnabled());
    }

    @Test
    public void testZoomEnabled() {
        mMapboxMap.setZoomEnabled(true);
        assertTrue("ZoomEnabled should be true", mMapboxMap.isZoomEnabled());
    }

    @Test
    public void testZoomDisabled() {
        mMapboxMap.setZoomEnabled(false);
        assertFalse("ZoomEnabled should be false", mMapboxMap.isZoomEnabled());
    }

    @Test
    public void testZoomControlsEnabled() {
        mMapboxMap.setZoomControlsEnabled(true);
        assertTrue("ZoomControlsEnabled should be true", mMapboxMap.isZoomControlsEnabled());
    }

    @Test
    public void testZoomControlsDisabled() {
        mMapboxMap.setZoomControlsEnabled(false);
        assertFalse("ZoomControlsEnabled should be false", mMapboxMap.isZoomControlsEnabled());
    }

    @Test
    public void testTiltEnabled() {
        mMapboxMap.setTiltEnabled(true);
        assertTrue("TiltEnabled should be true", mMapboxMap.isTiltEnabled());
    }

    @Test
    public void testTiltDisabled() {
        mMapboxMap.setTiltEnabled(false);
        assertFalse("TiltEnabled should be false", mMapboxMap.isTiltEnabled());
    }

    @Test
    public void testConcurrentInfoWindowEnabled() {
        mMapboxMap.setAllowConcurrentMultipleOpenInfoWindows(true);
        assertTrue("ConcurrentWindows should be true", mMapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
    }

    @Test
    public void testConcurrentInfoWindowDisabled() {
        mMapboxMap.setAllowConcurrentMultipleOpenInfoWindows(false);
        assertFalse("ConcurrentWindows should be false", mMapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
    }

    @Test
    public void testMyLocationEnabled() {
        try {
            mMapboxMap.setMyLocationEnabled(true);
            assertTrue("MyLocationEnabled should be true", mMapboxMap.isMyLocationEnabled());
        } catch (SecurityException e) {
            assertTrue(false);
        }
    }

    @Test
    public void testMyLocationDisabled() {
        try {
            mMapboxMap.setMyLocationEnabled(false);
            assertFalse("MyLocationEnabled should be false", mMapboxMap.isMyLocationEnabled());
        } catch (SecurityException e) {
            assertTrue(false);
        }
    }

    @Test
    public void testStyleUrl() {
        mMapboxMap.setStyleUrl("somestyle");
        assertEquals("StyleUrl should be same", "somestyle", mMapboxMap.getStyleUrl());
    }

    @Test
    public void testStyle() {
        mMapboxMap.setStyle(Style.MAPBOX_STREETS);
        assertEquals("Style should be same", Style.MAPBOX_STREETS, mMapboxMap.getStyleUrl());
    }

    @Test
    public void testCameraPosition() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(position));
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position));
        assertEquals("CameraPosition should be same", position , mMapboxMap.getCameraPosition());
    }

    @Test
    public void testAnimateCameraWithCallbackParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), null);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testAnimateCameraWithDurationParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), 0);
        assertEquals("CameraPosition should be same",position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testAnimateCameraWithDurationAndCallbackParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), 0, null);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testEaseCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(position), 1000);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testEaseCameraWithCallbackParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(position), 1000, null);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testOnMarkerClick() {
        mMapboxMap.setOnMarkerClickListener(mOnMarkerClickListener);
        Marker marker = new MarkerOptions().getMarker();
        when(mOnMarkerClickListener.onMarkerClick(marker)).thenReturn(true);
        mMapboxMap.selectMarker(marker);
        verify(mOnMarkerClickListener, times(1)).onMarkerClick(marker);
    }

}
