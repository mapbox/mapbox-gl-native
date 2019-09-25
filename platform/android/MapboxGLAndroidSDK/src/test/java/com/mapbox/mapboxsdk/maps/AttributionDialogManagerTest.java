package com.mapbox.mapboxsdk.maps;

import android.content.Context;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.robolectric.RobolectricTestRunner;

import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
public class AttributionDialogManagerTest {
  @InjectMocks
  Context context = mock(Context.class);

  @InjectMocks
  MapboxMap mapboxMap = mock(MapboxMap.class);

  @InjectMocks
  Style style = mock(Style.class);

  private AttributionDialogManager attributionDialogManager;
  private CameraPosition cameraPosition;

  private static final String ASSERT_MAPBOX_TOKEN = "TestAccessToken";

  private static final String ASSERT_MAPBOX_STYLE_URI = "mapbox://styles/mapbox/streets-v11";
  private static final String ASSERT_MAPBOX_LOCAL_STYLE_URI = "asset://style.json";

  private static final String ASSERT_MAPBOX_PACKAGE_NAME = "com.mapbox.attributionmanagertest";

  private static final String ASSERT_MAPBOX_FEEDBACK_FINAL_URL =
          "https://apps.mapbox.com/feedback?referrer=com.mapbox.attributionmanagertest&"
                  + "access_token=TestAccessToken&owner=mapbox&id=streets-v11"
                  + "#/22.200001/11.100000/12.000000/24.000000/5";
  private static final String ASSERT_MAPBOX_FEEDHACK_FINAL_URL_LOCAL_STYLE =
          "https://apps.mapbox.com/feedback?referrer=com.mapbox.attributionmanagertest&"
                  + "access_token=TestAccessToken#/22.200001/11.100000/12.000000/24.000000/5";
  private static final String ASSERT_MAPBOX_FEEDBACL_FINAL_URL_NULL_CAMERA_POSITION =
          "https://apps.mapbox.com/feedback?referrer=com.mapbox.attributionmanagertest&access_token=TestAccessToken";

  @Before
  public void beforeTest() {
    attributionDialogManager = new AttributionDialogManager(context, mapboxMap);
    cameraPosition = new CameraPosition.Builder(CameraPosition.DEFAULT)
            .tilt(5.0f).zoom(12).bearing(24.0f).target(new LatLng(11.1f, 22.2f)).build();
  }

  @Test
  public void testSanity() {
    assertNotNull("AttributionDialogManager should not be null", attributionDialogManager);
  }

  @Test
  public void testBuildMapFeedbackMapUrl() {
    when(context.getApplicationContext()).thenReturn(context);
    when(context.getPackageName()).thenReturn(ASSERT_MAPBOX_PACKAGE_NAME);
    when(style.getUri()).thenReturn(ASSERT_MAPBOX_STYLE_URI);
    when(mapboxMap.getCameraPosition()).thenReturn(cameraPosition);
    when(mapboxMap.getStyle()).thenReturn(style);

    Assert.assertEquals(ASSERT_MAPBOX_FEEDBACK_FINAL_URL,
            attributionDialogManager.buildMapFeedbackMapUrl(ASSERT_MAPBOX_TOKEN));
  }

  @Test
  public void testBuildMapFeedbackMapUrlWithLocalStyleJson() {
    when(context.getApplicationContext()).thenReturn(context);
    when(context.getPackageName()).thenReturn(ASSERT_MAPBOX_PACKAGE_NAME);
    when(style.getUri()).thenReturn(ASSERT_MAPBOX_LOCAL_STYLE_URI);
    when(mapboxMap.getCameraPosition()).thenReturn(cameraPosition);
    when(mapboxMap.getStyle()).thenReturn(style);

    Assert.assertEquals(ASSERT_MAPBOX_FEEDHACK_FINAL_URL_LOCAL_STYLE,
            attributionDialogManager.buildMapFeedbackMapUrl(ASSERT_MAPBOX_TOKEN));
  }

  @Test
  public void testBuildMapFeedbackMapUrlWithNullCameraPosition() {
    when(context.getApplicationContext()).thenReturn(context);
    when(context.getPackageName()).thenReturn(ASSERT_MAPBOX_PACKAGE_NAME);
    when(style.getUri()).thenReturn(ASSERT_MAPBOX_LOCAL_STYLE_URI);
    when(mapboxMap.getCameraPosition()).thenReturn(null);
    when(mapboxMap.getStyle()).thenReturn(style);

    Assert.assertEquals(ASSERT_MAPBOX_FEEDBACL_FINAL_URL_NULL_CAMERA_POSITION,
            attributionDialogManager.buildMapFeedbackMapUrl(ASSERT_MAPBOX_TOKEN));
  }


}
