package com.mapbox.mapboxsdk.maps;

import android.graphics.Color;
import android.view.Gravity;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import java.util.Arrays;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNull;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class MapboxMapOptionsTest {

  private static final double DELTA = 1e-15;

  @Test
  public void testSanity() {
    assertNotNull("should not be null", new MapboxMapOptions());
  }

  @Test
  public void testDebugEnabled() {
    assertFalse(new MapboxMapOptions().getDebugActive());
    assertTrue(new MapboxMapOptions().debugActive(true).getDebugActive());
    assertFalse(new MapboxMapOptions().debugActive(false).getDebugActive());
  }

  @Test
  public void testCompassEnabled() {
    assertTrue(new MapboxMapOptions().compassEnabled(true).getCompassEnabled());
    assertFalse(new MapboxMapOptions().compassEnabled(false).getCompassEnabled());
  }

  @Test
  public void testCompassGravity() {
    assertEquals(Gravity.TOP | Gravity.END, new MapboxMapOptions().getCompassGravity());
    assertEquals(Gravity.BOTTOM, new MapboxMapOptions().compassGravity(Gravity.BOTTOM).getCompassGravity());
    assertNotEquals(Gravity.START, new MapboxMapOptions().compassGravity(Gravity.BOTTOM).getCompassGravity());
  }

  @Test
  public void testCompassMargins() {
    assertTrue(Arrays.equals(new int[] {0, 1, 2, 3}, new MapboxMapOptions().compassMargins(
      new int[] {0, 1, 2, 3}).getCompassMargins()));
    assertFalse(Arrays.equals(new int[] {0, 1, 2, 3}, new MapboxMapOptions().compassMargins(
      new int[] {0, 0, 0, 0}).getCompassMargins()));
  }

  @Test
  public void testLogoEnabled() {
    assertTrue(new MapboxMapOptions().logoEnabled(true).getLogoEnabled());
    assertFalse(new MapboxMapOptions().logoEnabled(false).getLogoEnabled());
  }

  @Test
  public void testLogoGravity() {
    assertEquals(Gravity.BOTTOM | Gravity.START, new MapboxMapOptions().getLogoGravity());
    assertEquals(Gravity.BOTTOM, new MapboxMapOptions().logoGravity(Gravity.BOTTOM).getLogoGravity());
    assertNotEquals(Gravity.START, new MapboxMapOptions().logoGravity(Gravity.BOTTOM).getLogoGravity());
  }

  @Test
  public void testLogoMargins() {
    assertTrue(Arrays.equals(new int[] {0, 1, 2, 3}, new MapboxMapOptions().logoMargins(
      new int[] {0, 1, 2, 3}).getLogoMargins()));
    assertFalse(Arrays.equals(new int[] {0, 1, 2, 3}, new MapboxMapOptions().logoMargins(
      new int[] {0, 0, 0, 0}).getLogoMargins()));
  }

  @Test
  public void testAttributionTintColor() {
    assertEquals(-1, new MapboxMapOptions().getAttributionTintColor());
    assertEquals(Color.RED, new MapboxMapOptions().attributionTintColor(Color.RED).getAttributionTintColor());
  }

  @Test
  public void testAttributionEnabled() {
    assertTrue(new MapboxMapOptions().attributionEnabled(true).getAttributionEnabled());
    assertFalse(new MapboxMapOptions().attributionEnabled(false).getAttributionEnabled());
  }

  @Test
  public void testAttributionGravity() {
    assertEquals(Gravity.BOTTOM | Gravity.START, new MapboxMapOptions().getAttributionGravity());
    assertEquals(Gravity.BOTTOM, new MapboxMapOptions().attributionGravity(Gravity.BOTTOM).getAttributionGravity());
    assertNotEquals(Gravity.START, new MapboxMapOptions().attributionGravity(Gravity.BOTTOM).getAttributionGravity());
  }

  @Test
  public void testAttributionMargins() {
    assertTrue(Arrays.equals(new int[] {0, 1, 2, 3}, new MapboxMapOptions().attributionMargins(
      new int[] {0, 1, 2, 3}).getAttributionMargins()));
    assertFalse(Arrays.equals(new int[] {0, 1, 2, 3}, new MapboxMapOptions().attributionMargins(
      new int[] {0, 0, 0, 0}).getAttributionMargins()));
  }

  @Test
  public void testMinZoom() {
    assertEquals(MapboxConstants.MINIMUM_ZOOM, new MapboxMapOptions().getMinZoomPreference(), DELTA);
    assertEquals(5.0f, new MapboxMapOptions().minZoomPreference(5.0f).getMinZoomPreference(), DELTA);
    assertNotEquals(2.0f, new MapboxMapOptions().minZoomPreference(5.0f).getMinZoomPreference(), DELTA);
  }

  @Test
  public void testMaxZoom() {
    assertEquals(MapboxConstants.MAXIMUM_ZOOM, new MapboxMapOptions().getMaxZoomPreference(), DELTA);
    assertEquals(5.0f, new MapboxMapOptions().maxZoomPreference(5.0f).getMaxZoomPreference(), DELTA);
    assertNotEquals(2.0f, new MapboxMapOptions().maxZoomPreference(5.0f).getMaxZoomPreference(), DELTA);
  }

  @Test
  public void testLocationEnabled() {
    assertFalse(new MapboxMapOptions().getLocationEnabled());
    assertTrue(new MapboxMapOptions().locationEnabled(true).getLocationEnabled());
    assertFalse(new MapboxMapOptions().locationEnabled(false).getLocationEnabled());
  }

  @Test
  public void testTiltGesturesEnabled() {
    assertTrue(new MapboxMapOptions().getTiltGesturesEnabled());
    assertTrue(new MapboxMapOptions().tiltGesturesEnabled(true).getTiltGesturesEnabled());
    assertFalse(new MapboxMapOptions().tiltGesturesEnabled(false).getTiltGesturesEnabled());
  }

  @Test
  public void testScrollGesturesEnabled() {
    assertTrue(new MapboxMapOptions().getScrollGesturesEnabled());
    assertTrue(new MapboxMapOptions().scrollGesturesEnabled(true).getScrollGesturesEnabled());
    assertFalse(new MapboxMapOptions().scrollGesturesEnabled(false).getScrollGesturesEnabled());
  }

  @Test
  public void testZoomGesturesEnabled() {
    assertTrue(new MapboxMapOptions().getZoomGesturesEnabled());
    assertTrue(new MapboxMapOptions().zoomGesturesEnabled(true).getZoomGesturesEnabled());
    assertFalse(new MapboxMapOptions().zoomGesturesEnabled(false).getZoomGesturesEnabled());
  }

  @Test
  public void testRotateGesturesEnabled() {
    assertTrue(new MapboxMapOptions().getRotateGesturesEnabled());
    assertTrue(new MapboxMapOptions().rotateGesturesEnabled(true).getRotateGesturesEnabled());
    assertFalse(new MapboxMapOptions().rotateGesturesEnabled(false).getRotateGesturesEnabled());
  }

  @Test
  public void testZoomControlsEnabled() {
    assertFalse(new MapboxMapOptions().getZoomControlsEnabled());
    assertTrue(new MapboxMapOptions().zoomControlsEnabled(true).getZoomControlsEnabled());
    assertFalse(new MapboxMapOptions().zoomControlsEnabled(false).getZoomControlsEnabled());
  }

  @Test
  public void testStyleUrl() {
    assertEquals(Style.DARK, new MapboxMapOptions().styleUrl(Style.DARK).getStyle());
    assertNotEquals(Style.LIGHT, new MapboxMapOptions().styleUrl(Style.DARK).getStyle());
    assertNull(new MapboxMapOptions().getStyle());
  }

  @Test
  public void testCamera() {
    CameraPosition position = new CameraPosition.Builder().build();
    assertEquals(new CameraPosition.Builder(position).build(), new MapboxMapOptions().camera(position).getCamera());
    assertNotEquals(new CameraPosition.Builder().target(new LatLng(1, 1)), new MapboxMapOptions().camera(position));
    assertNull(new MapboxMapOptions().getCamera());
  }

  @Test
  public void testMyLocationForegroundTint() {
    assertEquals(Color.BLUE, new MapboxMapOptions()
      .myLocationForegroundTintColor(Color.BLUE).getMyLocationForegroundTintColor());
  }

  @Test
  public void testMyLocationBackgroundTint() {
    assertEquals(Color.BLUE, new MapboxMapOptions()
      .myLocationBackgroundTintColor(Color.BLUE).getMyLocationBackgroundTintColor());
  }

  @Test
  public void testPrefetchesTiles() {
    // Default value
    assertTrue(new MapboxMapOptions().getPrefetchesTiles());

    // Check mutations
    assertTrue(new MapboxMapOptions().setPrefetchesTiles(true).getPrefetchesTiles());
    assertFalse(new MapboxMapOptions().setPrefetchesTiles(false).getPrefetchesTiles());
  }
}

