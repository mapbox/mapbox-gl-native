package com.mapbox.mapboxsdk.testapp.utils;

import androidx.annotation.IdRes;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.doubleClick;
import static androidx.test.espresso.matcher.ViewMatchers.withId;

public class GestureUtils {

  public static void doubleClickGesture(@IdRes int id) {
    onView(withId(id)).perform(doubleClick());
  }

}
