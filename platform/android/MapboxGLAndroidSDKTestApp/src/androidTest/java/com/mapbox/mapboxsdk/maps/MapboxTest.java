package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import org.junit.Test;

import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertSame;
import static junit.framework.Assert.assertTrue;

public class MapboxTest extends BaseActivityTest {

  private static final String ACCESS_TOKEN = "pk.0000000001";
  private static final String ACCESS_TOKEN_2 = "pk.0000000002";

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  @Test
  public void testConnected() {
    validateTestSetup();
    assertTrue(Mapbox.isConnected());

    // test manual connectivity
    Mapbox.setConnected(true);
    assertTrue(Mapbox.isConnected());
    Mapbox.setConnected(false);
    assertFalse(Mapbox.isConnected());

    // reset to Android connectivity
    Mapbox.setConnected(null);
    assertTrue(Mapbox.isConnected());
  }

  @Test
  public void setAccessToken() {
    validateTestSetup();
    String realToken =  Mapbox.getAccessToken();
    Mapbox.setAccessToken(ACCESS_TOKEN);
    assertSame(ACCESS_TOKEN, Mapbox.getAccessToken());
    Mapbox.setAccessToken(ACCESS_TOKEN_2);
    assertSame(ACCESS_TOKEN_2, Mapbox.getAccessToken());
    Mapbox.setAccessToken(realToken);
  }
}