package com.mapbox.mapboxsdk.maps;

import android.arch.core.executor.testing.InstantTaskExecutorRule;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.view.Gravity;

import com.mapbox.mapboxsdk.R;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(JUnit4.class)
public class UiSettingsTest {

  @Rule
  public InstantTaskExecutorRule instantExecutorRule = new InstantTaskExecutorRule();

  @InjectMocks
  Projection projection = mock(Projection.class);

  @InjectMocks
  Context context = mock(Context.class);

  @InjectMocks
  Resources resources = mock(Resources.class);

  @InjectMocks
  PackageManager packageManager = mock(PackageManager.class);

  private UiSettings uiSettings;

  @Before
  public void beforeTest() {
    when(context.getResources()).thenReturn(resources);
    when(resources.getDimension(R.dimen.mapbox_four_dp)).thenReturn(25f);
    when(context.getPackageManager()).thenReturn(packageManager);
    when(packageManager.hasSystemFeature(PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)).thenReturn(true);

    MapboxMapOptions options = new MapboxMapOptions();
    uiSettings = new UiSettings();
    uiSettings.initialiseProjection(projection);
    uiSettings.initialiseGestures(options);
    uiSettings.initialiseCompass(options, resources);
    uiSettings.initialiseLogo(options, resources);
    uiSettings.initialiseAttribution(context, options);
    uiSettings.initialiseZoomControl(context);
  }

  @Test
  public void testSanity() {
    assertNotNull("uiSettings should not be null", uiSettings);
  }

  @Test
  public void testCompassEnabled() {
    uiSettings.setCompassEnabled(true);
    assertEquals("Compass should be enabled", true, uiSettings.isCompassEnabled());
  }

  @Test
  public void testCompassDisabled() {
    uiSettings.setCompassEnabled(false);
    assertEquals("Compass should be disabled", false, uiSettings.isCompassEnabled());
  }

  @Test
  public void testCompassGravity() {
    uiSettings.setCompassGravity(Gravity.START);
    assertEquals("Compass gravity should be same", Gravity.START, uiSettings.getCompassGravity());
  }

  @Test
  public void testCompassMargins() {
    uiSettings.setCompassMargins(1, 2, 3, 4);
    assertTrue("Compass margin left should be same", uiSettings.getCompassMarginLeft() == 1);
    assertTrue("Compass margin top should be same", uiSettings.getCompassMarginTop() == 2);
    assertTrue("Compass margin right should be same", uiSettings.getCompassMarginRight() == 3);
    assertTrue("Compass margin bottom should be same", uiSettings.getCompassMarginBottom() == 4);
  }

  @Test
  public void testCompassFadeWhenFacingNorth() {
    assertTrue("Compass should fade when facing north by default.", uiSettings.isCompassFadeWhenFacingNorth());
    uiSettings.setCompassFadeFacingNorth(false);
    assertFalse("Compass fading should be disabled", uiSettings.isCompassFadeWhenFacingNorth());
  }

  @Test
  public void testLogoEnabled() {
    uiSettings.setLogoEnabled(true);
    assertEquals("Logo should be enabled", true, uiSettings.isLogoEnabled());
  }

  @Test
  public void testLogoDisabled() {
    uiSettings.setLogoEnabled(false);
    assertEquals("Logo should be disabled", false, uiSettings.isLogoEnabled());
  }

  @Test
  public void testLogoGravity() {
    uiSettings.setLogoGravity(Gravity.END);
    assertEquals("Logo gravity should be same", Gravity.END, uiSettings.getLogoGravity());
  }

  @Test
  public void testLogoMargins() {
    uiSettings.setLogoMargins(1, 2, 3, 4);
    assertTrue("Compass margin left should be same", uiSettings.getLogoMarginLeft() == 1);
    assertTrue("Compass margin top should be same", uiSettings.getLogoMarginTop() == 2);
    assertTrue("Compass margin right should be same", uiSettings.getLogoMarginRight() == 3);
    assertTrue("Compass margin bottom should be same", uiSettings.getLogoMarginBottom() == 4);
  }

  @Test
  public void testAttributionEnabled() {
    uiSettings.setAttributionEnabled(true);
    assertEquals("Attribution should be enabled", true, uiSettings.isAttributionEnabled());
  }

  @Test
  public void testAttributionDisabled() {
    uiSettings.setAttributionEnabled(false);
    assertEquals("Attribution should be disabled", false, uiSettings.isAttributionEnabled());
  }

  @Test
  public void testAttributionGravity() {
    uiSettings.setAttributionGravity(Gravity.END);
    assertEquals("Attribution gravity should be same", Gravity.END, uiSettings.getAttributionGravity());
  }

  @Test
  public void testAttributionMargins() {
    uiSettings.setAttributionMargins(1, 2, 3, 4);
    assertTrue("Attribution margin left should be same", uiSettings.getAttributionMarginLeft() == 1);
    assertTrue("Attribution margin top should be same", uiSettings.getAttributionMarginTop() == 2);
    assertTrue("Attribution margin right should be same", uiSettings.getAttributionMarginRight() == 3);
    assertTrue("Attribution margin bottom should be same", uiSettings.getAttributionMarginBottom() == 4);
  }

