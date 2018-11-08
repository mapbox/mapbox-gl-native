package com.mapbox.mapboxsdk.testapp.utils;

import androidx.annotation.StringRes;
import androidx.test.espresso.Espresso;
import androidx.test.espresso.action.ViewActions;
import androidx.test.espresso.matcher.ViewMatchers;

import com.mapbox.mapboxsdk.testapp.R;

import org.hamcrest.Matchers;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.action.ViewActions.click;
import static androidx.test.espresso.matcher.ViewMatchers.withContentDescription;

public class DrawerUtils {

  private static final String HOME_BUTTON_STRING = "Navigate up";

  public static void openDrawer() {
    onView(withContentDescription(HOME_BUTTON_STRING)).perform(click());
  }

  public static void clickItem(@StringRes int txtId) {
    Espresso.onView(Matchers.allOf(ViewMatchers.withId(R.id.design_menu_item_text),
      ViewMatchers.hasSibling(ViewMatchers.withText(txtId)))).perform(ViewActions.click());
  }

}
