package com.mapbox.mapboxsdk.maps;

import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidMarkerPositionException;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import timber.log.Timber;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static junit.framework.TestCase.assertFalse;
import static junit.framework.TestCase.assertNotNull;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

/**
 * This test is responsible for testing the public API.
 */
public class MapboxMapTest {

    @Rule
    public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

    private OnMapReadyIdlingResource idlingResource;
    private EspressoTestActivity activity;

    @Before
    public void beforeTest() {
        activity = rule.getActivity();
        idlingResource = new OnMapReadyIdlingResource(activity);
        Espresso.registerIdlingResources(idlingResource);
    }

    @Test
    @Ignore
    public void testSanity() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("mapboxMap should not be null", mapboxMap);
    }

    //
    // MinZoomLevel
    //

    @Test
    @Ignore
    public void testMinZoom() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        onView(withId(R.id.mapView)).perform(new MinZoomAction(mapboxMap));
        assertEquals("MinZoom should match", 10, mapboxMap.getMinZoom(), 0);
    }

    @Test
    @Ignore
    public void testMaxZoom() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        double zoom = 10;
        mapboxMap.setMaxZoom(zoom);
        assertEquals("MaxZoom should match", zoom, mapboxMap.getMaxZoom(), 0);
    }

    @Test
    @Ignore
    public void testInitialZoomLevels() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertEquals("MaxZoom should match", MapboxConstants.MAXIMUM_ZOOM, mapboxMap.getMaxZoom(), 0);
        assertEquals("MinZoom should match", MapboxConstants.MINIMUM_ZOOM, mapboxMap.getMinZoom(), 0);
    }

    //
    // TrackingSettings
    //

    @Test
    @Ignore
    public void testTrackingSettings() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("TrackingSettings should not be null", mapboxMap.getTrackingSettings());
    }

    //
    // InfoWindow
    //

    @Test
    @Ignore
    public void testConcurrentInfoWindowEnabled() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        mapboxMap.setAllowConcurrentMultipleOpenInfoWindows(true);
        assertTrue("ConcurrentWindows should be true", mapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
    }

    @Test
    @Ignore
    public void testConcurrentInfoWindowDisabled() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        mapboxMap.setAllowConcurrentMultipleOpenInfoWindows(false);
        assertFalse("ConcurrentWindows should be false", mapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
    }

    @Test
    @Ignore
    public void testInfoWindowAdapter() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MapboxMap.InfoWindowAdapter infoWindowAdapter = new MapboxMap.InfoWindowAdapter() {
            @Nullable
            @Override
            public View getInfoWindow(@NonNull Marker marker) {
                return null;
            }
        };
        mapboxMap.setInfoWindowAdapter(infoWindowAdapter);
        assertEquals("InfoWindowAdpter should be the same", infoWindowAdapter, mapboxMap.getInfoWindowAdapter());
    }

    //
    // Location
    //

    @Test
    @Ignore /* disabled due to enabling permissions during test #7177 */
    public void testMyLocationEnabled() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        mapboxMap.setMyLocationEnabled(true);
        assertTrue("MyLocationEnabled should be true", mapboxMap.isMyLocationEnabled());
    }

    @Test
    @Ignore /* can't create handler inside thread that not called Looper.prepare() */
    public void testMyLocationDisabled() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        mapboxMap.setMyLocationEnabled(false);
        assertFalse("MyLocationEnabled should be false", mapboxMap.isMyLocationEnabled());
    }

    //
    // setters/getters interfaces
    //

    @Test
    @Ignore
    public void testFpsListener() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MapboxMap.OnFpsChangedListener fpsChangedListener = new MapboxMap.OnFpsChangedListener() {
            @Override
            public void onFpsChanged(double fps) {

            }
        };
        mapboxMap.setOnFpsChangedListener(fpsChangedListener);
        assertEquals("FpsListener should match", fpsChangedListener, mapboxMap.getOnFpsChangedListener());
    }

    @Test
    @Ignore
    public void testInfoWindowClickListener() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MapboxMap.OnInfoWindowClickListener clickListener = new MapboxMap.OnInfoWindowClickListener() {
            @Override
            public boolean onInfoWindowClick(@NonNull Marker marker) {
                return false;
            }
        };
        mapboxMap.setOnInfoWindowClickListener(clickListener);
        assertEquals("InfoWidowClickListener should match", clickListener, mapboxMap.getOnInfoWindowClickListener());
    }

    @Test
    @Ignore
    public void testInfoWindowCloseListener() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MapboxMap.OnInfoWindowCloseListener listener = new MapboxMap.OnInfoWindowCloseListener() {
            @Override
            public void onInfoWindowClose(Marker marker) {

            }
        };
        mapboxMap.setOnInfoWindowCloseListener(listener);
        assertEquals("InfoWindowCloseListener should match", listener, mapboxMap.getOnInfoWindowCloseListener());
    }

    @Test
    @Ignore
    public void testInfoWindowLongClickListener() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MapboxMap.OnInfoWindowLongClickListener listener = new MapboxMap.OnInfoWindowLongClickListener() {
            @Override
            public void onInfoWindowLongClick(Marker marker) {

            }
        };
        mapboxMap.setOnInfoWindowLongClickListener(listener);
        assertEquals("InfoWindowLongClickListener should match", listener, mapboxMap.getOnInfoWindowLongClickListener());
    }

    //
    // Annotations
    //

    @Test
    @Ignore
    public void testAddMarker() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        assertTrue("Marker should be contained", mapboxMap.getMarkers().contains(marker));
    }

    @Test(expected = InvalidMarkerPositionException.class)
    @Ignore
    public void testAddMarkerInvalidPosition() {
        new MarkerOptions().getMarker();
    }

    @Test
    @Ignore
    public void testAddMarkers() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().position(new LatLng()).title("a");
        MarkerOptions markerOptions2 = new MarkerOptions().position(new LatLng()).title("b");
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        List<Marker> markers = mapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 2", 2, mapboxMap.getMarkers().size());
        assertTrue(mapboxMap.getMarkers().contains(markers.get(0)));
        assertTrue(mapboxMap.getMarkers().contains(markers.get(1)));
    }

    @Test
    @Ignore
    public void testAddMarkersEmpty() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        mapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 0", 0, mapboxMap.getMarkers().size());
    }

    @Test
    @Ignore
    public void testAddMarkersSingleMarker() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions = new MarkerOptions().title("a").position(new LatLng());
        markerList.add(markerOptions);
        List<Marker> markers = mapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 1", 1, mapboxMap.getMarkers().size());
        assertTrue(mapboxMap.getMarkers().contains(markers.get(0)));
    }

    @Test
    @Ignore
    public void testAddPolygon() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        PolygonOptions polygonOptions = new PolygonOptions().add(new LatLng());
        Polygon polygon = mapboxMap.addPolygon(polygonOptions);
        assertTrue("Polygon should be contained", mapboxMap.getPolygons().contains(polygon));
    }

    @Test
    @Ignore
    public void testAddEmptyPolygon() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        PolygonOptions polygonOptions = new PolygonOptions();
        Polygon polygon = mapboxMap.addPolygon(polygonOptions);
        assertTrue("Polygon should be ignored", !mapboxMap.getPolygons().contains(polygon));
    }

    @Test
    @Ignore
    public void testAddPolygons() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<PolygonOptions> polygonList = new ArrayList<>();
        PolygonOptions polygonOptions1 = new PolygonOptions().fillColor(Color.BLACK).add(new LatLng());
        PolygonOptions polygonOptions2 = new PolygonOptions().fillColor(Color.WHITE).add(new LatLng());
        PolygonOptions polygonOptions3 = new PolygonOptions();
        polygonList.add(polygonOptions1);
        polygonList.add(polygonOptions2);
        polygonList.add(polygonOptions3);
        mapboxMap.addPolygons(polygonList);
        assertEquals("Polygons size should be 2", 2, mapboxMap.getPolygons().size());
        assertTrue(mapboxMap.getPolygons().contains(polygonOptions1.getPolygon()));
        assertTrue(mapboxMap.getPolygons().contains(polygonOptions2.getPolygon()));
        assertTrue("Polygon should be ignored", !mapboxMap.getPolygons().contains(polygonOptions3.getPolygon()));
    }

    @Test
    @Ignore
    public void addPolygonsEmpty() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        mapboxMap.addPolygons(new ArrayList<PolygonOptions>());
        assertEquals("Polygons size should be 0", 0, mapboxMap.getPolygons().size());
    }

    @Test
    @Ignore
    public void addPolygonsSingle() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<PolygonOptions> polygonList = new ArrayList<>();
        PolygonOptions polygonOptions = new PolygonOptions().fillColor(Color.BLACK).add(new LatLng());
        polygonList.add(polygonOptions);
        mapboxMap.addPolygons(polygonList);
        assertEquals("Polygons size should be 1", 1, mapboxMap.getPolygons().size());
        assertTrue(mapboxMap.getPolygons().contains(polygonOptions.getPolygon()));
    }

    @Test
    @Ignore
    public void testAddPolyline() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        PolylineOptions polylineOptions = new PolylineOptions().add(new LatLng());
        Polyline polyline = mapboxMap.addPolyline(polylineOptions);
        assertTrue("Polyline should be contained", mapboxMap.getPolylines().contains(polyline));
    }

    @Test
    @Ignore
    public void testAddEmptyPolyline() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        PolylineOptions polylineOptions = new PolylineOptions();
        Polyline polyline = mapboxMap.addPolyline(polylineOptions);
        assertTrue("Polyline should be ignored", !mapboxMap.getPolylines().contains(polyline));
    }

    @Test
    @Ignore
    public void testAddPolylines() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<PolylineOptions> polylineList = new ArrayList<>();
        PolylineOptions polygonOptions1 = new PolylineOptions().color(Color.BLACK).add(new LatLng());
        PolylineOptions polygonOptions2 = new PolylineOptions().color(Color.WHITE).add(new LatLng());
        PolylineOptions polygonOptions3 = new PolylineOptions();
        polylineList.add(polygonOptions1);
        polylineList.add(polygonOptions2);
        polylineList.add(polygonOptions3);
        mapboxMap.addPolylines(polylineList);
        assertEquals("Polygons size should be 2", 2, mapboxMap.getPolylines().size());
        assertTrue(mapboxMap.getPolylines().contains(polygonOptions1.getPolyline()));
        assertTrue(mapboxMap.getPolylines().contains(polygonOptions2.getPolyline()));
        assertTrue("Polyline should be ignored", !mapboxMap.getPolylines().contains(polygonOptions3.getPolyline()));
    }

    @Test
    @Ignore
    public void testAddPolylinesEmpty() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        mapboxMap.addPolylines(new ArrayList<PolylineOptions>());
        assertEquals("Polygons size should be 0", 0, mapboxMap.getPolylines().size());
    }

    @Test
    @Ignore
    public void testAddPolylinesSingle() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<PolylineOptions> polylineList = new ArrayList<>();
        PolylineOptions polygonOptions = new PolylineOptions().color(Color.BLACK).add(new LatLng());
        polylineList.add(polygonOptions);
        mapboxMap.addPolylines(polylineList);
        assertEquals("Polygons size should be 1", 1, mapboxMap.getPolylines().size());
        assertTrue(mapboxMap.getPolylines().contains(polygonOptions.getPolyline()));
    }

    @Test
    @Ignore
    public void testRemoveMarker() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.removeMarker(marker);
        assertTrue("Markers should be empty", mapboxMap.getMarkers().isEmpty());
    }

    @Test
    @Ignore
    public void testRemovePolygon() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        PolygonOptions polygonOptions = new PolygonOptions();
        Polygon polygon = mapboxMap.addPolygon(polygonOptions);
        mapboxMap.removePolygon(polygon);
        assertTrue("Polygons should be empty", mapboxMap.getPolylines().isEmpty());
    }

    @Test
    @Ignore
    public void testRemovePolyline() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        PolylineOptions polylineOptions = new PolylineOptions();
        Polyline polyline = mapboxMap.addPolyline(polylineOptions);
        mapboxMap.removePolyline(polyline);
        assertTrue("Polylines should be empty", mapboxMap.getPolylines().isEmpty());
    }

    @Test
    @Ignore
    public void testRemoveAnnotation() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        onView(withId(R.id.mapView)).perform(new RemoveAnnotationAction(marker, mapboxMap));
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }

    @Test
    @Ignore
    public void testRemoveAnnotationById() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        mapboxMap.addMarker(markerOptions);
        // id will always be 0 in unit tests
        mapboxMap.removeAnnotation(0);
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }

    @Test
    @Ignore
    public void testRemoveAnnotations() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        mapboxMap.addMarkers(markerList);
        mapboxMap.removeAnnotations();
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }

    @Test
    @Ignore
    public void testClear() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        mapboxMap.addMarkers(markerList);
        mapboxMap.clear();
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }

    @Test
    @Ignore
    public void testRemoveAnnotationsByList() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        List<Marker> markers = mapboxMap.addMarkers(markerList);
        Marker marker = mapboxMap.addMarker(new MarkerOptions().position(new LatLng()).title("c"));
        mapboxMap.removeAnnotations(markers);
        assertTrue("Annotations should not be empty", mapboxMap.getAnnotations().size() == 1);
        assertTrue("Marker should be contained", mapboxMap.getAnnotations().contains(marker));
    }

    @Test
    @Ignore
    public void testGetAnnotationById() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker initialMarker = mapboxMap.addMarker(markerOptions);
        Marker retrievedMarker = (Marker) mapboxMap.getAnnotation(0);
        assertEquals("Markers should match", initialMarker, retrievedMarker);
    }

    @Test
    @Ignore
    public void testGetAnnotations() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("Annotations should be non null", mapboxMap.getAnnotations());
    }

    @Test
    @Ignore
    public void testGetMarkers() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("Markers should be non null", mapboxMap.getMarkers());
    }

    @Test
    @Ignore
    public void testGetPolygons() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("Polygons should be non null", mapboxMap.getPolygons());
    }

    @Test
    @Ignore
    public void testGetPolylines() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("Polylines should be non null", mapboxMap.getPolylines());
    }

    @Test
    @Ignore
    public void testGetSelectedMarkers() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        assertNotNull("Selected markers should be non null", mapboxMap.getSelectedMarkers());
    }

    @Test
    @Ignore
    public void testSelectMarker() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.selectMarker(marker);
        assertTrue("Marker should be contained", mapboxMap.getSelectedMarkers().contains(marker));
    }

    @Test
    @Ignore
    public void testDeselectMarker() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.selectMarker(marker);
        mapboxMap.deselectMarker(marker);
        assertTrue("Selected markers should be empty", mapboxMap.getSelectedMarkers().isEmpty());
    }

    @Test
    @Ignore
    public void testDeselectMarkers() {
        MapboxMap mapboxMap = activity.getMapboxMap();
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker1 = mapboxMap.addMarker(markerOptions);
        Marker marker2 = mapboxMap.addMarker(markerOptions);
        mapboxMap.selectMarker(marker1);
        mapboxMap.selectMarker(marker2);
        mapboxMap.deselectMarkers();
        assertTrue("Selected markers should be empty", mapboxMap.getSelectedMarkers().isEmpty());
    }

    @After
    public void afterTest() {
        Timber.e("@After test: unregister idle resource");
        Espresso.unregisterIdlingResources(idlingResource);
    }

    private class MinZoomAction implements ViewAction {

        private MapboxMap mapboxMap;

        MinZoomAction(MapboxMap map) {
            mapboxMap = map;
        }

        @Override
        public Matcher<View> getConstraints() {
            return isDisplayed();
        }

        @Override
        public String getDescription() {
            return getClass().getSimpleName();
        }

        @Override
        public void perform(UiController uiController, View view) {
            mapboxMap.setMinZoom(10);
        }
    }

    private class RemoveAnnotationAction implements ViewAction {

        private Annotation annotation;
        private MapboxMap mapboxMap;

        RemoveAnnotationAction(Annotation annotation, MapboxMap mapboxMap) {
            this.annotation = annotation;
            this.mapboxMap = mapboxMap;
        }

        @Override
        public Matcher<View> getConstraints() {
            return isDisplayed();
        }

        @Override
        public String getDescription() {
            return getClass().getSimpleName();
        }

        @Override
        public void perform(UiController uiController, View view) {
            mapboxMap.removeAnnotation(annotation);
        }
    }

}
