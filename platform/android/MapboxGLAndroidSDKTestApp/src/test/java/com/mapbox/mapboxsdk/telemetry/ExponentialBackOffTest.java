package com.mapbox.mapboxsdk.telemetry;

import org.junit.Test;

import static junit.framework.Assert.assertEquals;

public class ExponentialBackOffTest {

  @Test
  public void testExponentialBackOff() {
    MapboxEventManager.ExponentialBackoffCounter counter = new MapboxEventManager.ExponentialBackoffCounter();
    assertEquals(30000, counter.getNextCount());
    assertEquals(60000, counter.getNextCount());
    assertEquals(90000, counter.getNextCount());
    assertEquals(120000, counter.getNextCount());
    assertEquals(150000, counter.getNextCount());
    assertEquals(180000, counter.getNextCount());
  }
}