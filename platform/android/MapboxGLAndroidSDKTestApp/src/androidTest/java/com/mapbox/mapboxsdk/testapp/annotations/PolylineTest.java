package com.mapbox.mapboxsdk.testapp.annotations;

import android.graphics.Color;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertEquals;

public class PolylineTest {

  @Rule
  public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;
  private Polyline polyline;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Ignore
  @Test
  public void addPolylineTest() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    final MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
    LatLng latLngOne = new LatLng();
    LatLng latLngTwo = new LatLng(1, 0);

    assertEquals("Polygons should be empty", 0, mapboxMap.getPolygons().size());

    final PolylineOptions options = new PolylineOptions();
    options.color(Color.BLUE);
    options.add(latLngOne);
    options.add(latLngTwo);

    onView(withId(R.id.mapView)).perform(new AddPolyLineAction(mapboxMap, options));

    assertEquals("Polylines should be 1", 1, mapboxMap.getPolylines().size());
    assertEquals("Polyline id should be 0", 0, polyline.getId());
    assertEquals("Polyline points size should match", 2, polyline.getPoints().size());
    assertEquals("Polyline stroke color should match", Color.BLUE, polyline.getColor());
    mapboxMap.clear();
    assertEquals("Polyline should be empty", 0, mapboxMap.getPolylines().size());
  }

  private class AddPolyLineAction implements ViewAction {

    private MapboxMap mapboxMap;
    private PolylineOptions options;

    AddPolyLineAction(MapboxMap map, PolylineOptions polylineOptions) {
      mapboxMap = map;
      options = polylineOptions;
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
      polyline = mapboxMap.addPolyline(options);
    }
  }

  @After
  public void unregisterIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
