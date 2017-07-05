package com.mapbox.mapboxsdk.maps;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraIdleListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveCanceledListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;

class CameraChangeDispatcher implements MapboxMap.OnCameraMoveStartedListener, MapboxMap.OnCameraMoveListener,
  MapboxMap.OnCameraMoveCanceledListener, OnCameraIdleListener {

  private boolean idle = true;

  private OnCameraMoveStartedListener onCameraMoveStartedListener;
  private OnCameraMoveCanceledListener onCameraMoveCanceledListener;
  private OnCameraMoveListener onCameraMoveListener;
  private OnCameraIdleListener onCameraIdleListener;

  void setOnCameraMoveStartedListener(OnCameraMoveStartedListener onCameraMoveStartedListener) {
    this.onCameraMoveStartedListener = onCameraMoveStartedListener;
  }

  void setOnCameraMoveCanceledListener(OnCameraMoveCanceledListener onCameraMoveCanceledListener) {
    this.onCameraMoveCanceledListener = onCameraMoveCanceledListener;
  }

  void setOnCameraMoveListener(OnCameraMoveListener onCameraMoveListener) {
    this.onCameraMoveListener = onCameraMoveListener;
  }

  void setOnCameraIdleListener(OnCameraIdleListener onCameraIdleListener) {
    this.onCameraIdleListener = onCameraIdleListener;
  }

  @Override
  public void onCameraMoveStarted(int reason) {
    if (!idle) {
      return;
    }

    idle = false;
    if (onCameraMoveStartedListener != null) {
      onCameraMoveStartedListener.onCameraMoveStarted(reason);
    }
  }

  @Override
  public void onCameraMove() {
    if (onCameraMoveListener != null && !idle) {
      onCameraMoveListener.onCameraMove();
    }
  }

  @Override
  public void onCameraMoveCanceled() {
    if (onCameraMoveCanceledListener != null && !idle) {
      onCameraMoveCanceledListener.onCameraMoveCanceled();
    }
  }

  @Override
  public void onCameraIdle() {
    if (!idle) {
      idle = true;
      if (onCameraIdleListener != null) {
        onCameraIdleListener.onCameraIdle();
      }
    }
  }
}
