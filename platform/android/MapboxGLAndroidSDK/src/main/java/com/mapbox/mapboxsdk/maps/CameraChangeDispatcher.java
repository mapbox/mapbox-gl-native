package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraIdleListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveCanceledListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;

class CameraChangeDispatcher implements MapboxMap.OnCameraMoveStartedListener, MapboxMap.OnCameraMoveListener,
  MapboxMap.OnCameraMoveCanceledListener, OnCameraIdleListener {

  private boolean idle = true;

  private final List<OnCameraMoveStartedListener> onCameraMoveStartedListenerList = new ArrayList<>();
  private final List<OnCameraMoveCanceledListener> onCameraMoveCanceledListenerList = new ArrayList<>();
  private final List<OnCameraMoveListener> onCameraMoveListenerList = new ArrayList<>();
  private final List<OnCameraIdleListener> onCameraIdleListenerList = new ArrayList<>();

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

    if (!onCameraMoveStartedListenerList.isEmpty()) {
      for (OnCameraMoveStartedListener cameraMoveStartedListener : onCameraMoveStartedListenerList) {
        cameraMoveStartedListener.onCameraMoveStarted(reason);
      }
    }
  }

  @Override
  public void onCameraMove() {
    if (onCameraMoveListener != null && !idle) {
      onCameraMoveListener.onCameraMove();
    }

    if (!onCameraMoveListenerList.isEmpty()) {
      for (OnCameraMoveListener cameraMoveListener : onCameraMoveListenerList) {
        cameraMoveListener.onCameraMove();
      }
    }
  }

  @Override
  public void onCameraMoveCanceled() {
    if (onCameraMoveCanceledListener != null && !idle) {
      onCameraMoveCanceledListener.onCameraMoveCanceled();
    }

    if (!onCameraMoveCanceledListenerList.isEmpty()) {
      for (OnCameraMoveCanceledListener cameraMoveCanceledListener : onCameraMoveCanceledListenerList) {
        cameraMoveCanceledListener.onCameraMoveCanceled();
      }
    }
  }

  @Override
  public void onCameraIdle() {
    if (!idle) {
      idle = true;
      if (onCameraIdleListener != null) {
        onCameraIdleListener.onCameraIdle();
      }

      if (!onCameraIdleListenerList.isEmpty()) {
        for (OnCameraIdleListener cameraIdleListener : onCameraIdleListenerList) {
          cameraIdleListener.onCameraIdle();
        }
      }
    }
  }

  void addOnCameraIdleListener(@NonNull OnCameraIdleListener listener) {
    onCameraIdleListenerList.add(listener);
  }

  void removeOnCameraIdleListener(@NonNull OnCameraIdleListener listener) {
    if (onCameraIdleListenerList.contains(listener)) {
      onCameraIdleListenerList.remove(listener);
    }
  }

  void addOnCameraMoveCancelListener(OnCameraMoveCanceledListener listener) {
    onCameraMoveCanceledListenerList.add(listener);
  }

  void removeOnCameraMoveCancelListener(OnCameraMoveCanceledListener listener) {
    if (onCameraMoveCanceledListenerList.contains(listener)) {
      onCameraMoveCanceledListenerList.remove(listener);
    }
  }

  void addOnCameraMoveStartedListener(OnCameraMoveStartedListener listener) {
    onCameraMoveStartedListenerList.add(listener);
  }

  void removeOnCameraMoveStartedListener(OnCameraMoveStartedListener listener) {
    if (onCameraMoveStartedListenerList.contains(listener)) {
      onCameraMoveStartedListenerList.remove(listener);
    }
  }

  void addOnCameraMoveListener(OnCameraMoveListener listener) {
    onCameraMoveListenerList.add(listener);
  }

  void removeOnCameraMoveListener(OnCameraMoveListener listener) {
    if (onCameraMoveListenerList.contains(listener)) {
      onCameraMoveListenerList.remove(listener);
    }
  }
}
