package com.mapbox.mapboxsdk.testapp.utils;

import android.support.annotation.IdRes;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;

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
