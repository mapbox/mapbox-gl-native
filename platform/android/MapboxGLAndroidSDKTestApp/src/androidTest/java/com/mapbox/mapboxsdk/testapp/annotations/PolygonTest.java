package com.mapbox.mapboxsdk.testapp.annotations;

import android.graphics.Color;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
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

public class PolygonTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  private Polygon polygon;

  @Test
  @Ignore
  /** native crash **/
  public void addPolygonTest() {
    validateTestSetup();
    LatLng latLngOne = new LatLng();
    LatLng latLngTwo = new LatLng(1, 0);
    LatLng latLngThree = new LatLng(1, 1);

    assertEquals("Polygons should be empty", 0, mapboxMap.getPolygons().size());

    final PolygonOptions options = new PolygonOptions();
    options.strokeColor(Color.BLUE);
    options.fillColor(Color.RED);
    options.add(latLngOne);
    options.add(latLngTwo);
    options.add(latLngThree);

    onView(withId(R.id.mapView)).perform(new AddPolygonAction(mapboxMap, options));

    assertEquals("Polygons should be 1", 1, mapboxMap.getPolygons().size());
    assertEquals("Polygon id should be 0", 0, polygon.getId());
    assertEquals("Polygon points size should match", 3, polygon.getPoints().size());
    assertEquals("Polygon stroke color should match", Color.BLUE, polygon.getStrokeColor());
    assertEquals("Polygon target should match", Color.RED, polygon.getFillColor());
    mapboxMap.clear();
    assertEquals("Polygons should be empty", 0, mapboxMap.getPolygons().size());
  }

  private class AddPolygonAction implements ViewAction {

    private MapboxMap mapboxMap;
    private PolygonOptions options;

    AddPolygonAction(MapboxMap map, PolygonOptions polygonOptions) {
      mapboxMap = map;
      options = polygonOptions;
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
      polygon = mapboxMap.addPolygon(options);
    }
  }
}
