package com.mapbox.mapboxsdk.maps;

import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

public class MapCameraController {

  @NonNull
  private final HashMap<Integer, LinkedList<CameraTransition>> queuedTransitions = new HashMap<>(5);

  @NonNull
  private final ReentrantLock runningTransitionsLock = new ReentrantLock();

  @NonNull
  private final HashMap<Integer, CameraTransition> runningTransitions = new HashMap<>(5);

  @NonNull
  private CameraBehavior behavior = new DefaultCameraBehavior();

  @NonNull
  private final Handler handler = new Handler(Looper.getMainLooper());

  @NonNull
  private final Transform transform;

  private volatile boolean destroyed;

  @NonNull
  private final Thread thread = new Thread(new Runnable() {

    private static final short MIN_FRAME_INTERVAL = 10;

    @Override
    public void run() {
      CameraPosition cameraPosition = null;
      while (true) {
        if (destroyed) {
          return;
        }

        final CameraPosition update = cameraPosition;
        if (update != null) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              if (destroyed) {
                return;
              }

              CameraPosition finalUpdate = update;

              synchronized (runningTransitionsLock) {
                Iterator<CameraTransition> iterator = runningTransitions.values().iterator();
                while (iterator.hasNext()) {
                  CameraTransition transition = iterator.next();
                  if (transition.isCanceled()) {
                    transition.onCancel();
                    iterator.remove();
                    CameraPosition.Builder builder = new CameraPosition.Builder(finalUpdate);
                    setCameraProperty(builder, transition.getCameraProperty(), null);
                    finalUpdate = builder.build();

                    startNextTransition(transition.getCameraProperty());
                  }
                }
              }

              // todo camera - check if update is noop, abort then

              transform.moveCamera(finalUpdate);

              synchronized (runningTransitionsLock) {
                Iterator<CameraTransition> iterator = runningTransitions.values().iterator();
                while (iterator.hasNext()) {
                  CameraTransition transition = iterator.next();
                  if (transition.isFinishing()) {
                    transition.onFinish();
                    iterator.remove();

                    startNextTransition(transition.getCameraProperty());
                  }
                }
              }
            }
          });
        }

        // todo camera - what's the correct delay not to flood and block the main thread?
        long nextFrameTime = System.currentTimeMillis() + MIN_FRAME_INTERVAL;

        synchronized (runningTransitionsLock) {
          boolean willRun = false;
          CameraPosition.Builder builder = new CameraPosition.Builder();
          for (final CameraTransition transition : runningTransitions.values()) {
            if (transition.getStartTime() <= nextFrameTime) {
              willRun = true;
              if (nextFrameTime >= transition.getEndTime()) {
                handler.post(new Runnable() {
                  @Override
                  public void run() {
                    transition.setFinishing();
                  }
                });
              }

              Object value = transition.isFinishing() ? transition.getEndValue() : transition.onFrame(nextFrameTime);
              setCameraProperty(builder, transition.getCameraProperty(), value);
            }
          }

          if (willRun) {
            cameraPosition = builder.build();
          } else {
            cameraPosition = null;
          }
        }

        long sleepTime = nextFrameTime - System.currentTimeMillis();

