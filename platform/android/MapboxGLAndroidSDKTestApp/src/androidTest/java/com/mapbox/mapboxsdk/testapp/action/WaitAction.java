package com.mapbox.mapboxsdk.testapp.action;

import androidx.test.espresso.UiController;
import androidx.test.espresso.ViewAction;
import android.view.View;
import org.hamcrest.Matcher;

import static androidx.test.espresso.Espresso.onView;
import static androidx.test.espresso.matcher.ViewMatchers.isDisplayed;
import static androidx.test.espresso.matcher.ViewMatchers.isRoot;

public final class WaitAction implements ViewAction {

  private final long loopTime;

  public WaitAction(long loopTime) {
    this.loopTime = loopTime;
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
    uiController.loopMainThreadForAtLeast(loopTime);
  }

  public static void invoke(long loopTime) {
    onView(isRoot()).perform(new WaitAction(loopTime));
  }
}

