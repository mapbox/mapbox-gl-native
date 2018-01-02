package com.mapbox.mapboxsdk.testapp.action;


import android.app.Activity;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.pm.ActivityInfo;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;
import android.view.ViewGroup;
import org.hamcrest.Matcher;

import static android.support.test.espresso.matcher.ViewMatchers.isRoot;

public class OrientationChangeAction implements ViewAction {

  private final int orientation;

  private OrientationChangeAction(int orientation) {
    this.orientation = orientation;
  }

  public static ViewAction orientationLandscape() {
    return new OrientationChangeAction(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
  }

  public static ViewAction orientationPortrait() {
    return new OrientationChangeAction(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
  }

  public static ViewAction orientationLandscapeReverse() {
    return new OrientationChangeAction(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
  }

  public static ViewAction orientationPortraitReverse() {
    return new OrientationChangeAction(ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT);
  }

  @Override
  public Matcher<View> getConstraints() {
    return isRoot();
  }

  @Override
  public String getDescription() {
    return "change orientation to " + orientation;
  }

  @Override
  public void perform(UiController uiController, View view) {
    uiController.loopMainThreadUntilIdle();
    Activity activity = getActivity(view.getContext());
    if (activity == null && view instanceof ViewGroup) {
      ViewGroup v = (ViewGroup) view;
      int c = v.getChildCount();
      for (int i = 0; i < c && activity == null; ++i) {
        activity = getActivity(v.getChildAt(i).getContext());
      }
    }
    activity.setRequestedOrientation(orientation);
  }

  private Activity getActivity(Context context) {
    while (context instanceof ContextWrapper) {
      if (context instanceof Activity) {
        return (Activity) context;
      }
      context = ((ContextWrapper) context).getBaseContext();
    }
    return null;
  }

}
