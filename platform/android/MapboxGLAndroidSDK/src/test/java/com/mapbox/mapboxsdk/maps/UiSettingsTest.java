package com.mapbox.mapboxsdk.maps;

import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.maps.widgets.CompassView;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class UiSettingsTest {

  @InjectMocks
  Projection projection = mock(Projection.class);

  @InjectMocks
  FocalPointChangeListener focalPointChangeListener = mock(FocalPointChangeListener.class);

  @InjectMocks
  CompassView compassView = mock(CompassView.class);

  @InjectMocks
  ImageView imageView = mock(ImageView.class);

  @InjectMocks
  ImageView logoView = mock(ImageView.class);

  @InjectMocks
  FrameLayout.LayoutParams layoutParams = mock(FrameLayout.LayoutParams.class);

  private UiSettings uiSettings;

  @Before
  public void beforeTest() {
    uiSettings = new UiSettings(projection, focalPointChangeListener, compassView, imageView, logoView, 1);
  }

  @Test
  public void testSanity() {
    assertNotNull("uiSettings should not be null", uiSettings);
  }

  @Test
  public void testCompassEnabled() {
    when(compassView.isEnabled()).thenReturn(true);
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
    when(compassView.getLayoutParams()).thenReturn(layoutParams);
    layoutParams.gravity = Gravity.START;
    uiSettings.setCompassGravity(Gravity.START);
    assertEquals("Compass gravity should be same", Gravity.START, uiSettings.getCompassGravity());
  }

  @Test
  public void testCompassMargins() {
    when(projection.getContentPadding()).thenReturn(new int[] {0, 0, 0, 0});
    when(compassView.getLayoutParams()).thenReturn(layoutParams);
    layoutParams.leftMargin = 1;
    layoutParams.topMargin = 2;
    layoutParams.rightMargin = 3;
    layoutParams.bottomMargin = 4;
    uiSettings.setCompassMargins(1, 2, 3, 4);
    assertTrue("Compass margin left should be same", uiSettings.getCompassMarginLeft() == 1);
    assertTrue("Compass margin top should be same", uiSettings.getCompassMarginTop() == 2);
    assertTrue("Compass margin right should be same", uiSettings.getCompassMarginRight() == 3);
    assertTrue("Compass margin bottom should be same", uiSettings.getCompassMarginBottom() == 4);
  }

  @Test
  public void testCompassFadeWhenFacingNorth() {
    when(compassView.isFadeCompassViewFacingNorth()).thenReturn(true);
    assertTrue("Compass should fade when facing north by default.", uiSettings.isCompassFadeWhenFacingNorth());
    uiSettings.setCompassFadeFacingNorth(false);
    when(compassView.isFadeCompassViewFacingNorth()).thenReturn(false);
    assertFalse("Compass fading should be disabled", uiSettings.isCompassFadeWhenFacingNorth());
  }

  @Test
  public void testLogoEnabled() {
    uiSettings.setLogoEnabled(true);
    assertEquals("Logo should be enabled", true, uiSettings.isLogoEnabled());
  }

  @Test
  public void testLogoDisabled() {
    when(logoView.getVisibility()).thenReturn(View.GONE);
    uiSettings.setLogoEnabled(false);
    assertEquals("Logo should be disabled", false, uiSettings.isLogoEnabled());
  }

  @Test
  public void testLogoGravity() {
    layoutParams.gravity = Gravity.END;
    when(logoView.getLayoutParams()).thenReturn(layoutParams);
    uiSettings.setLogoGravity(Gravity.END);
    assertEquals("Logo gravity should be same", Gravity.END, uiSettings.getLogoGravity());
  }

  @Test
  public void testLogoMargins() {
    when(projection.getContentPadding()).thenReturn(new int[] {0, 0, 0, 0});
    when(logoView.getLayoutParams()).thenReturn(layoutParams);
    layoutParams.leftMargin = 1;
    layoutParams.topMargin = 2;
    layoutParams.rightMargin = 3;
    layoutParams.bottomMargin = 4;
    uiSettings.setLogoMargins(1, 2, 3, 4);
    assertTrue("Compass margin left should be same", uiSettings.getLogoMarginLeft() == 1);
    assertTrue("Compass margin top should be same", uiSettings.getLogoMarginTop() == 2);
    assertTrue("Compass margin right should be same", uiSettings.getLogoMarginRight() == 3);
    assertTrue("Compass margin bottom should be same", uiSettings.getLogoMarginBottom() == 4);
  }

  @Test
  public void testAttributionEnabled() {
    when(imageView.getVisibility()).thenReturn(View.VISIBLE);
    uiSettings.setAttributionEnabled(true);
    assertEquals("Attribution should be enabled", true, uiSettings.isAttributionEnabled());
  }

  @Test
  public void testAttributionDisabled() {
    when(imageView.getVisibility()).thenReturn(View.GONE);
    uiSettings.setAttributionEnabled(false);
    assertEquals("Attribution should be disabled", false, uiSettings.isAttributionEnabled());
  }

  @Test
  public void testAttributionGravity() {
    when(imageView.getLayoutParams()).thenReturn(layoutParams);
    layoutParams.gravity = Gravity.END;
    uiSettings.setAttributionGravity(Gravity.END);
    assertEquals("Attribution gravity should be same", Gravity.END, uiSettings.getAttributionGravity());
  }

  @Test
  public void testAttributionMargins() {
    when(imageView.getLayoutParams()).thenReturn(layoutParams);
    when(projection.getContentPadding()).thenReturn(new int[] {0, 0, 0, 0});
    layoutParams.leftMargin = 1;
    layoutParams.topMargin = 2;
    layoutParams.rightMargin = 3;
    layoutParams.bottomMargin = 4;
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
  public void testQuickZoomGesturesEnabled() {
    uiSettings.setQuickZoomGesturesEnabled(true);
    assertEquals("QuickZoom gesture should be enabled", true, uiSettings.isQuickZoomGesturesEnabled());
  }

  @Test
  public void testQuickZoomGesturesDisabled() {
    uiSettings.setQuickZoomGesturesEnabled(false);
    assertEquals("QuickZoom gesture should be disabled", false, uiSettings.isQuickZoomGesturesEnabled());
  }

  @Test
  public void testQuickZoomGestureChangeAllowed() {
    uiSettings.setQuickZoomGesturesEnabled(false);
    assertEquals("QuickZoom gesture should be false", false, uiSettings.isQuickZoomGesturesEnabled());
    uiSettings.setQuickZoomGesturesEnabled(true);
    assertEquals("QuickZoom gesture should be true", true, uiSettings.isQuickZoomGesturesEnabled());
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
  public void testDisableRotateWhenScalingEnabled() {
    uiSettings.setDisableRotateWhenScaling(true);
    assertEquals("Rotate disabling should be enabled", true,
      uiSettings.isDisableRotateWhenScaling());
  }

  @Test
  public void testDisableRotateWhenScalingDisabled() {
    uiSettings.setDisableRotateWhenScaling(false);
    assertEquals("Rotate disabling should be disabled", false,
      uiSettings.isDisableRotateWhenScaling());
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

  @Test
  public void testAreAllGesturesEnabled() {
    uiSettings.setAllGesturesEnabled(true);
    assertEquals("All gestures check should return true", true,
        uiSettings.areAllGesturesEnabled());
  }

  @Test
  public void testAreAllGesturesEnabledWithOneGestureDisabled() {
    uiSettings.setAllGesturesEnabled(true);
    uiSettings.setScrollGesturesEnabled(false);
    assertEquals("All gestures check should return false", false,
        uiSettings.areAllGesturesEnabled());
  }

  @Test
  public void testZoomRateDefaultValue() {
    assertEquals("Default zoom rate should be 1.0f", 1.0f,
      uiSettings.getZoomRate(), 0);
  }

  @Test
  public void testZoomRate() {
    uiSettings.setZoomRate(0.83f);
    assertEquals("Zoom rate should be 0.83f", 0.83f,
      uiSettings.getZoomRate(), 0);
  }
}
