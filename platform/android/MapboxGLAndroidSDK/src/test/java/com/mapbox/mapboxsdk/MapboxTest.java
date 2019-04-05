package com.mapbox.mapboxsdk;

import android.content.Context;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static junit.framework.TestCase.assertNotNull;
import static junit.framework.TestCase.assertSame;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MapboxTest {

  private Context context;
  private Context appContext;

  @Before
  public void before() {
    context = mock(Context.class);
    appContext = mock(Context.class);
    when(context.getApplicationContext()).thenReturn(appContext);
  }

  @Test
  public void testGetAccessToken() {
    final String accessToken = "pk.0000000001";
    MapboxInjector.inject(context, accessToken);
    assertSame(accessToken, Mapbox.getAccessToken());
  }

  @Test
  public void testApplicationContext() {
    MapboxInjector.inject(context, "pk.0000000001");
    assertNotNull(Mapbox.getApplicationContext());
    assertNotEquals(context, appContext);
    assertEquals(appContext, appContext);
  }

  @Test
  public void testPkTokenValid() {
    assertTrue(Mapbox.isAccessTokenValid("pk.0000000001"));
  }

  @Test
  public void testSkTokenValid() {
    assertTrue(Mapbox.isAccessTokenValid("sk.0000000001"));
  }

  @Test
  public void testEmptyToken() {
    assertFalse(Mapbox.isAccessTokenValid(""));
  }

  @Test
  public void testNullToken() {
    assertFalse(Mapbox.isAccessTokenValid(null));
  }

  @Test
  public void testBlaBlaToken() {
    assertFalse(Mapbox.isAccessTokenValid("blabla"));
  }

  @After
  public void after() {
    MapboxInjector.clear();
  }

}
