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
import java.util.concurrent.CopyOnWriteArrayList;

public class MapCameraController {

  @NonNull
  private final List<OnCameraChangedListener> cameraChangedListeners = new CopyOnWriteArrayList<>();

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
    queuedTransitions.put(CameraTransition.PROPERTY_TARGET, new LinkedList<CameraTransition>());
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

      boolean hasUpdates = false;
      CameraPosition.Builder builder = new CameraPosition.Builder();
      for (final CameraTransition transition : runningTransitions.values()) {
        if (transition.getStartTimeNanos() <= frameTimeNanos) {
          hasUpdates = true;
          if (frameTimeNanos >= transition.getEndTimeNanos()) {
            transition.setFinishing();
          }

          Object value = transition.isFinishing() ? transition.getEndValue() : transition.onFrame(frameTimeNanos);
          setCameraProperty(builder, transition.getCameraProperty(), value);
        }
      }

      if (hasUpdates) {
        transform.moveCamera(builder.build());
        for (OnCameraChangedListener listener : cameraChangedListeners) {
          listener.onCameraChanged();
        }
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

  /**
   * Starts transition immediately. Transition's delay doesn't queue the transition,
   * but blocks the slot until the delay time is fulfilled.
   */
  public void startTransition(CameraTransition transition) {
    behavior.animationStarted(this, transition);

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

  /**
   * Queues the transition. The transition is going to be executed immediately if there are other transitions
   * changing the same property as the scheduled one. If there is a running animation,
   * the queued one is going to get started when the running one finished or gets canceled.
   */
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

  /**
   * Cancels queued transitions.
   */
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

  /**
   * Cancels running transitions.
   */
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

  /**
   * Cancels all transitions.
   */
  public void cancelAllTransitions() {
    cancelQueuedTransitions();
    cancelRunningTransitions();
  }

  /**
   * Adds a camera listener.
   */
  public void addOnCameraChangedListener(OnCameraChangedListener listener) {
    cameraChangedListeners.add(listener);
  }

  /**
   * Removes a camera listener.
   */
  public void removeOnCameraChangedListener(OnCameraChangedListener listener) {
    cameraChangedListeners.remove(listener);
  }

  /**
   * Returns all queued transitions. The returned objects maps the camera property,
   * for example {@link CameraTransition#PROPERTY_TARGET} or {@link CameraTransition#PROPERTY_ZOOM},
   * to the list of queued transitions that will animate this property.
   */
  @NonNull
  public HashMap<Integer, LinkedList<CameraTransition>> getQueuedTransitions() {
    return new HashMap<>(queuedTransitions);
  }

  /**
   * Returns all running transitions. The returned objects maps the camera property,
   * for example {@link CameraTransition#PROPERTY_TARGET} or {@link CameraTransition#PROPERTY_ZOOM},
   * to the currently running transition.
   */
  @NonNull
  public HashMap<Integer, CameraTransition> getRunningTransitions() {
    return new HashMap<>(runningTransitions);
  }

  /**
   * Returns the currently set behavior.
   */
  @NonNull
  public CameraBehavior getBehavior() {
    return behavior;
  }

  /**
   * Sets the behavior.
   */
  public void setBehavior(@NonNull CameraBehavior behavior) {
    this.behavior = behavior;
  }

  /**
   * Returns the current camera position.
   */
  public CameraPosition getCameraPosition() {
    return transform.getCameraPosition();
  }

  void onDestroy() {
    cancelAllTransitions();
    choreographer.removeFrameCallback(frameCallback);
  }

  private static void setCameraProperty(CameraPosition.Builder builder, int property, Object value) {
    switch (property) {
      case CameraTransition.PROPERTY_TARGET:
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
      case CameraTransition.PROPERTY_TARGET:
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

  /**
   * Camera behavior's methods are called in various state of the camera transition.
   * It can be used to achieve a desired outcome when transitions collide (try to change the same camera property)
   * or perform business logic that would otherwise have to be handled in many other places that schedule animations.
   */
  public interface CameraBehavior {

    /**
     * Called whenever a transition is started. If a transition is queued,
     * it's only going to be started when at the top of the queue.
     */
    void animationStarted(MapCameraController controller, CameraTransition transition);

    /**
     * Called whenever there's a transition scheduled to be started,
     * but another one is already changing the requested camera property.
     * <p>
     * The transition that is going be returned by this method is going to be executed (or allowed to finish),
     * the other one is going to get canceled immediately.
     */
    @NonNull
    CameraTransition resolve(CameraTransition currentTransition, CameraTransition interruptingTransition);
  }

  public interface OnCameraChangedListener {
    void onCameraChanged();
  }

  /**
   * Default implementation of the camera behavior.
   */
  public static class DefaultCameraBehavior implements CameraBehavior {

    @Override
    public void animationStarted(MapCameraController controller, CameraTransition transition) {
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
