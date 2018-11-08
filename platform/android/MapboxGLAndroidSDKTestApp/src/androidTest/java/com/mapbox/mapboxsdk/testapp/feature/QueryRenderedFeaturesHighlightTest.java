package com.mapbox.mapboxsdk.testapp.feature;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.feature.QueryRenderedFeaturesBoxHighlightActivity;

import org.junit.Ignore;
import org.junit.Test;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.RootMatchers.withDecorView;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;
import static androidx.test.espresso.matcher.ViewMatchers.withText;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.not;

/**
 * Instrumentation test to validate if clicking box on screen highlights features.
 */
public class QueryRenderedFeaturesHighlightTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return QueryRenderedFeaturesBoxHighlightActivity.class;
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

}
