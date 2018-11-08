package com.mapbox.mapboxsdk.testapp.utils;

import androidx.annotation.IdRes;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.assertion.ViewAssertions.matches;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

public class ViewUtils {

  public static void clickView(@IdRes int viewRes) {
    onView(withId(viewRes))
      .perform(click());
  }

  public static void checkViewIsDisplayed(int id) {
    onView(withId(id))
      .check(matches(isDisplayed()));
  }
}
