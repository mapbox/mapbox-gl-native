package com.mapbox.mapboxsdk.telemetry;

import org.junit.Test;

import okhttp3.internal.Util;

import static junit.framework.Assert.assertEquals;

public class HttpTransportTest {

  @Test
  public void testNonAsciiUserAgent() {

    final String swedishUserAgent = "Sveriges Fjäll/1.0/1 MapboxEventsAndroid/4.0.0-SNAPSHOT";
    final String asciiVersion = "Sveriges Fj?ll/1.0/1 MapboxEventsAndroid/4.0.0-SNAPSHOT";

    assertEquals("asciiVersion and swedishUserAgent should match", asciiVersion,
      Util.toHumanReadableAscii(swedishUserAgent));
  }
}