        if (sleepTime >= 0) {
          try {
            Thread.sleep(sleepTime);
          } catch (InterruptedException e) {
            e.printStackTrace();
          }
        }
      }
    }
  }, "MapCameraController");

  MapCameraController(@NonNull final Transform transform) {
    this.transform = transform;
    queuedTransitions.put(CameraTransition.PROPERTY_CENTER, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_ZOOM, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_PITCH, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_BEARING, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_ANCHOR, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_PADDING, new LinkedList<CameraTransition>());
    thread.start();
  }

  public void startTransition(CameraTransition transition) {
    long time = System.currentTimeMillis();
    transition.initTime(getCameraPropertyValue(transition.getCameraProperty()), time);

    synchronized (runningTransitionsLock) {
      behavior.animationScheduled(this, transition);

      final CameraTransition runningTransition = runningTransitions.get(transition.getCameraProperty());
      if (runningTransition != null) {
        CameraTransition resultingTransition = behavior.resolve(runningTransition, transition);
        if (resultingTransition != transition && resultingTransition != runningTransition) {
          throw new UnsupportedOperationException();
        } else if (resultingTransition != transition) {
          // todo camera - invoke cancel callback right away
          transition.cancel();
        } else {
          runningTransition.cancel();
          runningTransitions.put(transition.getCameraProperty(), transition);
        }
      } else {
        runningTransitions.put(transition.getCameraProperty(), transition);
      }
    }
  }

  public void queueTransition(CameraTransition transition) {
    CameraTransition running = runningTransitions.get(transition.getCameraProperty());
    LinkedList<CameraTransition> queued = queuedTransitions.get(transition.getCameraProperty());
    assert queued != null;
    if (running == null && queued.isEmpty()) {
      startTransition(transition);
    } else {
      queued.addLast(transition);
    }
  }

  @NonNull
  public HashMap<Integer, LinkedList<CameraTransition>> getQueuedTransitions() {
    return queuedTransitions;
  }

  @NonNull
  public HashMap<Integer, CameraTransition> getRunningTransitions() {
    return runningTransitions;
  }

  @NonNull
  public CameraBehavior getBehavior() {
    return behavior;
  }

  public void setBehavior(@NonNull CameraBehavior behavior) {
    this.behavior = behavior;
  }

  void onDestroy() {
    destroyed = true;

    for (List<CameraTransition> transitions : queuedTransitions.values()) {
      for (CameraTransition transition : transitions) {
        transition.onCancel();
      }
    }
    queuedTransitions.clear();

    for (CameraTransition transition : runningTransitions.values()) {
      transition.onCancel();
    }
    runningTransitions.clear();

    try {
      thread.join();
    } catch (InterruptedException ex) {
      ex.printStackTrace();
    }
  }

  private static void setCameraProperty(CameraPosition.Builder builder, int property, Object value) {
    switch (property) {
      case CameraTransition.PROPERTY_CENTER:
        builder.target(value == null ? null : (LatLng) value);
        break;

      case CameraTransition.PROPERTY_ZOOM:
        builder.zoom(value == null ? -1 : (Double) value);
        break;

      case CameraTransition.PROPERTY_PITCH:
        builder.tilt(value == null ? -1 : (Double) value);
        break;

      case CameraTransition.PROPERTY_BEARING:
        builder.bearing(value == null ? -1 : (Double) value);
        break;

      case CameraTransition.PROPERTY_PADDING:
        if (value instanceof Double[]) {
          double[] padding = new double[4];
          for (int i = 0; i < 4; i++) {
            padding[i] = ((Double[]) value)[i];
          }
          builder.padding(padding);
        } else {
          builder.padding(value == null ? null : (double[]) value);
        }
        break;
    }
  }

  private Object getCameraPropertyValue(int property) {
    CameraPosition cameraPosition = transform.getCameraPosition();
    assert cameraPosition != null;
    switch (property) {
      case CameraTransition.PROPERTY_CENTER:
        return cameraPosition.target;

      case CameraTransition.PROPERTY_ZOOM:
        return cameraPosition.zoom;

      case CameraTransition.PROPERTY_PITCH:
        return cameraPosition.tilt;

      case CameraTransition.PROPERTY_BEARING:
        return cameraPosition.bearing;

      case CameraTransition.PROPERTY_PADDING:
        double[] padding = cameraPosition.padding;
        final Double[] result = new Double[padding.length];
        for (int i = 0; i < padding.length; i++) {
          result[i] = padding[i];
        }
        return result;

      default:
        throw new IllegalArgumentException("no such property");
    }
  }

  private void startNextTransition(int cameraProperty) {
    LinkedList<CameraTransition> queue = queuedTransitions.get(cameraProperty);
    assert queue != null;
    if (!queue.isEmpty()) {
      startTransition(queue.pop());
    }
  }

  public interface CameraBehavior {

    void animationScheduled(MapCameraController controller, CameraTransition transition);

    @NonNull
    CameraTransition resolve(CameraTransition currentTransition, CameraTransition interruptingTransition);
  }

  public static class DefaultCameraBehavior implements CameraBehavior {

    @Override
    public void animationScheduled(MapCameraController controller, CameraTransition transition) {
      if (transition.getType() == CameraTransition.TYPE_GESTURE) {
        for (CameraTransition currentTransition : controller.getRunningTransitions().values()) {
          if (currentTransition.getType() != CameraTransition.TYPE_GESTURE) {
            currentTransition.cancel();
          }
        }

        for (List<CameraTransition> currentTransitions : controller.getQueuedTransitions().values()) {
          for (CameraTransition currentTransition : currentTransitions) {
            if (currentTransition.getType() != CameraTransition.TYPE_GESTURE) {
              currentTransition.cancel();
            }
          }
        }
      }
    }

    @NonNull
    @Override
    public CameraTransition resolve(CameraTransition currentTransition, CameraTransition interruptingTransition) {
      return interruptingTransition;
    }
  }
}
