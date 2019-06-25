package com.mapbox.mapboxsdk.maps;

import android.support.test.annotation.UiThreadTest;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.AppCenter;
import com.mapbox.mapboxsdk.Mapbox;
import org.junit.Test;
import org.junit.runner.RunWith;

import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertSame;
import static junit.framework.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class MapboxTest extends AppCenter {

  private static final String ACCESS_TOKEN = "pk.0000000001";
  private static final String ACCESS_TOKEN_2 = "pk.0000000002";

  @Test
  @UiThreadTest
  public void testConnected() {
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
  @UiThreadTest
  public void setAccessToken() {
    String realToken =  Mapbox.getAccessToken();
    Mapbox.setAccessToken(ACCESS_TOKEN);
    assertSame(ACCESS_TOKEN, Mapbox.getAccessToken());
    Mapbox.setAccessToken(ACCESS_TOKEN_2);
    assertSame(ACCESS_TOKEN_2, Mapbox.getAccessToken());
    Mapbox.setAccessToken(realToken);
  }
}