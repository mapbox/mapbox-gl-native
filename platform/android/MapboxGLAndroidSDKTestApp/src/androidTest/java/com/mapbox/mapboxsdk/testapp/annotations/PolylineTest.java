package com.mapbox.mapboxsdk.testapp.annotations;

import android.graphics.Color;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

import org.hamcrest.Matcher;
import org.junit.Ignore;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertEquals;

public class PolylineTest extends BaseActivityTest {

  private Polyline polyline;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Ignore
  @Test
  public void addPolylineTest() {
    validateTestSetup();
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
}
