package com.mapbox.mapboxsdk.perf;

import android.Manifest;
import android.support.test.rule.GrantPermissionRule;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import org.junit.Rule;
import org.junit.Test;

import static com.mapbox.mapboxsdk.testapp.action.MapboxMapAction.invoke;

public class AnimationTest extends BasePerfTest {

  private static final int ANIMATION_DURATION = 5000;

  @Rule
  public GrantPermissionRule permissionRule =
    GrantPermissionRule.grant(Manifest.permission.WRITE_EXTERNAL_STORAGE);

  @Test
  public void testAnimation() {
    validateTestSetup();
    invoke(mapboxMap, (uiController, mapboxMap) -> {
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
        new LatLng(60.168577, 24.938001), 12), ANIMATION_DURATION);
      uiController.loopMainThreadForAtLeast(ANIMATION_DURATION);
    });
  }

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }
}
