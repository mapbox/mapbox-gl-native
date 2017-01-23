package com.mapbox.mapboxsdk.testapp.annotations;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.annotation.MarkerViewActivity;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.model.annotations.TextMarkerViewOptions;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;
import com.mapbox.mapboxsdk.testapp.utils.ViewUtils;

import org.hamcrest.Matcher;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.junit.Assert.assertEquals;

public class MarkerViewTest {

  @Rule
  public final ActivityTestRule<EspressoTestActivity> rule = new ActivityTestRule<>(EspressoTestActivity.class);

  private OnMapReadyIdlingResource idlingResource;
  private Marker marker;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  @Ignore
  public void addMarkerViewTest() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
    assertEquals("Markers should be empty", 0, mapboxMap.getMarkers().size());

    TextMarkerViewOptions options = new TextMarkerViewOptions();
    options.text(TestConstants.TEXT_MARKER_TEXT);
    options.position(new LatLng());
    options.snippet(TestConstants.TEXT_MARKER_SNIPPET);
    options.title(TestConstants.TEXT_MARKER_TITLE);

    onView(withId(R.id.mapView)).perform(new AddTextMarkerViewAction(mapboxMap, options));
    assertEquals("Markers sze should be 1, ", 1, mapboxMap.getMarkers().size());
    assertEquals("Marker id should be 0", 0, marker.getId());
    assertEquals("Marker target should match", new LatLng(), marker.getPosition());
    assertEquals("Marker snippet should match", TestConstants.TEXT_MARKER_SNIPPET, marker.getSnippet());
    assertEquals("Marker target should match", TestConstants.TEXT_MARKER_TITLE, marker.getTitle());
    onView(withText(TestConstants.TEXT_MARKER_TEXT)).check(matches(isDisplayed()));
  }

  @Test
  @Ignore
  public void showInfoWindowTest() {
    ViewUtils.checkViewIsDisplayed(R.id.mapView);
    MapboxMap mapboxMap = rule.getActivity().getMapboxMap();

    final TextMarkerViewOptions options = new TextMarkerViewOptions();
    options.position(new LatLng());
    options.text(TestConstants.TEXT_MARKER_TEXT);
    options.snippet(TestConstants.TEXT_MARKER_SNIPPET);
    options.title(TestConstants.TEXT_MARKER_TITLE);

    onView(withId(R.id.mapView)).perform(new AddTextMarkerViewAction(mapboxMap, options));
    onView(withText(TestConstants.TEXT_MARKER_TEXT)).check(matches(isDisplayed()));
    onView(withId(R.id.mapView)).perform(new ShowInfoWindowAction(mapboxMap));
    onView(withText(TestConstants.TEXT_MARKER_TITLE)).check(matches(isDisplayed()));
    onView(withText(TestConstants.TEXT_MARKER_SNIPPET)).check(matches(isDisplayed()));
  }

  private class AddTextMarkerViewAction implements ViewAction {

    private MapboxMap mapboxMap;
    private TextMarkerViewOptions options;

    AddTextMarkerViewAction(MapboxMap map, TextMarkerViewOptions markerOptions) {
      mapboxMap = map;
      options = markerOptions;
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
      mapboxMap.getMarkerViewManager().addMarkerViewAdapter(
        new MarkerViewActivity.TextAdapter(view.getContext(), mapboxMap));
      marker = mapboxMap.addMarker(options);
      uiController.loopMainThreadForAtLeast(250);
    }
  }

  private class ShowInfoWindowAction implements ViewAction {

    private MapboxMap mapboxMap;

    ShowInfoWindowAction(MapboxMap map) {
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
      mapboxMap.selectMarker(marker);
      uiController.loopMainThreadForAtLeast(250);
    }
  }

  @After
  public void unregisterIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }
}
