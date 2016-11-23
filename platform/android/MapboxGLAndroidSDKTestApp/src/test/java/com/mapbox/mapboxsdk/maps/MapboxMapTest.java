package com.mapbox.mapboxsdk.maps;

import android.graphics.Color;

import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.exceptions.InvalidMarkerPositionException;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class MapboxMapTest {

    private MapboxMap mMapboxMap;

    @InjectMocks
    MapView mMapView = mock(MapView.class);

    @Mock
    MapboxMap.OnMarkerClickListener mOnMarkerClickListener;

    @Mock
    MapboxMap.OnCameraChangeListener mOnCameraChangeListener;

    @Mock
    MapboxMap.InfoWindowAdapter mInfoWindowAdapter;

    @Mock
    MapboxMap.OnScrollListener mScrollListener;

    @Mock
    MapboxMap.OnFlingListener mFlingListener;

    @Mock
    MapboxMap.OnFpsChangedListener mFpsChangedListener;

    @Mock
    MapboxMap.OnInfoWindowClickListener mWindowClickListener;

    @Mock
    MapboxMap.OnInfoWindowCloseListener mWindowCloseListener;

    @Mock
    MapboxMap.OnInfoWindowLongClickListener mWindowLongClickListener;

    @Mock
    MapboxMap.OnMyLocationChangeListener mLocationChangeListener;

    @Mock
    MapboxMap.OnMyLocationTrackingModeChangeListener mMyLocationTrackingModeChangeListener;

    @Mock
    MapboxMap.OnMyBearingTrackingModeChangeListener mMyBearingTrackingModeChangeListener;

    @Mock
    IconManager iconManager;

    @Before
    public void beforeTest() {
        MockitoAnnotations.initMocks(this);
        mMapboxMap = new MapboxMap(mMapView, iconManager);
    }

    @Test
    public void testSanity() {
        assertNotNull("mMapboxMap should not be null", mMapboxMap);
    }

    @Test
    public void testMock() {
        assertNotNull("mMapView should be mocked", mMapView);
    }

    @Test
    public void testGetMapView() {
        assertNotNull("MapView should be non null", mMapboxMap.getMapView());
    }

    //
    // UiSettings
    //

    @Test
    public void testUiSettings() {
        assertNotNull("UiSettings should not be null", mMapboxMap.getUiSettings());
    }

    //
    // MinZoomLevel
    //

    @Test
    public void testMinZoom() {
        double zoom = 10;
        mMapboxMap.setMinZoom(zoom);
        assertEquals("MinZoom should match", zoom, mMapboxMap.getMinZoom(), 0);
    }

    @Test
    public void testMaxZoom() {
        double zoom = 10;
        mMapboxMap.setMaxZoom(zoom);
        assertEquals("MaxZoom should match", zoom, mMapboxMap.getMaxZoom(), 0);
    }

    @Test
    public void testInitialZoomLevels() {
        assertEquals("MaxZoom should match", 0, mMapboxMap.getMaxZoom(), 0);
        assertEquals("MinZoom should match", 0, mMapboxMap.getMinZoom(), 0);
    }

    //
    // TrackingSettings
    //

    @Test
    public void testTrackingSettings() {
        assertNotNull("TrackingSettings should not be null", mMapboxMap.getTrackingSettings());
    }

    //
    // Projection
    //

    @Test
    public void testProjection() {
        assertNotNull("Projection should not be null", mMapboxMap.getProjection());
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

    @Test
    public void testInfoWindowAdapter() {
        mMapboxMap.setInfoWindowAdapter(mInfoWindowAdapter);
        assertEquals("InfoWindowAdpter should be the same", mInfoWindowAdapter, mMapboxMap.getInfoWindowAdapter());
    }

    //
    // Location
    //

    @Test
    public void testMyLocationEnabled() {
        when(mMapView.isPermissionsAccepted()).thenReturn(true);
        mMapboxMap.setMyLocationEnabled(true);
        assertTrue("MyLocationEnabled should be true", mMapboxMap.isMyLocationEnabled());

    }

    @Test
    public void testMyLocationDisabled() {
        when(mMapView.isPermissionsAccepted()).thenReturn(true);
        mMapboxMap.setMyLocationEnabled(false);
        assertFalse("MyLocationEnabled should be false", mMapboxMap.isMyLocationEnabled());
    }

    //
    // setters/getters interfaces
    //

    @Test
    public void testScrollListener() {
        mMapboxMap.setOnScrollListener(mScrollListener);
        assertEquals("ScrollListener should match", mScrollListener, mMapboxMap.getOnScrollListener());
    }

    @Test
    public void testFlingListener() {
        mMapboxMap.setOnFlingListener(mFlingListener);
        assertEquals("FlingListener should match", mFlingListener, mMapboxMap.getOnFlingListener());
    }

    @Test
    public void testFpsListener() {
        mMapboxMap.setOnFpsChangedListener(mFpsChangedListener);
        assertEquals("FpsListener should match", mFpsChangedListener, mMapboxMap.getOnFpsChangedListener());
    }

    @Test
    public void testInfoWindowClickListener() {
        mMapboxMap.setOnInfoWindowClickListener(mWindowClickListener);
        assertEquals("InfoWidowClickListener should match", mWindowClickListener, mMapboxMap.getOnInfoWindowClickListener());
    }

    @Test
    public void testInfoWindowCloseListener() {
        mMapboxMap.setOnInfoWindowCloseListener(mWindowCloseListener);
        assertEquals("InfoWindowCloseListener should match", mWindowCloseListener, mMapboxMap.getOnInfoWindowCloseListener());
    }

    @Test
    public void testInfoWindowLongClickListener() {
        mMapboxMap.setOnInfoWindowLongClickListener(mWindowLongClickListener);
        assertEquals("InfoWindowLongClickListener should match", mWindowLongClickListener, mMapboxMap.getOnInfoWindowLongClickListener());
    }

    @Test
    public void testOnBearingTrackingModeChangeListener() {
        mMapboxMap.setOnMyBearingTrackingModeChangeListener(mMyBearingTrackingModeChangeListener);
        assertEquals("MyBearingTrackingChangeListerner should match", mMyBearingTrackingModeChangeListener, mMapboxMap.getOnMyBearingTrackingModeChangeListener());
    }

    @Test
    public void testOnLocationTrackingModeChangeListener() {
        mMapboxMap.setOnMyLocationTrackingModeChangeListener(mMyLocationTrackingModeChangeListener);
        assertEquals("MyLocationTrackigChangeListener should match", mMyLocationTrackingModeChangeListener, mMapboxMap.getOnMyLocationTrackingModeChangeListener());
    }

    //
    // Annotations
    //

    @Test
    public void testAddMarker() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mMapboxMap.addMarker(markerOptions);
        assertTrue("Marker should be contained", mMapboxMap.getMarkers().contains(marker));
    }

    @Test(expected = InvalidMarkerPositionException.class)
    public void testAddMarkerInvalidPosition() {
        new MarkerOptions().getMarker();
    }

    @Test
    public void testAddMarkers() {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().position(new LatLng()).title("a");
        MarkerOptions markerOptions2 = new MarkerOptions().position(new LatLng()).title("b");
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        List<Marker> markers = mMapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 2", 2, mMapboxMap.getMarkers().size());
        assertTrue(mMapboxMap.getMarkers().contains(markers.get(0)));
        assertTrue(mMapboxMap.getMarkers().contains(markers.get(1)));
    }

    @Test
    public void testAddMarkersEmpty() {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        mMapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 0", 0, mMapboxMap.getMarkers().size());
    }

    @Test
    public void testAddMarkersSingleMarker() {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions = new MarkerOptions().title("a").position(new LatLng());
        markerList.add(markerOptions);
        List<Marker> markers = mMapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 1", 1, mMapboxMap.getMarkers().size());
        assertTrue(mMapboxMap.getMarkers().contains(markers.get(0)));
    }

    @Test
    public void testAddPolygon() {
        PolygonOptions polygonOptions = new PolygonOptions().add(new LatLng());
        Polygon polygon = mMapboxMap.addPolygon(polygonOptions);
        assertTrue("Polygon should be contained", mMapboxMap.getPolygons().contains(polygon));
    }

    @Test
    public void testAddEmptyPolygon() {
        PolygonOptions polygonOptions = new PolygonOptions();
        Polygon polygon = mMapboxMap.addPolygon(polygonOptions);
        assertTrue("Polygon should be ignored", !mMapboxMap.getPolygons().contains(polygon));
    }

    @Test
    public void testAddPolygons() {
        List<PolygonOptions> polygonList = new ArrayList<>();
        PolygonOptions polygonOptions1 = new PolygonOptions().fillColor(Color.BLACK).add(new LatLng());
        PolygonOptions polygonOptions2 = new PolygonOptions().fillColor(Color.WHITE).add(new LatLng());
        PolygonOptions polygonOptions3 = new PolygonOptions();
        polygonList.add(polygonOptions1);
        polygonList.add(polygonOptions2);
        polygonList.add(polygonOptions3);
        mMapboxMap.addPolygons(polygonList);
        assertEquals("Polygons size should be 2", 2, mMapboxMap.getPolygons().size());
        assertTrue(mMapboxMap.getPolygons().contains(polygonOptions1.getPolygon()));
        assertTrue(mMapboxMap.getPolygons().contains(polygonOptions2.getPolygon()));
        assertTrue("Polygon should be ignored", !mMapboxMap.getPolygons().contains(polygonOptions3.getPolygon()));
    }

    @Test
    public void addPolygonsEmpty() {
        mMapboxMap.addPolygons(new ArrayList<PolygonOptions>());
        assertEquals("Polygons size should be 0", 0, mMapboxMap.getPolygons().size());
    }

    @Test
    public void addPolygonsSingle() {
        List<PolygonOptions> polygonList = new ArrayList<>();
        PolygonOptions polygonOptions = new PolygonOptions().fillColor(Color.BLACK).add(new LatLng());
        polygonList.add(polygonOptions);
        mMapboxMap.addPolygons(polygonList);
        assertEquals("Polygons size should be 1", 1, mMapboxMap.getPolygons().size());
        assertTrue(mMapboxMap.getPolygons().contains(polygonOptions.getPolygon()));
    }

    @Test
    public void testAddPolyline() {
        PolylineOptions polylineOptions = new PolylineOptions().add(new LatLng());
        Polyline polyline = mMapboxMap.addPolyline(polylineOptions);
        assertTrue("Polyline should be contained", mMapboxMap.getPolylines().contains(polyline));
    }

    @Test
    public void testAddEmptyPolyline() {
        PolylineOptions polylineOptions = new PolylineOptions();
        Polyline polyline = mMapboxMap.addPolyline(polylineOptions);
        assertTrue("Polyline should be ignored", !mMapboxMap.getPolylines().contains(polyline));
    }

    @Test
    public void testAddPolylines() {
        List<PolylineOptions> polylineList = new ArrayList<>();
        PolylineOptions polygonOptions1 = new PolylineOptions().color(Color.BLACK).add(new LatLng());
        PolylineOptions polygonOptions2 = new PolylineOptions().color(Color.WHITE).add(new LatLng());
        PolylineOptions polygonOptions3 = new PolylineOptions();
        polylineList.add(polygonOptions1);
        polylineList.add(polygonOptions2);
        polylineList.add(polygonOptions3);
        mMapboxMap.addPolylines(polylineList);
        assertEquals("Polygons size should be 2", 2, mMapboxMap.getPolylines().size());
        assertTrue(mMapboxMap.getPolylines().contains(polygonOptions1.getPolyline()));
        assertTrue(mMapboxMap.getPolylines().contains(polygonOptions2.getPolyline()));
        assertTrue("Polyline should be ignored", !mMapboxMap.getPolylines().contains(polygonOptions3.getPolyline()));
    }

    @Test
    public void testAddPolylinesEmpty() {
        mMapboxMap.addPolylines(new ArrayList<PolylineOptions>());
        assertEquals("Polygons size should be 0", 0, mMapboxMap.getPolylines().size());
    }

    @Test
    public void testAddPolylinesSingle() {
        List<PolylineOptions> polylineList = new ArrayList<>();
        PolylineOptions polygonOptions = new PolylineOptions().color(Color.BLACK).add(new LatLng());
        polylineList.add(polygonOptions);
        mMapboxMap.addPolylines(polylineList);
        assertEquals("Polygons size should be 1", 1, mMapboxMap.getPolylines().size());
        assertTrue(mMapboxMap.getPolylines().contains(polygonOptions.getPolyline()));
    }

    @Test
    public void testRemoveMarker() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mMapboxMap.addMarker(markerOptions);
        mMapboxMap.removeMarker(marker);
        assertTrue("Markers should be empty", mMapboxMap.getMarkers().isEmpty());
    }

    @Test
    public void testRemovePolygon() {
        PolygonOptions polygonOptions = new PolygonOptions();
        Polygon polygon = mMapboxMap.addPolygon(polygonOptions);
        mMapboxMap.removePolygon(polygon);
        assertTrue("Polygons should be empty", mMapboxMap.getPolylines().isEmpty());
    }

    @Test
    public void testRemovePolyline() {
        PolylineOptions polylineOptions = new PolylineOptions();
        Polyline polyline = mMapboxMap.addPolyline(polylineOptions);
        mMapboxMap.removePolyline(polyline);
        assertTrue("Polylines should be empty", mMapboxMap.getPolylines().isEmpty());
    }

    @Test
    public void testRemoveAnnotation() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mMapboxMap.addMarker(markerOptions);
        mMapboxMap.removeAnnotation(marker);
        assertTrue("Annotations should be empty", mMapboxMap.getAnnotations().isEmpty());
    }

    @Test
    public void testRemoveAnnotationById() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        mMapboxMap.addMarker(markerOptions);
        // id will always be 0 in unit tests
        mMapboxMap.removeAnnotation(0);
        assertTrue("Annotations should be empty", mMapboxMap.getAnnotations().isEmpty());
    }

    @Test
    public void testRemoveAnnotations() {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        mMapboxMap.addMarkers(markerList);
        mMapboxMap.removeAnnotations();
        assertTrue("Annotations should be empty", mMapboxMap.getAnnotations().isEmpty());
    }

    @Test
    public void testClear() {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        mMapboxMap.addMarkers(markerList);
        mMapboxMap.clear();
        assertTrue("Annotations should be empty", mMapboxMap.getAnnotations().isEmpty());
    }

    @Test
    public void testRemoveAnnotationsByList() {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        List<Marker> markers = mMapboxMap.addMarkers(markerList);
        Marker marker = mMapboxMap.addMarker(new MarkerOptions().position(new LatLng()).title("c"));
        mMapboxMap.removeAnnotations(markers);
        assertTrue("Annotations should not be empty", mMapboxMap.getAnnotations().size() == 1);
        assertTrue("Marker should be contained", mMapboxMap.getAnnotations().contains(marker));
    }

    @Test
    public void testGetAnnotationById() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker initialMarker = mMapboxMap.addMarker(markerOptions);
        Marker retrievedMarker = (Marker) mMapboxMap.getAnnotation(0);
        assertEquals("Markers should match", initialMarker, retrievedMarker);
    }

    @Test
    public void testGetAnnotations() {
        assertNotNull("Annotations should be non null", mMapboxMap.getAnnotations());
    }

    @Test
    public void testGetMarkers() {
        assertNotNull("Markers should be non null", mMapboxMap.getMarkers());
    }

    @Test
    public void testGetPolygons() {
        assertNotNull("Polygons should be non null", mMapboxMap.getPolygons());
    }

    @Test
    public void testGetPolylines() {
        assertNotNull("Polylines should be non null", mMapboxMap.getPolylines());
    }

    @Test
    public void testGetSelectedMarkers() {
        assertNotNull("Selected markers should be non null", mMapboxMap.getSelectedMarkers());
    }

    @Test
    public void testSelectMarker() {
        mMapboxMap.setOnMarkerClickListener(mOnMarkerClickListener);
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mMapboxMap.addMarker(markerOptions);
        when(mOnMarkerClickListener.onMarkerClick(marker)).thenReturn(true);
        mMapboxMap.selectMarker(marker);
        assertTrue("Marker should be contained", mMapboxMap.getSelectedMarkers().contains(marker));
    }

    @Test
    public void testDeselectMarker() {
        mMapboxMap.setOnMarkerClickListener(mOnMarkerClickListener);
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mMapboxMap.addMarker(markerOptions);
        when(mOnMarkerClickListener.onMarkerClick(marker)).thenReturn(true);
        mMapboxMap.selectMarker(marker);
        mMapboxMap.deselectMarker(marker);
        assertTrue("Selected markers should be empty", mMapboxMap.getSelectedMarkers().isEmpty());
    }

    @Test
    public void testDeselectMarkers() {
        mMapboxMap.setOnMarkerClickListener(mOnMarkerClickListener);
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker1 = mMapboxMap.addMarker(markerOptions);
        Marker marker2 = mMapboxMap.addMarker(markerOptions);
        when(mOnMarkerClickListener.onMarkerClick(marker1)).thenReturn(true);
        when(mOnMarkerClickListener.onMarkerClick(marker2)).thenReturn(true);
        mMapboxMap.selectMarker(marker1);
        mMapboxMap.selectMarker(marker2);
        mMapboxMap.deselectMarkers();
        assertTrue("Selected markers should be empty", mMapboxMap.getSelectedMarkers().isEmpty());
    }

    //
    // OnMarkerClick interface
    //

    @Test
    public void testOnMarkerClick() {
        mMapboxMap.setOnMarkerClickListener(mOnMarkerClickListener);
        Marker marker = new MarkerOptions().position(new LatLng()).getMarker();
        when(mOnMarkerClickListener.onMarkerClick(marker)).thenReturn(true);
        mMapboxMap.selectMarker(marker);
        verify(mOnMarkerClickListener, times(1)).onMarkerClick(marker);
    }

}
