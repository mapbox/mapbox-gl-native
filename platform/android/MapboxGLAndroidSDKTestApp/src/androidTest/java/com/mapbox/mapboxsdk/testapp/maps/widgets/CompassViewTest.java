package com.mapbox.mapboxsdk.testapp.maps.widgets;

import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testapp.utils.TestConstants;

import org.hamcrest.Matcher;
import org.junit.Ignore;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.action.ViewActions.click;
import static android.support.test.espresso.assertion.ViewAssertions.matches;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.hamcrest.Matchers.not;
import static org.junit.Assert.assertEquals;

public class CompassViewTest extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  public void testDefault() {
    validateTestSetup();
    onView(withId(R.id.compassView)).check(matches(not(isDisplayed())));
  }

  @Test
  public void testVisible() {
    validateTestSetup();

    onView(withId(R.id.mapView)).perform(new MoveCameraAction(mapboxMap,
        CameraUpdateFactory.newCameraPosition(
          new CameraPosition.Builder()
            .bearing(45)
            .zoom(1)
            .target(new LatLng())
            .build()
        )
      )
    );
    waitLoop();
    onView(withId(R.id.compassView)).check(matches(isDisplayed()));
  }

  @Test
  @Ignore // 10-31-2016 click action is not working
  public void testClick() {
    validateTestSetup();

    onView(withId(R.id.mapView)).perform(new MoveCameraAction(mapboxMap,
        CameraUpdateFactory.newCameraPosition(
          new CameraPosition.Builder()
            .bearing(45)
            .zoom(1)
            .target(new LatLng())
            .build()
        )
      )
    );

    waitLoop();
    onView(withId(R.id.compassView)).perform(click());
    onView(withId(R.id.mapView)).perform(new WaitAction(3000));
    onView(withId(R.id.compassView)).check(matches(not(isDisplayed())));

    CameraPosition cameraPosition = mapboxMap.getCameraPosition();
    assertEquals("Camera bearing should face north, ", 0, cameraPosition.bearing, TestConstants.BEARING_DELTA);
  }

  private class WaitAction implements ViewAction {

    private long waitTime;

    WaitAction(long waitTime) {
      this.waitTime = waitTime;
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
      uiController.loopMainThreadForAtLeast(waitTime);
    }
  }

  private class MoveCameraAction implements ViewAction {

    private MapboxMap mapboxMap;
    private CameraUpdate cameraUpdate;

    MoveCameraAction(MapboxMap map, CameraUpdate update) {
      mapboxMap = map;
      cameraUpdate = update;
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
      mapboxMap.moveCamera(cameraUpdate);
    }
  }
}

