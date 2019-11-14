package com.mapbox.mapboxsdk.maps;

import android.graphics.Color;
import androidx.test.espresso.UiController;
import androidx.test.espresso.ViewAction;
import android.view.View;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.exceptions.InvalidMarkerPositionException;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest;
import org.hamcrest.Matcher;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static junit.framework.TestCase.assertFalse;
import static junit.framework.TestCase.assertNotNull;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

/**
 * This test is responsible for testing the public API.
 * <p>
 * Methods executed on MapboxMap are called from a ViewAction to ensure correct synchronisation
 * with the application UI-thread.
 * </p>
 * @deprecated remove this file when removing deprecated annotations
 */
@Deprecated
public class MapboxMapTest extends EspressoTest {

  //
  // InfoWindow
  //

  @Test
  public void testConcurrentInfoWindowEnabled() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      mapboxMap.setAllowConcurrentMultipleOpenInfoWindows(true);
      assertTrue("ConcurrentWindows should be true", mapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
    }));
  }

  @Test
  public void testConcurrentInfoWindowDisabled() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      mapboxMap.setAllowConcurrentMultipleOpenInfoWindows(false);
      assertFalse("ConcurrentWindows should be false", mapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
    }));
  }

  @Test
  public void testInfoWindowAdapter() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MapboxMap.InfoWindowAdapter infoWindowAdapter = marker -> null;
      mapboxMap.setInfoWindowAdapter(infoWindowAdapter);
      assertEquals("InfoWindowAdpter should be the same", infoWindowAdapter, mapboxMap.getInfoWindowAdapter());
    }));
  }

  //
  // Annotations
  //

  @Test
  public void testAddMarker() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker marker = mapboxMap.addMarker(markerOptions);
      assertTrue("Marker should be contained", mapboxMap.getMarkers().contains(marker));
    }));
  }

  @Test(expected = InvalidMarkerPositionException.class)
  public void testAddMarkerInvalidPosition() {
    new MarkerOptions().getMarker();
  }

  @Test
  public void testAddMarkers() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<BaseMarkerOptions> markerList = new ArrayList<>();
      MarkerOptions markerOptions1 = new MarkerOptions().position(new LatLng()).title("a");
      MarkerOptions markerOptions2 = new MarkerOptions().position(new LatLng()).title("b");
      markerList.add(markerOptions1);
      markerList.add(markerOptions2);
      List<Marker> markers = mapboxMap.addMarkers(markerList);
      assertEquals("Markers size should be 2", 2, mapboxMap.getMarkers().size());
      assertTrue(mapboxMap.getMarkers().contains(markers.get(0)));
      assertTrue(mapboxMap.getMarkers().contains(markers.get(1)));
    }));
  }

  @Test
  public void testAddMarkersEmpty() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<BaseMarkerOptions> markerList = new ArrayList<>();
      mapboxMap.addMarkers(markerList);
      assertEquals("Markers size should be 0", 0, mapboxMap.getMarkers().size());
    }));
  }

  @Test
  public void testAddMarkersSingleMarker() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<BaseMarkerOptions> markerList = new ArrayList<>();
      MarkerOptions markerOptions = new MarkerOptions().title("a").position(new LatLng());
      markerList.add(markerOptions);
      List<Marker> markers = mapboxMap.addMarkers(markerList);
      assertEquals("Markers size should be 1", 1, mapboxMap.getMarkers().size());
      assertTrue(mapboxMap.getMarkers().contains(markers.get(0)));
    }));
  }

  @Test
  public void testAddPolygon() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      PolygonOptions polygonOptions = new PolygonOptions().add(new LatLng());
      Polygon polygon = mapboxMap.addPolygon(polygonOptions);
      assertTrue("Polygon should be contained", mapboxMap.getPolygons().contains(polygon));
    }));
  }

  @Test
  public void testAddEmptyPolygon() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      PolygonOptions polygonOptions = new PolygonOptions();
      Polygon polygon = mapboxMap.addPolygon(polygonOptions);
      assertTrue("Polygon should be ignored", !mapboxMap.getPolygons().contains(polygon));
    }));
  }

  @Test
  public void testAddPolygons() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
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
    }));
  }

  @Test
  public void addPolygonsEmpty() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      mapboxMap.addPolygons(new ArrayList<PolygonOptions>());
      assertEquals("Polygons size should be 0", 0, mapboxMap.getPolygons().size());
    }));
  }

  @Test
  public void addPolygonsSingle() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<PolygonOptions> polygonList = new ArrayList<>();
      PolygonOptions polygonOptions = new PolygonOptions().fillColor(Color.BLACK).add(new LatLng());
      polygonList.add(polygonOptions);
      mapboxMap.addPolygons(polygonList);
      assertEquals("Polygons size should be 1", 1, mapboxMap.getPolygons().size());
      assertTrue(mapboxMap.getPolygons().contains(polygonOptions.getPolygon()));
    }));
  }

  @Test
  public void testAddPolyline() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      PolylineOptions polylineOptions = new PolylineOptions().add(new LatLng());
      Polyline polyline = mapboxMap.addPolyline(polylineOptions);
      assertTrue("Polyline should be contained", mapboxMap.getPolylines().contains(polyline));
    }));
  }

  @Test
  public void testAddEmptyPolyline() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      PolylineOptions polylineOptions = new PolylineOptions();
      Polyline polyline = mapboxMap.addPolyline(polylineOptions);
      assertTrue("Polyline should be ignored", !mapboxMap.getPolylines().contains(polyline));
    }));
  }

  @Test
  public void testAddPolylines() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
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
      assertTrue(
        "Polyline should be ignored", !mapboxMap.getPolylines().contains(polygonOptions3.getPolyline())
      );
    }));
  }

  @Test
  public void testAddPolylinesEmpty() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      mapboxMap.addPolylines(new ArrayList<PolylineOptions>());
      assertEquals("Polygons size should be 0", 0, mapboxMap.getPolylines().size());
    }));
  }

  @Test
  public void testAddPolylinesSingle() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<PolylineOptions> polylineList = new ArrayList<>();
      PolylineOptions polygonOptions = new PolylineOptions().color(Color.BLACK).add(new LatLng());
      polylineList.add(polygonOptions);
      mapboxMap.addPolylines(polylineList);
      assertEquals("Polygons size should be 1", 1, mapboxMap.getPolylines().size());
      assertTrue(mapboxMap.getPolylines().contains(polygonOptions.getPolyline()));
    }));
  }

  @Test
  public void testRemoveMarker() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker marker = mapboxMap.addMarker(markerOptions);
      mapboxMap.removeMarker(marker);
      assertTrue("Markers should be empty", mapboxMap.getMarkers().isEmpty());
    }));
  }

  @Test
  public void testRemovePolygon() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      PolygonOptions polygonOptions = new PolygonOptions();
      Polygon polygon = mapboxMap.addPolygon(polygonOptions);
      mapboxMap.removePolygon(polygon);
      assertTrue("Polygons should be empty", mapboxMap.getPolylines().isEmpty());
    }));
  }

  @Test
  public void testRemovePolyline() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      PolylineOptions polylineOptions = new PolylineOptions();
      Polyline polyline = mapboxMap.addPolyline(polylineOptions);
      mapboxMap.removePolyline(polyline);
      assertTrue("Polylines should be empty", mapboxMap.getPolylines().isEmpty());
    }));
  }

  @Test
  public void testRemoveAnnotation() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker marker = mapboxMap.addMarker(markerOptions);
      mapboxMap.removeAnnotation(marker);
      assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }));
  }

  @Test
  public void testRemoveAnnotationById() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      mapboxMap.addMarker(markerOptions);
      // id will always be 0 in unit tests
      mapboxMap.removeAnnotation(0);
      assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }));
  }

  @Test
  public void testRemoveAnnotations() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<BaseMarkerOptions> markerList = new ArrayList<>();
      MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
      MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
      markerList.add(markerOptions1);
      markerList.add(markerOptions2);
      mapboxMap.addMarkers(markerList);
      mapboxMap.removeAnnotations();
      assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }));
  }

  @Test
  public void testClear() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      List<BaseMarkerOptions> markerList = new ArrayList<>();
      MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
      MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
      markerList.add(markerOptions1);
      markerList.add(markerOptions2);
      mapboxMap.addMarkers(markerList);
      mapboxMap.clear();
      assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
    }));
  }

  @Test
  public void testRemoveAnnotationsByList() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
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
    }));
  }

  @Test
  public void testGetAnnotationById() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker initialMarker = mapboxMap.addMarker(markerOptions);
      Marker retrievedMarker = (Marker) mapboxMap.getAnnotation(0);
      assertEquals("Markers should match", initialMarker, retrievedMarker);
    }));
  }

  @Test
  public void testGetAnnotations() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(
      new MapboxMapAction((uiController, view) ->
        assertNotNull("Annotations should be non null", mapboxMap.getAnnotations()))
    );
  }

  @Test
  public void testGetMarkers() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(
      new MapboxMapAction((uiController, view) ->
        assertNotNull("Markers should be non null", mapboxMap.getMarkers()))
    );
  }

  @Test
  public void testGetPolygons() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) ->
      assertNotNull("Polygons should be non null", mapboxMap.getPolygons()))
    );
  }

  @Test
  public void testGetPolylines() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) ->
      assertNotNull("Polylines should be non null", mapboxMap.getPolylines()))
    );
  }

  @Test
  public void testGetSelectedMarkers() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) ->
      assertNotNull("Selected markers should be non null", mapboxMap.getSelectedMarkers()))
    );
  }

  @Test
  public void testSelectMarker() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker marker = mapboxMap.addMarker(markerOptions);
      mapboxMap.selectMarker(marker);
      assertTrue("Marker should be contained", mapboxMap.getSelectedMarkers().contains(marker));
    }));
  }

  @Test
  public void testDeselectMarker() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker marker = mapboxMap.addMarker(markerOptions);
      mapboxMap.selectMarker(marker);
      mapboxMap.deselectMarker(marker);
      assertTrue("Selected markers should be empty", mapboxMap.getSelectedMarkers().isEmpty());
    }));
  }

  @Test
  public void testDeselectMarkers() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction((uiController, view) -> {
      MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
      Marker marker1 = mapboxMap.addMarker(markerOptions);
      Marker marker2 = mapboxMap.addMarker(markerOptions);
      mapboxMap.selectMarker(marker1);
      mapboxMap.selectMarker(marker2);
      mapboxMap.deselectMarkers();
      assertTrue("Selected markers should be empty", mapboxMap.getSelectedMarkers().isEmpty());
    }));
  }

  public class MapboxMapAction implements ViewAction {

    private InvokeViewAction invokeViewAction;

    MapboxMapAction(InvokeViewAction invokeViewAction) {
      this.invokeViewAction = invokeViewAction;
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
      invokeViewAction.onViewAction(uiController, view);
    }
  }

  interface InvokeViewAction {
    void onViewAction(UiController uiController, View view);
  }
}