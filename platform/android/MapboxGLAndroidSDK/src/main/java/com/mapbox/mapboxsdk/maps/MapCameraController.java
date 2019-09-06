package com.mapbox.mapboxsdk.maps;

import android.support.annotation.NonNull;
import android.view.Choreographer;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

public class MapCameraController {

  @NonNull
  private final HashMap<Integer, LinkedList<CameraTransition>> queuedTransitions = new HashMap<>(5);

  @NonNull
  private final HashMap<Integer, CameraTransition> runningTransitions = new HashMap<>(5);

  @NonNull
  private CameraBehavior behavior = new DefaultCameraBehavior();

  @NonNull
  private final Transform transform;

  private final Choreographer choreographer = Choreographer.getInstance();

  MapCameraController(@NonNull final Transform transform) {
    this.transform = transform;
    queuedTransitions.put(CameraTransition.PROPERTY_CENTER, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_ZOOM, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_PITCH, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_BEARING, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_ANCHOR, new LinkedList<CameraTransition>());
    queuedTransitions.put(CameraTransition.PROPERTY_PADDING, new LinkedList<CameraTransition>());
    choreographer.postFrameCallback(frameCallback);
  }

  private final Choreographer.FrameCallback frameCallback = new Choreographer.FrameCallback() {
    @Override
    public void doFrame(long frameTimeNanos) {
      List<CameraTransition> canceled = new ArrayList<>();
      Iterator<CameraTransition> iterator = runningTransitions.values().iterator();
      while (iterator.hasNext()) {
        CameraTransition transition = iterator.next();
        if (transition.isCanceled()) {
          iterator.remove();
          canceled.add(transition);
        }
      }

      for (CameraTransition transition : canceled) {
        transition.onCancel();
        checkNextTransition(transition.getCameraProperty());
      }

      boolean shouldRun = false;
      CameraPosition.Builder builder = new CameraPosition.Builder();
      for (final CameraTransition transition : runningTransitions.values()) {
        if (transition.getStartTimeNanos() <= frameTimeNanos) {
          shouldRun = true;
          if (frameTimeNanos >= transition.getEndTimeNanos()) {
            transition.setFinishing();
          }

          Object value = transition.isFinishing() ? transition.getEndValue() : transition.onFrame(frameTimeNanos);
          setCameraProperty(builder, transition.getCameraProperty(), value);
        }
      }

      if (shouldRun) {
        transform.moveCamera(builder.build());
      }

      List<CameraTransition> finished = new ArrayList<>();
      iterator = runningTransitions.values().iterator();
      while (iterator.hasNext()) {
        CameraTransition transition = iterator.next();
        transition.onProgress();

        if (transition.isFinishing()) {
          iterator.remove();
          finished.add(transition);
        }
      }

      for (CameraTransition transition : finished) {
        transition.onFinish();
        checkNextTransition(transition.getCameraProperty());
      }

      choreographer.postFrameCallback(this);
    }
  };

  public void startTransition(CameraTransition transition) {
    behavior.animationScheduled(this, transition);

    final CameraTransition runningTransition = runningTransitions.get(transition.getCameraProperty());
    if (runningTransition != null) {
      CameraTransition resultingTransition = behavior.resolve(runningTransition, transition);
      if (resultingTransition != transition && resultingTransition != runningTransition) {
        throw new UnsupportedOperationException();
      } else if (resultingTransition != transition) {
        transition.onCancel();
      } else {
        runningTransition.cancel();
        queuedTransitions.get(transition.getCameraProperty()).push(transition);
      }
    } else {
      long time = System.nanoTime();
      transition.onStart(getCameraPropertyValue(transition.getCameraProperty()), time);
      runningTransitions.put(transition.getCameraProperty(), transition);
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

  public void cancelQueuedTransitions() {
    List<CameraTransition> canceled = new ArrayList<>();
    for (List<CameraTransition> transitions : queuedTransitions.values()) {
      Iterator<CameraTransition> iterator = transitions.iterator();
      while (iterator.hasNext()) {
        CameraTransition transition = iterator.next();
        iterator.remove();
        canceled.add(transition);
      }
    }

    for (CameraTransition transition : canceled) {
      transition.onCancel();
    }
  }

  public void cancelRunningTransitions() {
    List<CameraTransition> canceled = new ArrayList<>();
    Iterator<CameraTransition> iterator = runningTransitions.values().iterator();
    while (iterator.hasNext()) {
      CameraTransition transition = iterator.next();
      iterator.remove();
      canceled.add(transition);
    }

    for (CameraTransition transition : canceled) {
      transition.onCancel();
      checkNextTransition(transition.getCameraProperty());
    }
  }

  public void cancelAllTransitions() {
    cancelQueuedTransitions();
    cancelRunningTransitions();
  }

  @NonNull
  public HashMap<Integer, LinkedList<CameraTransition>> getQueuedTransitions() {
    return new HashMap<>(queuedTransitions);
  }

  @NonNull
  public HashMap<Integer, CameraTransition> getRunningTransitions() {
    return new HashMap<>(runningTransitions);
  }

  @NonNull
  public CameraBehavior getBehavior() {
    return behavior;
  }

  public void setBehavior(@NonNull CameraBehavior behavior) {
    this.behavior = behavior;
  }

  void onDestroy() {
    cancelAllTransitions();
    choreographer.removeFrameCallback(frameCallback);
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

  private void checkNextTransition(int cameraProperty) {
    if (runningTransitions.get(cameraProperty) == null) {
      LinkedList<CameraTransition> queue = queuedTransitions.get(cameraProperty);
      assert queue != null;
      if (!queue.isEmpty()) {
        startTransition(queue.pop());
      }
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
      if (transition.getReason() == CameraTransition.REASON_GESTURE) {
        for (CameraTransition currentTransition : controller.getRunningTransitions().values()) {
          if (currentTransition.getReason() != CameraTransition.REASON_GESTURE) {
            currentTransition.cancel();
          }
        }

        for (List<CameraTransition> currentTransitions : controller.getQueuedTransitions().values()) {
          for (CameraTransition currentTransition : currentTransitions) {
            if (currentTransition.getReason() != CameraTransition.REASON_GESTURE) {
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
