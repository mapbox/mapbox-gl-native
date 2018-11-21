package com.mapbox.mapboxsdk.testapp.action;

import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import org.hamcrest.Matcher;

import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;

public final class WaitAction implements ViewAction {

  private static final long DEFAULT_LOOP_TIME = 375;
  private final long loopTime;

  public WaitAction() {
    this(DEFAULT_LOOP_TIME);
  }

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
}

