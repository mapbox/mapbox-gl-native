package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;

import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.android.gestures.ShoveGestureDetector;
import com.mapbox.android.gestures.StandardScaleGestureDetector;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Before;
import org.junit.Test;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class MapTouchListenersTest {

  private MapGestureDetector mapGestureDetector;
  private LatLng latLng;
  private PointF pointF;

  @Before
  public void setUp() throws Exception {
    latLng = new LatLng();
    pointF = new PointF();

    Projection projection = mock(Projection.class);
    when(projection.fromScreenLocation(pointF)).thenReturn(latLng);
    mapGestureDetector = new MapGestureDetector(null,
      null, projection, null, null, null, null);
  }

  @Test
  public void onMapClickListenerTest() throws Exception {
    MapboxMap.OnMapClickListener listener = mock(MapboxMap.OnMapClickListener.class);
    mapGestureDetector.addOnMapClickListener(listener);
    mapGestureDetector.notifyOnMapClickListeners(pointF);
    verify(listener, times(1)).onMapClick(latLng);

    mapGestureDetector.removeOnMapClickListener(listener);
    mapGestureDetector.notifyOnMapClickListeners(pointF);
    verify(listener, times(1)).onMapClick(latLng);
  }

  @Test
  public void onMapLongClickListenerTest() throws Exception {
    MapboxMap.OnMapLongClickListener listener = mock(MapboxMap.OnMapLongClickListener.class);
    mapGestureDetector.addOnMapLongClickListener(listener);
    mapGestureDetector.notifyOnMapLongClickListeners(pointF);
    verify(listener, times(1)).onMapLongClick(latLng);

    mapGestureDetector.removeOnMapLongClickListener(listener);
    mapGestureDetector.notifyOnMapLongClickListeners(pointF);
    verify(listener, times(1)).onMapLongClick(latLng);
  }

  @Test
  public void onFlingListenerTest() throws Exception {
    MapboxMap.OnFlingListener listener = mock(MapboxMap.OnFlingListener.class);
    mapGestureDetector.addOnFlingListener(listener);
    mapGestureDetector.notifyOnFlingListeners();
    verify(listener, times(1)).onFling();

    mapGestureDetector.removeOnFlingListener(listener);
    mapGestureDetector.notifyOnFlingListeners();
    verify(listener, times(1)).onFling();
  }

  @Test
  public void onScrollListenerTest() throws Exception {
    MapboxMap.OnScrollListener listener = mock(MapboxMap.OnScrollListener.class);
    mapGestureDetector.addOnScrollListener(listener);
    mapGestureDetector.notifyOnScrollListeners();
    verify(listener, times(1)).onScroll();

    mapGestureDetector.removeOnScrollListener(listener);
    mapGestureDetector.notifyOnScrollListeners();
    verify(listener, times(1)).onScroll();
  }

  @Test
  public void onMoveListenerTest() throws Exception {
    MapboxMap.OnMoveListener listener = mock(MapboxMap.OnMoveListener.class);
    MoveGestureDetector detector = mock(MoveGestureDetector.class);
    mapGestureDetector.addOnMoveListener(listener);
    mapGestureDetector.notifyOnMoveBeginListeners(detector);
    mapGestureDetector.notifyOnMoveListeners(detector);
    mapGestureDetector.notifyOnMoveEndListeners(detector);
    verify(listener, times(1)).onMoveBegin(detector);
    verify(listener, times(1)).onMove(detector);
    verify(listener, times(1)).onMoveEnd(detector);

    mapGestureDetector.removeOnMoveListener(listener);
    mapGestureDetector.notifyOnMoveBeginListeners(detector);
    mapGestureDetector.notifyOnMoveListeners(detector);
    mapGestureDetector.notifyOnMoveEndListeners(detector);
    verify(listener, times(1)).onMoveBegin(detector);
    verify(listener, times(1)).onMove(detector);
    verify(listener, times(1)).onMoveEnd(detector);
  }

  @Test
  public void onRotateListenerTest() throws Exception {
    MapboxMap.OnRotateListener listener = mock(MapboxMap.OnRotateListener.class);
    RotateGestureDetector detector = mock(RotateGestureDetector.class);
    mapGestureDetector.addOnRotateListener(listener);
    mapGestureDetector.notifyOnRotateBeginListeners(detector);
    mapGestureDetector.notifyOnRotateListeners(detector);
    mapGestureDetector.notifyOnRotateEndListeners(detector);
    verify(listener, times(1)).onRotateBegin(detector);
    verify(listener, times(1)).onRotate(detector);
    verify(listener, times(1)).onRotateEnd(detector);

    mapGestureDetector.removeOnRotateListener(listener);
    mapGestureDetector.notifyOnRotateBeginListeners(detector);
    mapGestureDetector.notifyOnRotateListeners(detector);
    mapGestureDetector.notifyOnRotateEndListeners(detector);
    verify(listener, times(1)).onRotateBegin(detector);
    verify(listener, times(1)).onRotate(detector);
    verify(listener, times(1)).onRotateEnd(detector);
  }

  @Test
  public void onScaleListenerTest() throws Exception {
    MapboxMap.OnScaleListener listener = mock(MapboxMap.OnScaleListener.class);
    StandardScaleGestureDetector detector = mock(StandardScaleGestureDetector.class);
    mapGestureDetector.addOnScaleListener(listener);
    mapGestureDetector.notifyOnScaleBeginListeners(detector);
    mapGestureDetector.notifyOnScaleListeners(detector);
    mapGestureDetector.notifyOnScaleEndListeners(detector);
    verify(listener, times(1)).onScaleBegin(detector);
    verify(listener, times(1)).onScale(detector);
    verify(listener, times(1)).onScaleEnd(detector);

    mapGestureDetector.removeOnScaleListener(listener);
    mapGestureDetector.notifyOnScaleBeginListeners(detector);
    mapGestureDetector.notifyOnScaleListeners(detector);
    mapGestureDetector.notifyOnScaleEndListeners(detector);
    verify(listener, times(1)).onScaleBegin(detector);
    verify(listener, times(1)).onScale(detector);
    verify(listener, times(1)).onScaleEnd(detector);
  }

  @Test
  public void onShoveListenerTest() throws Exception {
    MapboxMap.OnShoveListener listener = mock(MapboxMap.OnShoveListener.class);
    ShoveGestureDetector detector = mock(ShoveGestureDetector.class);
    mapGestureDetector.addShoveListener(listener);
    mapGestureDetector.notifyOnShoveBeginListeners(detector);
    mapGestureDetector.notifyOnShoveListeners(detector);
    mapGestureDetector.notifyOnShoveEndListeners(detector);
    verify(listener, times(1)).onShoveBegin(detector);
    verify(listener, times(1)).onShove(detector);
    verify(listener, times(1)).onShoveEnd(detector);

    mapGestureDetector.removeShoveListener(listener);
    mapGestureDetector.notifyOnShoveBeginListeners(detector);
    mapGestureDetector.notifyOnShoveListeners(detector);
    mapGestureDetector.notifyOnShoveEndListeners(detector);
    verify(listener, times(1)).onShoveBegin(detector);
    verify(listener, times(1)).onShove(detector);
    verify(listener, times(1)).onShoveEnd(detector);
  }
}
