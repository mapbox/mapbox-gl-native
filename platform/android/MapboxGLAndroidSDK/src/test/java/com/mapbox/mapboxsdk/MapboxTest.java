package com.mapbox.mapboxsdk;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.DisplayMetrics;

import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;
import com.mapbox.mapboxsdk.maps.MapView;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import static junit.framework.TestCase.assertNotNull;
import static junit.framework.TestCase.assertSame;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.nullable;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MapboxTest {

  private Context context;
  private Context appContext;

  @Rule
  public ExpectedException expectedException = ExpectedException.none();

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

  @Test
  public void testNoInstance() {
    DisplayMetrics displayMetrics = mock(DisplayMetrics.class);
    Resources resources = mock(Resources.class);
    when(resources.getDisplayMetrics()).thenReturn(displayMetrics);
    when(context.getResources()).thenReturn(resources);
    TypedArray typedArray = mock(TypedArray.class);
    when(context.obtainStyledAttributes(nullable(AttributeSet.class), any(int[].class), anyInt(), anyInt()))
      .thenReturn(typedArray);

    expectedException.expect(MapboxConfigurationException.class);
    expectedException.expectMessage(
      "\nUsing MapView requires calling Mapbox.getInstance(Context context, String accessToken) before "
        + "inflating or creating the view. The access token parameter is required when using a Mapbox service."
        + "\nPlease see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one."
        + "\nMore information in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens."
    );
    new MapView(context);
  }

  @After
  public void after() {
    MapboxInjector.clear();
  }
}
