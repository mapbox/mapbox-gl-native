package com.mapbox.mapboxsdk.maps;

import android.os.Handler;
import android.support.annotation.NonNull;

import java.util.concurrent.CopyOnWriteArrayList;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraIdleListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveCanceledListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;

/**
 * Class responsible for dispatching camera change events to registered listeners.
 */
class CameraChangeDispatcher implements MapboxMap.OnCameraMoveStartedListener, MapboxMap.OnCameraMoveListener,
  MapboxMap.OnCameraMoveCanceledListener, OnCameraIdleListener {

  private final Handler handler = new Handler();

  private boolean idle = true;
  private int moveStartedReason;

  private final CopyOnWriteArrayList<OnCameraMoveStartedListener> onCameraMoveStarted = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraMoveCanceledListener> onCameraMoveCanceled = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraMoveListener> onCameraMove = new CopyOnWriteArrayList<>();
  private final CopyOnWriteArrayList<OnCameraIdleListener> onCameraIdle = new CopyOnWriteArrayList<>();

  private OnCameraMoveStartedListener onCameraMoveStartedListener;
  private OnCameraMoveCanceledListener onCameraMoveCanceledListener;
  private OnCameraMoveListener onCameraMoveListener;
  private OnCameraIdleListener onCameraIdleListener;

  private final Runnable onCameraMoveStartedRunnable = new Runnable() {
    @Override
    public void run() {
      if (!idle) {
        return;
      }
      idle = false;

      // deprecated API
      if (onCameraMoveStartedListener != null) {
        onCameraMoveStartedListener.onCameraMoveStarted(moveStartedReason);
      }

      // new API
      if (!onCameraMoveStarted.isEmpty()) {
        for (OnCameraMoveStartedListener cameraMoveStartedListener : onCameraMoveStarted) {
          cameraMoveStartedListener.onCameraMoveStarted(moveStartedReason);
        }
      }
    }
  };

  private final Runnable onCameraMoveRunnable = new Runnable() {
    @Override
    public void run() {
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
  };

  private final Runnable onCameraMoveCancelRunnable = new Runnable() {
    @Override
    public void run() {
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
  };

  private final Runnable onCameraIdleRunnable = new Runnable() {
    @Override
    public void run() {
      if (idle) {
        return;
      }
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
  };

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
  public void onCameraMoveStarted(final int reason) {
    moveStartedReason = reason;
    handler.post(onCameraMoveStartedRunnable);
  }

  @Override
  public void onCameraMove() {
    handler.post(onCameraMoveRunnable);
  }

  @Override
  public void onCameraMoveCanceled() {
    handler.post(onCameraMoveCancelRunnable);
  }

  @Override
  public void onCameraIdle() {
    handler.post(onCameraIdleRunnable);
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
