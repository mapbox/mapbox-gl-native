package com.mapbox.mapboxsdk.testapp.maps.widgets;

import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapViewUtils;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.activity.style.FillExtrusionActivity;

import org.hamcrest.Matcher;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.hamcrest.Matchers.not;

public class FillExtrusionTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return FillExtrusionActivity.class;
  }

  @Test
  public void testRotate() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new RotateAction(mapboxMap));
  }

  private class RotateAction implements ViewAction {

    private MapboxMap mapboxMap;

    RotateAction(MapboxMap map) {
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
      int i = 0;
      while(i < 360){
        MapViewUtils.setDirection(mapboxMap, i);
        i += 5;
        uiController.loopMainThreadForAtLeast(20);
      }
    }
  }
}