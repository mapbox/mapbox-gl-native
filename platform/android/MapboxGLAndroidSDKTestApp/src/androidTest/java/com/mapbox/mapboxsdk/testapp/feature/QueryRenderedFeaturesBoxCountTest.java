package com.mapbox.mapboxsdk.testapp.feature;

import android.support.test.espresso.Espresso;
import android.support.test.rule.ActivityTestRule;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.feature.QueryRenderedFeaturesBoxCountActivity;
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
 * Instrumentation test to validate if clicking on the blue rectangle from
 * QueryRenderedFeaturesBoxSymbolCountActivity shows a Toast that 149 features were found.
 */
public class QueryRenderedFeaturesBoxCountTest {

  @Rule
  public final ActivityTestRule<QueryRenderedFeaturesBoxCountActivity> rule =
    new ActivityTestRule<>(QueryRenderedFeaturesBoxCountActivity.class);

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
    onView(withText("149 features in box"))
      .inRoot(withDecorView(not(is(rule.getActivity().getWindow().getDecorView()))))
      .check(matches(isDisplayed()));
  }

  @After
  public void unregisterIdlingResource() {
    Espresso.unregisterIdlingResources(idlingResource);
  }

}