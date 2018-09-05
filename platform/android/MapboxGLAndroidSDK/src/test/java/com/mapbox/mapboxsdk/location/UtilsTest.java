package com.mapbox.mapboxsdk.location;

import org.junit.Test;

import static junit.framework.Assert.assertEquals;

public final class UtilsTest {

  @Test
  public void shortestRotation_doesReturnValueDistanceQuickestToZero() throws Exception {
    float value = Utils.shortestRotation(0, 181);
    assertEquals(360f, value);
    value = Utils.shortestRotation(0, 179);
    assertEquals(0f, value);
    value = Utils.shortestRotation(0, 180);
    assertEquals(0f, value);
  }

  @Test
  public void shortestRotation_doesReturnValueDistanceQuickestToFifty() throws Exception {
    float value = Utils.shortestRotation(50, 231);
    assertEquals(410f, value);
    value = Utils.shortestRotation(50, 229);
    assertEquals(50f, value);
    value = Utils.shortestRotation(50, 180);
    assertEquals(50f, value);
  }
}
