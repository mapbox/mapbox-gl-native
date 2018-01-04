package com.mapbox.mapboxsdk.maps;

import org.junit.Test;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

public class CameraChangeDispatcherTest {

  @Test
  public void testSetCameraIdleListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraIdleListener listener = mock(MapboxMap.OnCameraIdleListener.class);
    dispatcher.setOnCameraIdleListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    dispatcher.onCameraIdle();
    verify(listener).onCameraIdle();
  }

  @Test
  public void testSetCameraMoveStartedListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraMoveStartedListener listener = mock(MapboxMap.OnCameraMoveStartedListener.class);
    dispatcher.setOnCameraMoveStartedListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    verify(listener).onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
  }

  @Test
  public void testSetCameraMoveCancelListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraMoveCanceledListener listener = mock(MapboxMap.OnCameraMoveCanceledListener.class);
    dispatcher.setOnCameraMoveCanceledListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    dispatcher.onCameraMoveCanceled();
    verify(listener).onCameraMoveCanceled();
  }

  @Test
  public void testSetCameraMoveListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraMoveListener listener = mock(MapboxMap.OnCameraMoveListener.class);
    dispatcher.setOnCameraMoveListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    dispatcher.onCameraMove();
    verify(listener).onCameraMove();
  }

  @Test
  public void testAddCameraIdleListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraIdleListener listener = mock(MapboxMap.OnCameraIdleListener.class);
    dispatcher.addOnCameraIdleListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    dispatcher.onCameraIdle();
    verify(listener).onCameraIdle();
  }

  @Test
  public void testAddCameraMoveStartedListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraMoveStartedListener listener = mock(MapboxMap.OnCameraMoveStartedListener.class);
    dispatcher.addOnCameraMoveStartedListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    verify(listener).onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
  }

  @Test
  public void testAddCameraMoveCancelListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraMoveCanceledListener listener = mock(MapboxMap.OnCameraMoveCanceledListener.class);
    dispatcher.addOnCameraMoveCancelListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    dispatcher.onCameraMoveCanceled();
    verify(listener).onCameraMoveCanceled();
  }

  @Test
  public void testAddCameraMoveListener() {
    CameraChangeDispatcher dispatcher = new CameraChangeDispatcher();
    MapboxMap.OnCameraMoveListener listener = mock(MapboxMap.OnCameraMoveListener.class);
    dispatcher.addOnCameraMoveListener(listener);
    dispatcher.onCameraMoveStarted(MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE);
    dispatcher.onCameraMove();
    verify(listener).onCameraMove();
  }
}
