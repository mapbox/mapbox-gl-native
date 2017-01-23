package com.mapbox.mapboxsdk.testapp.feature;

import android.graphics.PointF;
import android.support.test.espresso.Espresso;
import android.support.test.espresso.ViewAction;
import android.support.test.espresso.action.CoordinatesProvider;
import android.support.test.espresso.action.GeneralClickAction;
import android.support.test.espresso.action.Press;
import android.support.test.espresso.action.Tap;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.feature.QueryRenderedFeaturesPropertiesActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

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

/**
 * Instrumentation test to validate if clicking center of screen returns the correct features.
 */
public class QueryRenderedFeaturesPropertiesTest {

  @Rule
  public final ActivityTestRule<QueryRenderedFeaturesPropertiesActivity> rule =
    new ActivityTestRule<>(QueryRenderedFeaturesPropertiesActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  @Ignore
  public void testCountFeatures() {
    MapboxMap mapboxMap = rule.getActivity().getMapboxMap();
    LatLng centerScreen = mapboxMap.getCameraPosition().target;
    PointF centerPixel = mapboxMap.getProjection().toScreenLocation(centerScreen);
    onView(withId(R.id.mapView)).perform(clickXY(centerPixel.x, centerPixel.y));
    onView(withText("Found 4 features")).check(matches(isDisplayed()));
  }

  @After
  public void unregisterIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }

  private static ViewAction clickXY(final float x, final float y) {
    return new GeneralClickAction(
      Tap.SINGLE,
      new CoordinatesProvider() {
        @Override
        public float[] calculateCoordinates(View view) {
          final int[] screenPos = new int[2];
          view.getLocationOnScreen(screenPos);
          final float screenX = screenPos[0] + x;
          final float screenY = screenPos[1] + y;
          return new float[] {screenX, screenY};
        }
      },
      Press.FINGER);
  }
}