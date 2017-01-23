package com.mapbox.mapboxsdk.testapp.camera;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapViewUtils;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.assertEquals;

/**
 * Tests camera transformations that aren't part of our public API
 */
public class CameraInternalApiTest {

  @Rule
  public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  public void testBearing() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    EspressoTestActivity activity = rule.getActivity();
    MapboxMap mapboxMap = activity.getMapboxMap();

    CameraPosition initialPosition = new
      CameraPosition.Builder().target(new LatLng()).zoom(1).bearing(0).tilt(0).build();
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Default camera position should match default", cameraPosition, initialPosition);

    onView(withId(R.id.mapView)).perform(new BearingAction(mapboxMap));
    assertEquals("Bearing should match", 45.1f, MapViewUtils.getDirection(mapboxMap), TestConstants.BEARING_DELTA);
  }

  @Test
  public void testTilt() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    EspressoTestActivity activity = rule.getActivity();
    MapboxMap mapboxMap = activity.getMapboxMap();

    CameraPosition initialPosition = new CameraPosition.Builder().target(
      new LatLng()).zoom(1).bearing(0).tilt(0).build();
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Default camera position should match default", cameraPosition, initialPosition);

    onView(withId(R.id.mapView)).perform(new TiltAction(mapboxMap));
    assertEquals("Tilt should match", 40.0f, MapViewUtils.getTilt(mapboxMap), TestConstants.TILT_DELTA);
  }

  @Test
  public void testLatLng() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    EspressoTestActivity activity = rule.getActivity();
    MapboxMap mapboxMap = activity.getMapboxMap();

    CameraPosition initialPosition = new CameraPosition.Builder().target(
      new LatLng()).zoom(1).bearing(0).tilt(0).build();
    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Default camera position should match default", cameraPosition, initialPosition);

    onView(withId(R.id.mapView)).perform(new LatLngAction(mapboxMap));
    LatLng centerCoordinate = MapViewUtils.getLatLng(mapboxMap);
    assertEquals("Latitude should match", 1.1f, centerCoordinate.getLatitude(), TestConstants.LAT_LNG_DELTA);
    assertEquals("Longitude should match", 2.2f, centerCoordinate.getLongitude(), TestConstants.LAT_LNG_DELTA);
  }

  @After
  public void unregisterIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }

  private class BearingAction implements ViewAction {

    private MapboxMap mapboxMap;

    BearingAction(MapboxMap mapboxMap) {
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
      MapViewUtils.setDirection(mapboxMap, -45.1f);
    }
  }

  private class TiltAction implements ViewAction {

    private MapboxMap mapboxMap;

    TiltAction(MapboxMap mapboxMap) {
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
      MapViewUtils.setTilt(mapboxMap, 40.0f);
    }
  }

  private class LatLngAction implements ViewAction {

    private MapboxMap mapboxMap;

    LatLngAction(MapboxMap mapboxMap) {
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
      MapViewUtils.setLatLng(mapboxMap, new LatLng(1.1, 2.2));
    }
  }
}