  @Test
  public void testRotateGesturesEnabled() {
    uiSettings.setRotateGesturesEnabled(true);
    assertEquals("Rotate gesture should be enabled", true, uiSettings.isRotateGesturesEnabled());
  }

  @Test
  public void testRotateGesturesDisabled() {
    uiSettings.setRotateGesturesEnabled(false);
    assertEquals("Rotate gesture should be disabled", false, uiSettings.isRotateGesturesEnabled());
  }

  @Test
  public void testRotateGestureChangeAllowed() {
    uiSettings.setRotateGesturesEnabled(false);
    assertEquals("Rotate gesture should be false", false, uiSettings.isRotateGesturesEnabled());
    uiSettings.setRotateGesturesEnabled(true);
    assertEquals("Rotate gesture should be true", true, uiSettings.isRotateGesturesEnabled());
  }

  @Test
  public void testTiltGesturesEnabled() {
    uiSettings.setTiltGesturesEnabled(true);
    assertEquals("Tilt gesture should be enabled", true, uiSettings.isTiltGesturesEnabled());
  }

  @Test
  public void testTiltGesturesDisabled() {
    uiSettings.setTiltGesturesEnabled(false);
    assertEquals("Tilt gesture should be disabled", false, uiSettings.isTiltGesturesEnabled());
  }

  @Test
  public void testTiltGestureChangeAllowed() {
    uiSettings.setTiltGesturesEnabled(false);
    assertEquals("Tilt gesture should be false", false, uiSettings.isTiltGesturesEnabled());
    uiSettings.setTiltGesturesEnabled(true);
    assertEquals("Tilt gesture should be true", true, uiSettings.isTiltGesturesEnabled());
  }

  @Test
  public void testZoomGesturesEnabled() {
    uiSettings.setZoomGesturesEnabled(true);
    assertEquals("Zoom gesture should be enabled", true, uiSettings.isZoomGesturesEnabled());
  }

  @Test
  public void testZoomGesturesDisabled() {
    uiSettings.setZoomGesturesEnabled(false);
    assertEquals("Zoom gesture should be disabled", false, uiSettings.isZoomGesturesEnabled());
  }

  @Test
  public void testZoomGestureChangeAllowed() {
    uiSettings.setZoomGesturesEnabled(false);
    assertEquals("Zoom gesture should be false", false, uiSettings.isZoomGesturesEnabled());
    uiSettings.setZoomGesturesEnabled(true);
    assertEquals("Zoom gesture should be true", true, uiSettings.isZoomGesturesEnabled());
  }

  @Test
  public void testZoomControlsEnabled() {
    uiSettings.setZoomControlsEnabled(true);
    assertEquals("Zoom controls should be enabled", true, uiSettings.isZoomControlsEnabled());
  }

  @Test
  public void testZoomControlsDisabled() {
    uiSettings.setZoomControlsEnabled(false);
    assertEquals("Zoom controls should be disabled", false, uiSettings.isZoomControlsEnabled());
  }

  @Test
  public void testDoubleTapGesturesEnabled() {
    uiSettings.setDoubleTapGesturesEnabled(true);
    assertEquals("DoubleTap gesture should be enabled", true, uiSettings.isDoubleTapGesturesEnabled());
  }

  @Test
  public void testDoubleTapGesturesDisabled() {
    uiSettings.setDoubleTapGesturesEnabled(false);
    assertEquals("DoubleTap gesture should be disabled", false, uiSettings.isDoubleTapGesturesEnabled());
  }

  @Test
  public void testDoubleTapGestureChangeAllowed() {
    uiSettings.setDoubleTapGesturesEnabled(false);
    assertEquals("DoubleTap gesture should be false", false, uiSettings.isDoubleTapGesturesEnabled());
    uiSettings.setDoubleTapGesturesEnabled(true);
    assertEquals("DoubleTap gesture should be true", true, uiSettings.isDoubleTapGesturesEnabled());
  }

  @Test
  public void testScrollGesturesEnabled() {
    uiSettings.setScrollGesturesEnabled(true);
    assertEquals("Scroll gesture should be enabled", true, uiSettings.isScrollGesturesEnabled());
  }

  @Test
  public void testScrollGesturesDisabled() {
    uiSettings.setScrollGesturesEnabled(false);
    assertEquals("Scroll gesture should be disabled", false, uiSettings.isScrollGesturesEnabled());
  }

  @Test
  public void testScrollGestureChangeAllowed() {
    uiSettings.setScrollGesturesEnabled(false);
    assertEquals("Scroll gesture should be false", false, uiSettings.isScrollGesturesEnabled());
    uiSettings.setScrollGesturesEnabled(true);
    assertEquals("Scroll gesture should be true", true, uiSettings.isScrollGesturesEnabled());
  }

