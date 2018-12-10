package com.mapbox.mapboxsdk.maps;

import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.mockito.Mockito.mock;

public class MapboxMapTest {

  @Nullable
  private MapboxMap mapboxMap;

  @Before
  public void beforeTest() {

    mapboxMap = new MapboxMap(mock(NativeMapView.class),
      mock(Transform.class),
      mock(UiSettings.class),
      mock(Projection.class),
      mock(MapboxMap.OnGesturesManagerInteractionListener.class),
      mock(AnnotationManager.class),
      mock(CameraChangeDispatcher.class)
    );
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
