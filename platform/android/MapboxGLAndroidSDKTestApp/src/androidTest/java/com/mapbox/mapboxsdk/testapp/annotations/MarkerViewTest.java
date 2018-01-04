package com.mapbox.mapboxsdk.testapp.annotations;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.annotation.MarkerViewActivity;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.model.annotations.TextMarkerViewOptions;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;

import org.junit.Ignore;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;
import static org.junit.Assert.assertEquals;

public class MarkerViewTest extends BaseActivityTest {

  private Marker marker;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  @Ignore
  public void addMarkerViewTest() {
    validateTestSetup();
    addAdapter();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      assertEquals("Markers should be empty", 0, mapboxMap.getMarkers().size());

      TextMarkerViewOptions options = new TextMarkerViewOptions();
      options.text(TestConstants.TEXT_MARKER_TEXT);
      options.position(new LatLng());
      options.snippet(TestConstants.TEXT_MARKER_SNIPPET);
      options.title(TestConstants.TEXT_MARKER_TITLE);
      marker = mapboxMap.addMarker(options);
      assertEquals("Markers size should be 1, ", 1, mapboxMap.getMarkers().size());
      assertEquals("Marker id should be 0", 0, marker.getId());
      assertEquals("Marker target should match", new LatLng(), marker.getPosition());
      assertEquals("Marker snippet should match", TestConstants.TEXT_MARKER_SNIPPET, marker.getSnippet());
      assertEquals("Marker target should match", TestConstants.TEXT_MARKER_TITLE, marker.getTitle());
      uiController.loopMainThreadForAtLeast(500);
    });
    onView(withText(TestConstants.TEXT_MARKER_TEXT)).check(matches(isDisplayed()));
  }

  @Test
  @Ignore
  public void showInfoWindowTest() {
    validateTestSetup();
    addAdapter();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      final TextMarkerViewOptions options = new TextMarkerViewOptions();
      options.position(new LatLng());
      options.text(TestConstants.TEXT_MARKER_TEXT);
      options.snippet(TestConstants.TEXT_MARKER_SNIPPET);
      options.title(TestConstants.TEXT_MARKER_TITLE);
      marker = mapboxMap.addMarker(options);
      uiController.loopMainThreadForAtLeast(500);
      mapboxMap.selectMarker(marker);
    });
    onView(withText(TestConstants.TEXT_MARKER_TEXT)).check(matches(isDisplayed()));
    onView(withText(TestConstants.TEXT_MARKER_TITLE)).check(matches(isDisplayed()));
    onView(withText(TestConstants.TEXT_MARKER_SNIPPET)).check(matches(isDisplayed()));
  }

  private void addAdapter() {
    invoke(mapboxMap, (uiController, mapboxMap) -> mapboxMap.getMarkerViewManager().addMarkerViewAdapter(
      new MarkerViewActivity.TextAdapter(rule.getActivity(), mapboxMap)));
  }

}