  @Test
  public void testScaleVelocityAnimationEnabled() {
    uiSettings.setScaleVelocityAnimationEnabled(true);
    assertEquals("Scale velocity animation should be enabled", true, uiSettings.isScaleVelocityAnimationEnabled());
  }

  @Test
  public void testScaleVelocityAnimationDisabled() {
    uiSettings.setScaleVelocityAnimationEnabled(false);
    assertEquals("Scale velocity animation should be disabled", false, uiSettings.isScaleVelocityAnimationEnabled());
  }

  @Test
  public void testRotateVelocityAnimationEnabled() {
    uiSettings.setRotateVelocityAnimationEnabled(true);
    assertEquals("Rotate velocity animation should be enabled", true, uiSettings.isRotateVelocityAnimationEnabled());
  }

  @Test
  public void testRotateVelocityAnimationDisabled() {
    uiSettings.setRotateVelocityAnimationEnabled(false);
    assertEquals("Rotate velocity animation should be disabled", false, uiSettings.isRotateVelocityAnimationEnabled());
  }

  @Test
  public void testFlingVelocityAnimationEnabled() {
    uiSettings.setFlingVelocityAnimationEnabled(true);
    assertEquals("Fling velocity animation should be enabled", true, uiSettings.isFlingVelocityAnimationEnabled());
  }

  @Test
  public void testFlingVelocityAnimationDisabled() {
    uiSettings.setFlingVelocityAnimationEnabled(false);
    assertEquals("Fling velocity animation should be disabled", false, uiSettings.isFlingVelocityAnimationEnabled());
  }

  @Test
  public void testAllVelocityAnimationsEnabled() {
    uiSettings.setAllVelocityAnimationsEnabled(true);
    assertEquals("Scale velocity animation should be enabled", true, uiSettings.isScaleVelocityAnimationEnabled());
    assertEquals("Rotate velocity animation should be enabled", true, uiSettings.isRotateVelocityAnimationEnabled());
    assertEquals("Fling velocity animation should be enabled", true, uiSettings.isFlingVelocityAnimationEnabled());
  }

  @Test
  public void testAllVelocityAnimationsDisabled() {
    uiSettings.setAllVelocityAnimationsEnabled(false);
    assertEquals("Scale velocity animation should be disabled", false, uiSettings.isScaleVelocityAnimationEnabled());
    assertEquals("Rotate velocity animation should be disabled", false, uiSettings.isRotateVelocityAnimationEnabled());
    assertEquals("Fling velocity animation should be disabled", false, uiSettings.isFlingVelocityAnimationEnabled());
  }

  @Test
  public void testIncreaseRotateThresholdWhenScalingEnabled() {
    uiSettings.setIncreaseRotateThresholdWhenScaling(true);
    assertEquals("Rotate threshold increase should be enabled", true,
      uiSettings.isIncreaseRotateThresholdWhenScaling());
  }

  @Test
  public void testIncreaseRotateThresholdWhenScalingDisabled() {
    uiSettings.setIncreaseRotateThresholdWhenScaling(false);
    assertEquals("Rotate threshold increase should be disabled", false,
      uiSettings.isIncreaseRotateThresholdWhenScaling());
  }

  @Test
  public void testIncreaseScaleThresholdWhenRotatingEnabled() {
    uiSettings.setIncreaseScaleThresholdWhenRotating(true);
    assertEquals("Scale threshold increase should be enabled", true, uiSettings.isIncreaseScaleThresholdWhenRotating());
  }

  @Test
  public void testIncreaseScaleThresholdWhenRotatingDisabled() {
    uiSettings.setIncreaseScaleThresholdWhenRotating(false);
    assertEquals("Scale threshold increase should be disabled", false,
      uiSettings.isIncreaseScaleThresholdWhenRotating());
  }

  @Test
  public void testAllGesturesEnabled() {
    uiSettings.setAllGesturesEnabled(true);
    assertEquals("Rotate gesture should be enabled", true, uiSettings.isRotateGesturesEnabled());
    assertEquals("Tilt gesture should be enabled", true, uiSettings.isTiltGesturesEnabled());
    assertEquals("Zoom gesture should be enabled", true, uiSettings.isZoomGesturesEnabled());
    assertEquals("Scroll gesture should be enabled", true, uiSettings.isScrollGesturesEnabled());
  }

  @Test
  public void testAllGesturesDisabled() {
    uiSettings.setAllGesturesEnabled(false);
    assertEquals("Rotate gesture should be enabled", false, uiSettings.isRotateGesturesEnabled());
    assertEquals("Tilt gesture should be disabled", false, uiSettings.isTiltGesturesEnabled());
    assertEquals("Zoom gesture should be disabled", false, uiSettings.isZoomGesturesEnabled());
    assertEquals("Scroll gesture should be disabled", false, uiSettings.isScrollGesturesEnabled());
  }
}
