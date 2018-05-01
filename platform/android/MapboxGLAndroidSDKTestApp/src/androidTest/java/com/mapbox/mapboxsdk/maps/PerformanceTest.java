package com.mapbox.mapboxsdk.maps;

import android.Manifest;
import android.support.test.espresso.UiController;
import android.support.test.rule.GrantPermissionRule;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdate;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import com.mapbox.mapboxsdk.testrule.EnableLogcatDump;
import com.mapbox.mapboxsdk.testrule.EnablePostTestDumpsys;

import org.junit.Rule;
import org.junit.Test;

import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;

public class PerformanceTest extends BaseActivityTest {

  private static final long ANIMATION_DELAY = 250;

  private final CameraAnimation[] cameraAnimations = new CameraAnimation[3];
  private int currentIndex = 0;

  private UiController uiController;

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

//  @Rule
//  public EnableTestTracing enableTestTracing = new EnableTestTracing();

  //  @Rule
//  public EnablePerTestTraceFile enablePerTestTraceFile = new EnablePerTestTraceFile();
  @Rule
  public GrantPermissionRule readPermissionRule = GrantPermissionRule.grant(Manifest.permission.READ_EXTERNAL_STORAGE);

  @Rule
  public GrantPermissionRule writePermissionRule = GrantPermissionRule.grant(Manifest.permission.WRITE_EXTERNAL_STORAGE);

  @Rule
  public GrantPermissionRule dumpPermissionRule = GrantPermissionRule.grant(Manifest.permission.DUMP);

  @Rule
  public GrantPermissionRule logPermissionRule = GrantPermissionRule.grant(Manifest.permission.READ_LOGS);

  @Rule
  public GrantPermissionRule usagePermissionRule = GrantPermissionRule.grant(Manifest.permission.PACKAGE_USAGE_STATS);

  @Rule
  public EnablePostTestDumpsys enablePostTestDumpsys = new EnablePostTestDumpsys();

  @Rule
  public EnableLogcatDump enableLogcatDump = new EnableLogcatDump();

  @Test
  public void runPerformanceTest() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      this.uiController = uiController;
      fillCameraPositions();
      startAnimations();
    });
  }

  private void fillCameraPositions() {
    cameraAnimations[0] = new CameraAnimation(
      CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder()
        .target(new LatLng(37.789992, -122.402214))
        .zoom(3)
        .build()), 2000);

    cameraAnimations[1] = new CameraAnimation(
      CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder()
        .target(new LatLng(37.789992, -122.402214))
        .tilt(60)
        .zoom(17f)
        .bearing(135)
        .build()), 6000);

    cameraAnimations[2] = new CameraAnimation(
      CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder()
        .target(new LatLng(37.755659, -122.445668))
        .tilt(0)
        .zoom(11f)
        .bearing(0)
        .build()), 1000);
  }

  private void startAnimations() {
    for (CameraAnimation cameraAnimation : cameraAnimations) {
      uiController.loopMainThreadForAtLeast(ANIMATION_DELAY);
      mapboxMap.animateCamera(cameraAnimation.cameraUpdate, (int) cameraAnimation.animationDuration);
      uiController.loopMainThreadForAtLeast(cameraAnimation.animationDuration);
    }
    uiController.loopMainThreadForAtLeast(ANIMATION_DELAY);
  }

  private static class CameraAnimation {
    private CameraUpdate cameraUpdate;
    private long animationDuration;

    public CameraAnimation(CameraUpdate cameraUpdate, long animationDuration) {
      this.cameraUpdate = cameraUpdate;
      this.animationDuration = animationDuration;
    }
  }
}