package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;

import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class MapTouchListenersTest {

  @Test
  public void onMapClickListenerTest() throws Exception {
    LatLng latLng = new LatLng();
    PointF pointF = new PointF();

    Projection projection = mock(Projection.class);
    when(projection.fromScreenLocation(pointF)).thenReturn(latLng);
    MapGestureDetector mapGestureDetector = new MapGestureDetector(null,
      null, projection, null, null, null, null);

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
    LatLng latLng = new LatLng();
    PointF pointF = new PointF();

    Projection projection = mock(Projection.class);
    when(projection.fromScreenLocation(pointF)).thenReturn(latLng);
    MapGestureDetector mapGestureDetector = new MapGestureDetector(null,
      null, projection, null, null, null, null);

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
    LatLng latLng = new LatLng();
    PointF pointF = new PointF();

    Projection projection = mock(Projection.class);
    when(projection.fromScreenLocation(pointF)).thenReturn(latLng);
    MapGestureDetector mapGestureDetector = new MapGestureDetector(null,
      null, projection, null, null, null, null);

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
    LatLng latLng = new LatLng();
    PointF pointF = new PointF();

    Projection projection = mock(Projection.class);
    when(projection.fromScreenLocation(pointF)).thenReturn(latLng);
    MapGestureDetector mapGestureDetector = new MapGestureDetector(null,
      null, projection, null, null, null, null);

    MapboxMap.OnScrollListener listener = mock(MapboxMap.OnScrollListener.class);
    mapGestureDetector.addOnScrollListener(listener);
    mapGestureDetector.notifyOnScrollListeners();
    verify(listener, times(1)).onScroll();

    mapGestureDetector.removeOnScrollListener(listener);
    mapGestureDetector.notifyOnScrollListeners();
    verify(listener, times(1)).onScroll();
  }
}
