package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.Color;
import android.graphics.drawable.Drawable;

import com.mapbox.mapboxsdk.maps.FocalPointChangeListener;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.TrackingSettings;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;

import java.util.Arrays;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MyLocationViewSettingsTest {

  @InjectMocks
  Projection projection = mock(Projection.class);

  @InjectMocks
  MyLocationView myLocationView = mock(MyLocationView.class);

  @InjectMocks
  TrackingSettings trackingSettings = mock(TrackingSettings.class);

  @InjectMocks
  FocalPointChangeListener focalPointChangeListener = mock(FocalPointChangeListener.class);

  private MyLocationViewSettings locationViewSettings;

  @Before
  public void beforeTest() {
    locationViewSettings = new MyLocationViewSettings(myLocationView, projection, focalPointChangeListener);
  }

  @Test
  public void testSanity() {
    assertNotNull("should not be null", locationViewSettings);
  }

  @Test
  public void testForegroundDrawables() {
    Drawable foregroundDrawable = mock(Drawable.class);
    Drawable foregroundBearingDrawable = mock(Drawable.class);
    Drawable.ConstantState constantState = mock(Drawable.ConstantState.class);
    when(foregroundDrawable.getConstantState()).thenReturn(constantState);
    when(constantState.newDrawable()).thenReturn(foregroundDrawable);
    locationViewSettings.setForegroundDrawable(foregroundDrawable, foregroundBearingDrawable);
    assertEquals("foreground should match", foregroundDrawable, locationViewSettings.getForegroundDrawable());
    assertEquals("foreground bearing should match", foregroundBearingDrawable,
      locationViewSettings.getForegroundBearingDrawable());
  }

  @Test
  public void testBackgroundDrawable() {
    Drawable backgroundDrawable = mock(Drawable.class);
    int[] offset = new int[] {1, 2, 3, 4};
    locationViewSettings.setBackgroundDrawable(backgroundDrawable, offset);
    assertEquals("foreground should match", backgroundDrawable, locationViewSettings.getBackgroundDrawable());
    assertTrue("offsets should match", Arrays.equals(offset, locationViewSettings.getBackgroundOffset()));
  }

  @Test
  public void testForegroundTint() {
    int color = Color.RED;
    locationViewSettings.setForegroundTintColor(Color.RED);
    assertEquals("color should match", color, locationViewSettings.getForegroundTintColor());
  }

  @Test
  public void testForegroundTransparentTint() {
    int color = Color.TRANSPARENT;
    locationViewSettings.setForegroundTintColor(Color.TRANSPARENT);
    assertEquals("color should match", color, locationViewSettings.getForegroundTintColor());
  }

  @Test
  public void testBackgroundTint() {
    int color = Color.RED;
    locationViewSettings.setBackgroundTintColor(Color.RED);
    assertEquals("color should match", color, locationViewSettings.getBackgroundTintColor());
  }

  @Test
  public void testBackgroundTransparentTint() {
    int color = Color.TRANSPARENT;
    locationViewSettings.setBackgroundTintColor(Color.TRANSPARENT);
    assertEquals("color should match", color, locationViewSettings.getBackgroundTintColor());
  }

  @Test
  public void testEnabled() {
    assertFalse("initial state should be false", locationViewSettings.isEnabled());
    locationViewSettings.setEnabled(true);
    assertTrue("state should be true", locationViewSettings.isEnabled());
  }
}

