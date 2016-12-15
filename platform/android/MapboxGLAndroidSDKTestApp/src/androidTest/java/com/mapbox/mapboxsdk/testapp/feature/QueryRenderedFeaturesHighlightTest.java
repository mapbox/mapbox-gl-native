package com.mapbox.mapboxsdk.testapp.feature;

import android.support.test.espresso.Espresso;
import android.support.test.espresso.ViewAction;
import android.support.test.espresso.action.CoordinatesProvider;
import android.support.test.espresso.action.GeneralClickAction;
import android.support.test.espresso.action.Press;
import android.support.test.espresso.action.Tap;
import android.support.test.rule.ActivityTestRule;
import android.view.View;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.feature.QueryRenderedFeaturesBoxHighlightActivity;
import com.mapbox.mapboxsdk.testapp.utils.OnMapReadyIdlingResource;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.RootMatchers.withDecorView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static android.support.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.not;

/**
 * Instrumentation test to validate if clicking box on screen highlights features.
 */
public class QueryRenderedFeaturesHighlightTest {

  @Rule
  public final ActivityTestRule<QueryRenderedFeaturesBoxHighlightActivity> rule =
    new ActivityTestRule<>(QueryRenderedFeaturesBoxHighlightActivity.class);

  private OnMapReadyIdlingResource idlingResource;

  @Before
  public void registerIdlingResource() {
    idlingResource = new OnMapReadyIdlingResource(rule.getActivity());
    Espresso.registerIdlingResources(idlingResource);
  }

  @Test
  @Ignore
  public void testCountFeatures() {
    // click on box to query map
    onView(withId(R.id.selection_box)).perform(click());

    // validate if toast is shown
    onView(withText("50 features in box"))
      .inRoot(withDecorView(not(is(rule.getActivity().getWindow().getDecorView()))))
      .check(matches(isDisplayed()));
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