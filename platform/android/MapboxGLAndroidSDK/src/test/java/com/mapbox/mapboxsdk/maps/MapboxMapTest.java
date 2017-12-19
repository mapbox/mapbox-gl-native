package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationViewSettings;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.mockito.Mockito.mock;

public class MapboxMapTest {

  private MapboxMap mapboxMap;

  @Before
  public void beforeTest() {

    mapboxMap = new MapboxMap(mock(NativeMapView.class),
      mock(Transform.class),
      mock(UiSettings.class),
      mock(TrackingSettings.class),
      mock(MyLocationViewSettings.class),
      mock(Projection.class),
      mock(MapboxMap.OnRegisterTouchListener.class),
      mock(AnnotationManager.class),
      mock(CameraChangeDispatcher.class));
  }

  @Test(expected = IllegalArgumentException.class)
  public void testAnimateCameraChecksDurationPositive() {
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLng(new LatLng(30.0, 30.0)),
      0, null);
  }

  @Test(expected = IllegalArgumentException.class)
  public void testEaseCameraChecksDurationPositive() {
    mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(30.0, 30.0)),
      0, null);
  }

  @After
  public void afterTest() {
    mapboxMap = null;
  }
}
