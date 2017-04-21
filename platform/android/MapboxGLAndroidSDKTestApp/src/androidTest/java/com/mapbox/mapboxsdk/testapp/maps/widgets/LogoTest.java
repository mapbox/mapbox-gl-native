package com.mapbox.mapboxsdk.testapp.maps.widgets;

import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;

import org.hamcrest.Matcher;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.hamcrest.Matchers.not;

public class LogoTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  public void testDefault() {
    validateTestSetup();
    onView(withId(R.id.logoView)).check(matches(isDisplayed()));
  }

  @Test
  public void testDisabled() {
    validateTestSetup();

    onView(withId(R.id.logoView))
      .perform(new DisableAction(mapboxMap))
      .check(matches(not(isDisplayed())));
  }

  private class DisableAction implements ViewAction {

    private MapboxMap mapboxMap;

    DisableAction(MapboxMap map) {
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
      mapboxMap.getUiSettings().setLogoEnabled(false);
    }
  }
}
