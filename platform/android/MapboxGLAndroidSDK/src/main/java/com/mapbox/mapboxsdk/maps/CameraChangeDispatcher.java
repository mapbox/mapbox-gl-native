package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;

import java.util.concurrent.CopyOnWriteArrayList;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraIdleListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveCanceledListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;

class CameraChangeDispatcher implements MapboxMap.OnCameraMoveStartedListener, MapboxMap.OnCameraMoveListener,
  MapboxMap.OnCameraMoveCanceledListener, OnCameraIdleListener {

  private boolean idle = true;

  private final CopyOnWriteArrayList<OnCameraMoveStartedListener> onCameraMoveStarted = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraMoveCanceledListener> onCameraMoveCanceled = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraMoveListener> onCameraMove = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraIdleListener> onCameraIdle = new CopyOnWriteArrayList<>();

  private OnCameraMoveStartedListener onCameraMoveStartedListener;
  private OnCameraMoveCanceledListener onCameraMoveCanceledListener;
  private OnCameraMoveListener onCameraMoveListener;
  private OnCameraIdleListener onCameraIdleListener;

  @Deprecated
  void setOnCameraMoveStartedListener(OnCameraMoveStartedListener onCameraMoveStartedListener) {
    this.onCameraMoveStartedListener = onCameraMoveStartedListener;
  }

  @Deprecated
  void setOnCameraMoveCanceledListener(OnCameraMoveCanceledListener onCameraMoveCanceledListener) {
    this.onCameraMoveCanceledListener = onCameraMoveCanceledListener;
  }

  @Deprecated
  void setOnCameraMoveListener(OnCameraMoveListener onCameraMoveListener) {
    this.onCameraMoveListener = onCameraMoveListener;
  }

  @Deprecated
  void setOnCameraIdleListener(OnCameraIdleListener onCameraIdleListener) {
    this.onCameraIdleListener = onCameraIdleListener;
  }

  @Override
  public void onCameraMoveStarted(int reason) {
    if (!idle) {
      return;
    }
    idle = false;

    // deprecated API
    if (onCameraMoveStartedListener != null) {
      onCameraMoveStartedListener.onCameraMoveStarted(reason);
    }

    // new API
    if (!onCameraMoveStarted.isEmpty()) {
      for (OnCameraMoveStartedListener cameraMoveStartedListener : onCameraMoveStarted) {
        cameraMoveStartedListener.onCameraMoveStarted(reason);
      }
    }
  }

  @Override
  public void onCameraMove() {
    // deprecated API
    if (onCameraMoveListener != null && !idle) {
      onCameraMoveListener.onCameraMove();
    }

    // new API
    if (!onCameraMove.isEmpty() && !idle) {
      for (OnCameraMoveListener cameraMoveListener : onCameraMove) {
        cameraMoveListener.onCameraMove();
      }
    }
  }

  @Override
  public void onCameraMoveCanceled() {
    // deprecated API
    if (onCameraMoveCanceledListener != null && !idle) {
      onCameraMoveCanceledListener.onCameraMoveCanceled();
    }

    // new API
    if (!onCameraMoveCanceled.isEmpty() && !idle) {
      for (OnCameraMoveCanceledListener cameraMoveCanceledListener : onCameraMoveCanceled) {
        cameraMoveCanceledListener.onCameraMoveCanceled();
      }
    }
  }

  @Override
  public void onCameraIdle() {
    if (!idle) {
      idle = true;
      // deprecated API
      if (onCameraIdleListener != null) {
        onCameraIdleListener.onCameraIdle();
      }

      // new API
      if (!onCameraIdle.isEmpty()) {
        for (OnCameraIdleListener cameraIdleListener : onCameraIdle) {
          cameraIdleListener.onCameraIdle();
        }
      }
    }
  }

  void addOnCameraIdleListener(@NonNull OnCameraIdleListener listener) {
    onCameraIdle.add(listener);
  }

  void removeOnCameraIdleListener(@NonNull OnCameraIdleListener listener) {
    if (onCameraIdle.contains(listener)) {
      onCameraIdle.remove(listener);
    }
  }

  void addOnCameraMoveCancelListener(OnCameraMoveCanceledListener listener) {
    onCameraMoveCanceled.add(listener);
  }

  void removeOnCameraMoveCancelListener(OnCameraMoveCanceledListener listener) {
    if (onCameraMoveCanceled.contains(listener)) {
      onCameraMoveCanceled.remove(listener);
    }
  }

  void addOnCameraMoveStartedListener(OnCameraMoveStartedListener listener) {
    onCameraMoveStarted.add(listener);
  }

  void removeOnCameraMoveStartedListener(OnCameraMoveStartedListener listener) {
    if (onCameraMoveStarted.contains(listener)) {
      onCameraMoveStarted.remove(listener);
    }
  }

  void addOnCameraMoveListener(OnCameraMoveListener listener) {
    onCameraMove.add(listener);
  }

  void removeOnCameraMoveListener(OnCameraMoveListener listener) {
    if (onCameraMove.contains(listener)) {
      onCameraMove.remove(listener);
    }
  }
}
