package com.mapbox.mapboxsdk.maps;

import android.os.Handler;
import android.os.Message;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;

import java.lang.annotation.Retention;
import java.lang.ref.WeakReference;
import java.util.concurrent.CopyOnWriteArrayList;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraIdleListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveCanceledListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveListener;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener;
import static java.lang.annotation.RetentionPolicy.SOURCE;

/**
 * Class responsible for dispatching camera change events to registered listeners.
 */
class CameraChangeDispatcher implements MapboxMap.OnCameraMoveStartedListener, MapboxMap.OnCameraMoveListener,
  MapboxMap.OnCameraMoveCanceledListener, OnCameraIdleListener {

  private final CameraChangeHandler handler = new CameraChangeHandler(this);

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

  @Retention(SOURCE)
  @IntDef( {MOVE_STARTED, MOVE, MOVE_CANCELED, IDLE})
  @interface CameraChange {
  }

  private static final int MOVE_STARTED = 0;
  private static final int MOVE = 1;
  private static final int MOVE_CANCELED = 2;
  private static final int IDLE = 3;

  @Override
  public void onCameraMoveStarted(final int reason) {
    moveStartedReason = reason;
    handler.scheduleMessage(MOVE_STARTED);
  }

  @Override
  public void onCameraMove() {
    handler.scheduleMessage(MOVE);
  }

  @Override
  public void onCameraMoveCanceled() {
    handler.scheduleMessage(MOVE_CANCELED);
  }

  @Override
  public void onCameraIdle() {
    handler.scheduleMessage(IDLE);
  }

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

  private void executeOnCameraMoveStarted() {
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

  private void executeOnCameraMove() {
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

  private void executeOnCameraMoveCancelled() {
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

  private void executeOnCameraIdle() {
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

  private static class CameraChangeHandler extends Handler {

    private WeakReference<CameraChangeDispatcher> dispatcherWeakReference;

    CameraChangeHandler(CameraChangeDispatcher dispatcher) {
      super();
      this.dispatcherWeakReference = new WeakReference<>(dispatcher);
    }

    @Override
    public void handleMessage(Message msg) {
      CameraChangeDispatcher dispatcher = dispatcherWeakReference.get();
      if (dispatcher != null) {
        switch (msg.what) {
          case MOVE_STARTED:
            dispatcher.executeOnCameraMoveStarted();
            break;
          case MOVE:
            dispatcher.executeOnCameraMove();
            break;
          case MOVE_CANCELED:
            dispatcher.executeOnCameraMoveCancelled();
            break;
          case IDLE:
            dispatcher.executeOnCameraIdle();
            break;
        }
      }
    }

    void scheduleMessage(@CameraChange int change) {
      CameraChangeDispatcher dispatcher = dispatcherWeakReference.get();
      if (dispatcher != null) {
        // if there is a movement that is cancelled/stopped and restarted in the same code block
        // we can safely assume that the movement will continue, no need for dispatching unnecessary callbacks sequence
        if (change == MOVE_STARTED) {
          boolean shouldReturn = !dispatcher.idle && (hasMessages(IDLE) || hasMessages(MOVE_CANCELED));
          removeMessages(IDLE);
          removeMessages(MOVE_CANCELED);

          if (shouldReturn) {
            return;
          }
        }

        Message message = new Message();
        message.what = change;
        this.sendMessage(message);
      }
    }
  }
}
