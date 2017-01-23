package com.mapbox.mapboxsdk.maps;

import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

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
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

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
 * <p>
 * Methods executed on MapboxMap are called from a ViewAction to ensure correct synchronisation
 * with the application UI-thread.
 * </p>
 */
public class MapboxMapTest {

  @Rule
  public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;
  private EspressoTestActivity activity;

  @Before
  public void beforeTest() {
    activity = rule.getActivity();
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    idlingResource = new OnMapReadyIdlingResource(activity);
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  public void testSanity() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    MapboxMap mapboxMap = activity.getMapboxMap();
    assertNotNull("mapboxMap should not be null", mapboxMap);
  }

  //
  // MinZoomLevel
  //

  @Test
  public void testMinZoom() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.setMinZoomPreference(10);
        assertEquals("MinZoom should match", 10, mapboxMap.getMinZoomLevel(), 10);
      }
    }));
  }

  @Test
  public void testMaxZoom() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    final double zoom = 10;
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.setMaxZoomPreference(zoom);
        assertEquals("MaxZoom should match", zoom, mapboxMap.getMaxZoomLevel(), 10);
      }
    }));
  }

  @Test
  public void testInitialZoomLevels() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        assertEquals("MaxZoom should match", MapboxConstants.MAXIMUM_ZOOM, mapboxMap.getMaxZoomLevel(),
          TestConstants.ZOOM_DELTA);
        assertEquals("MinZoom should match", MapboxConstants.MINIMUM_ZOOM, mapboxMap.getMinZoomLevel(),
          TestConstants.ZOOM_DELTA);
      }
    }));
  }

  //
  // TrackingSettings
  //

  @Test
  public void testTrackingSettings() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    MapboxMap mapboxMap = activity.getMapboxMap();
    assertNotNull("TrackingSettings should not be null", mapboxMap.getTrackingSettings());
  }

  //
  // InfoWindow
  //

  @Test
  public void testConcurrentInfoWindowEnabled() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.setAllowConcurrentMultipleOpenInfoWindows(true);
        assertTrue("ConcurrentWindows should be true", mapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
      }
    }));
  }

  @Test
  public void testConcurrentInfoWindowDisabled() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.setAllowConcurrentMultipleOpenInfoWindows(false);
        assertFalse("ConcurrentWindows should be false", mapboxMap.isAllowConcurrentMultipleOpenInfoWindows());
      }
    }));
  }

  @Test
  public void testInfoWindowAdapter() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
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
    }));
  }

  //
  // Location
  //

  @Test
  @Ignore /* disabled due to enabling permissions during test #7177 */
  public void testMyLocationEnabled() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.setMyLocationEnabled(true);
        assertTrue("MyLocationEnabled should be true", mapboxMap.isMyLocationEnabled());
      }
    }));
  }

  @Test
  @Ignore /* can't create handler inside thread that not called Looper.prepare() */
  public void testMyLocationDisabled() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.setMyLocationEnabled(false);
        assertFalse("MyLocationEnabled should be false", mapboxMap.isMyLocationEnabled());
      }
    }));
  }

  //
  // setters/getters interfaces
  //

  @Test
  public void testFpsListener() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MapboxMap.OnFpsChangedListener fpsChangedListener = new MapboxMap.OnFpsChangedListener() {
          @Override
          public void onFpsChanged(double fps) {

          }
        };
        mapboxMap.setOnFpsChangedListener(fpsChangedListener);
        assertEquals("FpsListener should match", fpsChangedListener, mapboxMap.getOnFpsChangedListener());
      }
    }));
  }

  @Test
  public void testInfoWindowClickListener() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MapboxMap.OnInfoWindowClickListener clickListener = new MapboxMap.OnInfoWindowClickListener() {
          @Override
          public boolean onInfoWindowClick(@NonNull Marker marker) {
            return false;
          }
        };
        mapboxMap.setOnInfoWindowClickListener(clickListener);
        assertEquals("InfoWidowClickListener should match", clickListener, mapboxMap.getOnInfoWindowClickListener());
      }
    }));
  }

  @Test
  public void testInfoWindowCloseListener() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MapboxMap.OnInfoWindowCloseListener listener = new MapboxMap.OnInfoWindowCloseListener() {
          @Override
          public void onInfoWindowClose(Marker marker) {

          }
        };
        mapboxMap.setOnInfoWindowCloseListener(listener);
        assertEquals("InfoWindowCloseListener should match", listener, mapboxMap.getOnInfoWindowCloseListener());
      }
    }));
  }

  @Test
  public void testInfoWindowLongClickListener() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MapboxMap.OnInfoWindowLongClickListener listener = new MapboxMap.OnInfoWindowLongClickListener() {
          @Override
          public void onInfoWindowLongClick(Marker marker) {

          }
        };
        mapboxMap.setOnInfoWindowLongClickListener(listener);
        assertEquals("InfoWindowLongClickListener should match", listener,
          mapboxMap.getOnInfoWindowLongClickListener());
      }
    }));
  }

  //
  // Annotations
  //

  @Test
  public void testAddMarker() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        assertTrue("Marker should be contained", mapboxMap.getMarkers().contains(marker));
      }
    }));
  }

  @Test(expected = InvalidMarkerPositionException.class)
  public void testAddMarkerInvalidPosition() {
    new MarkerOptions().getMarker();
  }

  @Test
  public void testAddMarkers() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
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
    }));
  }

  @Test
  public void testAddMarkersEmpty() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        mapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 0", 0, mapboxMap.getMarkers().size());
      }
    }));
  }

  @Test
  public void testAddMarkersSingleMarker() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions = new MarkerOptions().title("a").position(new LatLng());
        markerList.add(markerOptions);
        List<Marker> markers = mapboxMap.addMarkers(markerList);
        assertEquals("Markers size should be 1", 1, mapboxMap.getMarkers().size());
        assertTrue(mapboxMap.getMarkers().contains(markers.get(0)));
      }
    }));
  }

  @Test
  public void testAddPolygon() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        PolygonOptions polygonOptions = new PolygonOptions().add(new LatLng());
        Polygon polygon = mapboxMap.addPolygon(polygonOptions);
        assertTrue("Polygon should be contained", mapboxMap.getPolygons().contains(polygon));
      }
    }));
  }

  @Test
  public void testAddEmptyPolygon() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        PolygonOptions polygonOptions = new PolygonOptions();
        Polygon polygon = mapboxMap.addPolygon(polygonOptions);
        assertTrue("Polygon should be ignored", !mapboxMap.getPolygons().contains(polygon));
      }
    }));
  }

  @Test
  public void testAddPolygons() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
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
    }));
  }

  @Test
  public void addPolygonsEmpty() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.addPolygons(new ArrayList<PolygonOptions>());
        assertEquals("Polygons size should be 0", 0, mapboxMap.getPolygons().size());
      }
    }));
  }

  @Test
  public void addPolygonsSingle() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        List<PolygonOptions> polygonList = new ArrayList<>();
        PolygonOptions polygonOptions = new PolygonOptions().fillColor(Color.BLACK).add(new LatLng());
        polygonList.add(polygonOptions);
        mapboxMap.addPolygons(polygonList);
        assertEquals("Polygons size should be 1", 1, mapboxMap.getPolygons().size());
        assertTrue(mapboxMap.getPolygons().contains(polygonOptions.getPolygon()));
      }
    }));
  }

  @Test
  public void testAddPolyline() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        PolylineOptions polylineOptions = new PolylineOptions().add(new LatLng());
        Polyline polyline = mapboxMap.addPolyline(polylineOptions);
        assertTrue("Polyline should be contained", mapboxMap.getPolylines().contains(polyline));
      }
    }));
  }

  @Test
  public void testAddEmptyPolyline() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        PolylineOptions polylineOptions = new PolylineOptions();
        Polyline polyline = mapboxMap.addPolyline(polylineOptions);
        assertTrue("Polyline should be ignored", !mapboxMap.getPolylines().contains(polyline));
      }
    }));
  }

  @Test
  public void testAddPolylines() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
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
    }));
  }

  @Test
  public void testAddPolylinesEmpty() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        mapboxMap.addPolylines(new ArrayList<PolylineOptions>());
        assertEquals("Polygons size should be 0", 0, mapboxMap.getPolylines().size());
      }
    }));
  }

  @Test
  public void testAddPolylinesSingle() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        List<PolylineOptions> polylineList = new ArrayList<>();
        PolylineOptions polygonOptions = new PolylineOptions().color(Color.BLACK).add(new LatLng());
        polylineList.add(polygonOptions);
        mapboxMap.addPolylines(polylineList);
        assertEquals("Polygons size should be 1", 1, mapboxMap.getPolylines().size());
        assertTrue(mapboxMap.getPolylines().contains(polygonOptions.getPolyline()));
      }
    }));
  }

  @Test
  public void testRemoveMarker() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.removeMarker(marker);
        assertTrue("Markers should be empty", mapboxMap.getMarkers().isEmpty());
      }
    }));
  }

  @Test
  public void testRemovePolygon() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        PolygonOptions polygonOptions = new PolygonOptions();
        Polygon polygon = mapboxMap.addPolygon(polygonOptions);
        mapboxMap.removePolygon(polygon);
        assertTrue("Polygons should be empty", mapboxMap.getPolylines().isEmpty());
      }
    }));
  }

  @Test
  public void testRemovePolyline() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        PolylineOptions polylineOptions = new PolylineOptions();
        Polyline polyline = mapboxMap.addPolyline(polylineOptions);
        mapboxMap.removePolyline(polyline);
        assertTrue("Polylines should be empty", mapboxMap.getPolylines().isEmpty());
      }
    }));
  }

  @Test
  public void testRemoveAnnotation() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.removeAnnotation(marker);
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
      }
    }));
  }

  @Test
  public void testRemoveAnnotationById() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        mapboxMap.addMarker(markerOptions);
        // id will always be 0 in unit tests
        mapboxMap.removeAnnotation(0);
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
      }
    }));
  }

  @Test
  public void testRemoveAnnotations() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        mapboxMap.addMarkers(markerList);
        mapboxMap.removeAnnotations();
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
      }
    }));
  }

  @Test
  public void testClear() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        List<BaseMarkerOptions> markerList = new ArrayList<>();
        MarkerOptions markerOptions1 = new MarkerOptions().title("a").position(new LatLng());
        MarkerOptions markerOptions2 = new MarkerOptions().title("b").position(new LatLng());
        markerList.add(markerOptions1);
        markerList.add(markerOptions2);
        mapboxMap.addMarkers(markerList);
        mapboxMap.clear();
        assertTrue("Annotations should be empty", mapboxMap.getAnnotations().isEmpty());
      }
    }));
  }

  @Test
  public void testRemoveAnnotationsByList() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
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
    }));
  }

  @Test
  public void testGetAnnotationById() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker initialMarker = mapboxMap.addMarker(markerOptions);
        Marker retrievedMarker = (Marker) mapboxMap.getAnnotation(0);
        assertEquals("Markers should match", initialMarker, retrievedMarker);
      }
    }));
  }

  @Test
  public void testGetAnnotations() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        assertNotNull("Annotations should be non null", mapboxMap.getAnnotations());
      }
    }));
  }

  @Test
  public void testGetMarkers() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        assertNotNull("Markers should be non null", mapboxMap.getMarkers());
      }
    }));
  }

  @Test
  public void testGetPolygons() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        assertNotNull("Polygons should be non null", mapboxMap.getPolygons());
      }
    }));
  }

  @Test
  public void testGetPolylines() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        assertNotNull("Polylines should be non null", mapboxMap.getPolylines());
      }
    }));
  }

  @Test
  public void testGetSelectedMarkers() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        assertNotNull("Selected markers should be non null", mapboxMap.getSelectedMarkers());
      }
    }));
  }

  @Test
  public void testSelectMarker() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.selectMarker(marker);
        assertTrue("Marker should be contained", mapboxMap.getSelectedMarkers().contains(marker));
      }
    }));
  }

  @Test
  public void testDeselectMarker() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker = mapboxMap.addMarker(markerOptions);
        mapboxMap.selectMarker(marker);
        mapboxMap.deselectMarker(marker);
        assertTrue("Selected markers should be empty", mapboxMap.getSelectedMarkers().isEmpty());
      }
    }));
  }

  @Test
  public void testDeselectMarkers() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = activity.getMapboxMap();
    onView(withId(R.id.mapView)).perform(new MapboxMapAction(new InvokeViewAction() {
      @Override
      public void onViewAction(UiController uiController, View view) {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng());
        Marker marker1 = mapboxMap.addMarker(markerOptions);
        Marker marker2 = mapboxMap.addMarker(markerOptions);
        mapboxMap.selectMarker(marker1);
        mapboxMap.selectMarker(marker2);
        mapboxMap.deselectMarkers();
        assertTrue("Selected markers should be empty", mapboxMap.getSelectedMarkers().isEmpty());
      }
    }));
  }

  @After
  public void afterTest() {
    Timber.e("@After test: unregister idle resource");
    Espresso.unregisterIdlingResources(idlingResource);
  }

  private class MapboxMapAction implements ViewAction {

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
