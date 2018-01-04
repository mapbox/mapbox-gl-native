package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.camera.CameraAnimationTypeActivity;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isRoot;
import static com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationLandscape;
import static com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationLandscapeReverse;
import static com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationPortrait;
import static com.mapbox.mapboxsdk.testapp.action.OrientationChangeAction.orientationPortraitReverse;

public class OrientationTest extends BaseActivityTest {

  @Test
  public void testChangeDeviceOrientation() {
    onView(isRoot()).perform(orientationLandscape());
    waitLoop(2200);
    onView(isRoot()).perform(orientationPortrait());
    waitLoop(2500);
    onView(isRoot()).perform(orientationLandscapeReverse());
    waitLoop(500);
    onView(isRoot()).perform(orientationPortraitReverse());
    waitLoop(1250);
    onView(isRoot()).perform(orientationLandscape());
    waitLoop(750);
    onView(isRoot()).perform(orientationPortrait());
    waitLoop(950);
    onView(isRoot()).perform(orientationLandscapeReverse());
    onView(isRoot()).perform(orientationPortraitReverse());
    onView(isRoot()).perform(orientationLandscape());
    onView(isRoot()).perform(orientationPortrait());
  }

  @Override
  protected Class getActivityClass() {
    return CameraAnimationTypeActivity.class;
  }

}
