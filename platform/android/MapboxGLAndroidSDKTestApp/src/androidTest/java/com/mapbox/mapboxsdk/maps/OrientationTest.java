package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.testapp.action.OrientationAction;
import com.mapbox.mapboxsdk.testapp.action.WaitAction;
import com.mapbox.mapboxsdk.testapp.activity.BaseTest;
import com.mapbox.mapboxsdk.testapp.activity.camera.CameraAnimationTypeActivity;
import org.junit.Test;

import static com.mapbox.mapboxsdk.testapp.action.OrientationAction.orientationLandscape;
import static com.mapbox.mapboxsdk.testapp.action.OrientationAction.orientationLandscapeReverse;
import static com.mapbox.mapboxsdk.testapp.action.OrientationAction.orientationPortrait;
import static com.mapbox.mapboxsdk.testapp.action.OrientationAction.orientationPortraitReverse;

public class OrientationTest extends BaseTest {

  @Test
  public void testChangeDeviceOrientation() {
    OrientationAction.invoke(orientationLandscape());
    WaitAction.invoke(2200);
    OrientationAction.invoke(orientationPortrait());
    WaitAction.invoke(2500);
    OrientationAction.invoke(orientationLandscapeReverse());
    WaitAction.invoke(500);
    OrientationAction.invoke(orientationPortraitReverse());
    WaitAction.invoke(1250);
    OrientationAction.invoke(orientationLandscape());
    WaitAction.invoke(750);
    OrientationAction.invoke(orientationPortrait());
    WaitAction.invoke(950);
    OrientationAction.invoke(orientationLandscapeReverse());
    OrientationAction.invoke(orientationPortraitReverse());
    OrientationAction.invoke(orientationLandscape());
    OrientationAction.invoke(orientationPortrait());
  }

  @Override
  protected Class getActivityClass() {
    return CameraAnimationTypeActivity.class;
  }

}
