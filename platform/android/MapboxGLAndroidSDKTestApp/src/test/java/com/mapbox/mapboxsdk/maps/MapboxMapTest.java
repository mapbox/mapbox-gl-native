package com.mapbox.mapboxsdk.maps;

import android.graphics.Point;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

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

    //
    // Test UiSettings
    //

    @Test
    public void testUiSettings() {
        assertNotNull("UiSettings should not be null", mMapboxMap.getUiSettings());
    }


    //
    // InfoWindow
    //

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

    //
    // Location
    //

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

    //
    // Style
    //

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

    //
    // CameraPosition
    //

    @Test
    public void testCameraPosition() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    //
    // Camera - moveCamera
    //

    @Test
    public void testNewCameraPositionMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(position));
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    //
    // Camera - animateCamera
    //

    @Test
    public void testNewCameraPositionAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position));
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testNewCameraPositionAnimateCameraWithCallbackParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), null);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testNewCameraPositionAnimateCameraWithDurationParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), 0);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testNewCameraPositionAnimateCameraWithDurationAndCallbackParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), 0, null);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    //
    // Camera - easeCamera
    //

    @Test
    public void testNewCameraPositionEaseCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(position), 1000);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    @Test
    public void testNewCameraPositionEaseCameraWithCallbackParameter() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(position), 1000, null);
        assertEquals("CameraPosition should be same", position, mMapboxMap.getCameraPosition());
    }

    //
    // Camera - LatLng
    //

    @Test
    public void testLatLngMoveCamera() {
        mMapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(4, 5)));
        assertEquals("LatLng should be same", new LatLng(4, 5), mMapboxMap.getCameraPosition().target);
    }

    @Test
    public void testLatLngAnimateCamera() {
        mMapboxMap.animateCamera(CameraUpdateFactory.newLatLng(new LatLng(4, 5)));
        assertEquals("LatLng should be same", new LatLng(4, 5), mMapboxMap.getCameraPosition().target);
    }

    @Test
    public void testLatLngEaseCamera() {
        mMapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(4, 5)), 1000);
        assertEquals("LatLng should be same", new LatLng(4, 5), mMapboxMap.getCameraPosition().target);
    }

    //
    // Camera - LatLngZoom
    //

    @Test
    public void testLatLngZoomMoveCamera() {
        mMapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(4, 5), 10));
        assertEquals("LatLng should be same", new LatLng(4, 5), mMapboxMap.getCameraPosition().target);
        assertTrue("Zoomlevel should be same", 10 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testLatLngZoomAnimateCamera() {
        mMapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(4, 5), 10));
        assertEquals("LatLng should be same", new LatLng(4, 5), mMapboxMap.getCameraPosition().target);
        assertTrue("Zoomlevel should be same", 10 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testLatLngZoomEaseCamera() {
        mMapboxMap.easeCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(4, 5), 10), 1000);
        assertEquals("LatLng should be same", new LatLng(4, 5), mMapboxMap.getCameraPosition().target);
        assertTrue("Zoomlevel should be same", 10 == mMapboxMap.getCameraPosition().zoom);
    }

    //
    // Camera - Zoom
    //

    @Test
    public void testZoomInMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.moveCamera(CameraUpdateFactory.zoomIn());
        assertTrue("Zoomlevel should be same", 4 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomOutMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.moveCamera(CameraUpdateFactory.zoomOut());
        assertTrue("Zoomlevel should be same", 2 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomByMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.moveCamera(CameraUpdateFactory.zoomBy(3));
        assertTrue("Zoomlevel should be same", 6 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomByPointMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.moveCamera(CameraUpdateFactory.zoomBy(3, new Point(0, 0)));
        assertTrue("Zoomlevel should be same", 6 == mMapboxMap.getCameraPosition().zoom);
        // todo calculate and check LatLng
    }

    @Test
    public void testZoomToMoveCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.moveCamera(CameraUpdateFactory.zoomTo(12));
        assertTrue("Zoomlevel should be same", 12 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomInAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.animateCamera(CameraUpdateFactory.zoomIn());
        assertTrue("Zoomlevel should be same", 4 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomOutAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.animateCamera(CameraUpdateFactory.zoomOut());
        assertTrue("Zoomlevel should be same", 2 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomByAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.animateCamera(CameraUpdateFactory.zoomBy(3));
        assertTrue("Zoomlevel should be same", 6 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomToAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.animateCamera(CameraUpdateFactory.zoomTo(12));
        assertTrue("Zoomlevel should be same", 12 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomByPointAnimateCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.animateCamera(CameraUpdateFactory.zoomBy(1, new Point(0, 0)));
        assertTrue("Zoomlevel should be same", 4 == mMapboxMap.getCameraPosition().zoom);
        // todo calculate and check LatLng
    }

    @Test
    public void testZoomInEaseCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.easeCamera(CameraUpdateFactory.zoomIn(), 1000);
        assertTrue("Zoomlevel should be same", 4 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomOutEaseCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.easeCamera(CameraUpdateFactory.zoomOut(), 1000);
        assertTrue("Zoomlevel should be same", 2 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomByEaseCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.easeCamera(CameraUpdateFactory.zoomBy(3), 1000);
        assertTrue("Zoomlevel should be same", 6 == mMapboxMap.getCameraPosition().zoom);
    }

    @Test
    public void testZoomByPointCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.easeCamera(CameraUpdateFactory.zoomBy(3, new Point(0, 0)), 1000);
        assertTrue("Zoomlevel should be same", 6 == mMapboxMap.getCameraPosition().zoom);
        // todo calculate and check LatLng
    }

    @Test
    public void testZoomToEaseCamera() {
        CameraPosition position = new CameraPosition.Builder().bearing(1).tilt(2).zoom(3).target(new LatLng(4, 5)).build();
        mMapboxMap.setCameraPosition(position);
        mMapboxMap.easeCamera(CameraUpdateFactory.zoomTo(12), 1000);
        assertTrue("Zoomlevel should be same", 12 == mMapboxMap.getCameraPosition().zoom);
    }

    //
    // OnMarkerClick interface
    //

    @Test
    public void testOnMarkerClick() {
        mMapboxMap.setOnMarkerClickListener(mOnMarkerClickListener);
        Marker marker = new MarkerOptions().getMarker();
        when(mOnMarkerClickListener.onMarkerClick(marker)).thenReturn(true);
        mMapboxMap.selectMarker(marker);
        verify(mOnMarkerClickListener, times(1)).onMarkerClick(marker);
    }

}
