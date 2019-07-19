package com.mapbox.mapboxsdk.maps;

import android.support.test.annotation.UiThreadTest;
import android.support.test.runner.AndroidJUnit4;

import com.mapbox.mapboxsdk.AppCenter;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;
import org.junit.runner.RunWith;

import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.assertNull;
import static junit.framework.Assert.assertSame;
import static junit.framework.Assert.assertTrue;

@RunWith(AndroidJUnit4.class)
public class MapboxTest extends AppCenter {

  private static final String ACCESS_TOKEN = "pk.0000000001";
  private static final String ACCESS_TOKEN_2 = "pk.0000000002";

  @Rule
  public ExpectedException expectedException = ExpectedException.none();

  private String realToken;

  @Before
  public void setup() {
    realToken = Mapbox.getAccessToken();
  }

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
    Mapbox.setAccessToken(ACCESS_TOKEN);
    assertSame(ACCESS_TOKEN, Mapbox.getAccessToken());
    Mapbox.setAccessToken(ACCESS_TOKEN_2);
    assertSame(ACCESS_TOKEN_2, Mapbox.getAccessToken());
  }

  @Test
  @UiThreadTest
  public void setInvalidAccessToken() {
    final String invalidAccessToken = "xyz";
    expectedException.expect(MapboxConfigurationException.class);
    expectedException.expectMessage(
      "A valid access token parameter is required when using a Mapbox service."
        + "\nPlease see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one."
        + "\nMore information in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens."
        + "Currently provided token is: " + invalidAccessToken
    );

    Mapbox.setAccessToken(invalidAccessToken);
    assertNull(Mapbox.getTelemetry());
    Mapbox.getSkuToken();
  }

  @Test
  @UiThreadTest
  public void setNullAccessToken() {
    expectedException.expect(MapboxConfigurationException.class);
    expectedException.expectMessage(
      "A valid access token parameter is required when using a Mapbox service."
        + "\nPlease see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one."
        + "\nMore information in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens."
        + "Currently provided token is: " + null
    );

    Mapbox.setAccessToken(null);
    assertNull(Mapbox.getTelemetry());
    Mapbox.getSkuToken();
  }

  @Test
  @UiThreadTest
  public void setValidAccessToken() {
    final String invalidAccessToken = "xyz";
    Mapbox.setAccessToken(invalidAccessToken);
    Mapbox.setAccessToken(ACCESS_TOKEN);
    assertNotNull(Mapbox.getTelemetry());
    assertNotNull(Mapbox.getSkuToken());
  }

  @After
  public void tearDown() {
    Mapbox.setAccessToken(realToken);
  }
}