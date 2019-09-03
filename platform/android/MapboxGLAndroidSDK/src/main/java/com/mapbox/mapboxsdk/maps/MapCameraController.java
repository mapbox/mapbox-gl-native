package com.mapbox.mapboxsdk.maps;

import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.camera.CameraPosition;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class MapCameraController {

  @NonNull
  private final List<CameraTransition> queuedTransitions = new ArrayList<>();

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
    @Override
    public void run() {
      CameraPosition cameraPosition = null;
      while (true) {
        if (destroyed) {
          return;
        }

        final CameraPosition update = cameraPosition;
        if (!runningTransitions.isEmpty()) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              CameraPosition finalUpdate = update;

              Iterator<CameraTransition> iterator = runningTransitions.values().iterator();
              while (iterator.hasNext()) {
                CameraTransition transition = iterator.next();
                if (transition.isCanceled()) {
                  transition.onCancel();
                  iterator.remove();
                  CameraPosition.Builder builder = new CameraPosition.Builder(finalUpdate);
                  if (transition.getCameraProperty() == CameraTransition.PROPERTY_CENTER) {
                    builder.target(null);
                  }
                  finalUpdate = builder.build();
                }
              }

              // todo camera - check if update is noop, abort then

              if (finalUpdate != null && finalUpdate.target != null) {
                // todo camera - remove if-check
                transform.moveCamera(finalUpdate);
              }

              iterator = runningTransitions.values().iterator();
              while (iterator.hasNext()) {
                CameraTransition transition = iterator.next();
                if (transition.isFinishing()) {
                  transition.onFinish();
                  iterator.remove();
                }
              }
            }
          });
        }

        // todo camera - what's the correct delay not to flood and block the main thread?
        double nextFrameTime = System.currentTimeMillis() + 5;

        if (!runningTransitions.isEmpty()) {
          boolean willRun = false;
          for (CameraTransition transition : runningTransitions.values()) {
            if (transition.startTime <= nextFrameTime) {
              willRun = true;
              break;
            }
          }

          if (willRun) {
            CameraPosition.Builder builder = new CameraPosition.Builder();

            TargetCameraTransition targetCameraTransition =
              (TargetCameraTransition) runningTransitions.get(CameraTransition.PROPERTY_CENTER);
            if (targetCameraTransition != null && targetCameraTransition.startTime <= nextFrameTime) {
              if (nextFrameTime >= targetCameraTransition.endTime) {
                builder.target(targetCameraTransition.endValue);
                targetCameraTransition.setFinishing();
              } else {
                builder.target(targetCameraTransition.onFrame(nextFrameTime));
              }

              cameraPosition = builder.build();
            }
          } else {
            cameraPosition = null;
          }

        } else {
          cameraPosition = null;
        }

        double sleepTime = nextFrameTime - System.currentTimeMillis();
        int nanos = (int) (sleepTime % 999_999);

        try {
          Thread.sleep((long) sleepTime, nanos);
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
      }
    }
  }, "MapCameraController");

  MapCameraController(@NonNull final Transform transform) {
    this.transform = transform;
    thread.start();
  }

  public void startTransition(final CameraTransition transition) {
    transition.setStartTime(System.currentTimeMillis() + transition.getDelay());
    switch (transition.getCameraProperty()) {
      case CameraTransition.PROPERTY_CENTER:
        transition.setStartValue(transform.getCameraPosition().target);
        break;
    }

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
        schedule(transition);
      }
    } else {
      schedule(transition);
    }
  }

  @NonNull
  public List<CameraTransition> getQueuedTransitions() {
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
    try {
      destroyed = true;
      thread.join();
    } catch (InterruptedException ex) {
      ex.printStackTrace();
    }
  }

  public interface CameraBehavior {

    void animationScheduled(MapCameraController controller, CameraTransition transition);

    @NonNull
    CameraTransition resolve(CameraTransition currentTransition, CameraTransition interruptingTransition);
  }

  private void schedule(CameraTransition transition) {
    runningTransitions.put(transition.getCameraProperty(), transition);
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

        for (CameraTransition currentTransition : controller.getQueuedTransitions()) {
          if (currentTransition.getType() != CameraTransition.TYPE_GESTURE) {
            currentTransition.cancel();
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
