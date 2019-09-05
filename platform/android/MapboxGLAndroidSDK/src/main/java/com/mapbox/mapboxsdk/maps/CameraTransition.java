package com.mapbox.mapboxsdk.maps;

import android.support.v4.view.animation.PathInterpolatorCompat;
import android.view.animation.Interpolator;

import java.util.concurrent.CopyOnWriteArrayList;

public abstract class CameraTransition<T> {

  public static final int TYPE_ANY = 0;
  public static final int TYPE_GESTURE = 1;
  public static final int TYPE_LOCATION = 2;

  public static final Interpolator INTERPOLATOR_LINEAR = PathInterpolatorCompat.create(0f, 0f, 1f, 1f);
  public static final Interpolator INTERPOLATOR_EASING = PathInterpolatorCompat.create(0f, 0f, 0.25f, 1f);

  public static final int PROPERTY_CENTER = 0;
  static final int PROPERTY_ZOOM = 1;
  static final int PROPERTY_PITCH = 2;
  static final int PROPERTY_BEARING = 3;
  static final int PROPERTY_ANCHOR = 4;
  static final int PROPERTY_PADDING = 5;

  private final CopyOnWriteArrayList<Listener> listeners = new CopyOnWriteArrayList<>();

  private final int type;

  private T startValue;
  private final T endValue;

  private double startTime;
  private double endTime;
  private final double duration;
  private final double delay;

  private boolean isCanceled;
  private boolean isStarted;
  private boolean isFinishing;

  private final Interpolator interpolator;

  CameraTransition(int type, double duration, double delay, T endValue, Interpolator interpolator) {
    this.type = type;
    this.duration = duration;
    this.delay = delay;
    this.endValue = endValue;
    this.interpolator = interpolator;
  }

  void initTime(T startValue, double currentTime) {
    this.startTime = currentTime + delay;
    this.endTime = this.startTime + duration;
    this.startValue = startValue;
    this.isStarted = true;
  }

  public int getType() {
    return type;
  }

  public boolean isCanceled() {
    return isCanceled;
  }

  public void cancel() {
    isCanceled = true;
  }

  public boolean isStarted() {
    return isStarted;
  }

  public T getStartValue() {
    if (startValue == null) {
      throw new RuntimeException("start or queue the transition first");
    }
    return startValue;
  }

  public T getEndValue() {
    return endValue;
  }

  public double getStartTime() {
    if (startTime <= 0) {
      throw new RuntimeException("start or queue the transition first");
    }
    return startTime;
  }

  public double getEndTime() {
    if (endTime <= 0) {
      throw new RuntimeException("start or queue the transition first");
    }
    return endTime;
  }

  public double getDuration() {
    return duration;
  }

  public double getDelay() {
    return delay;
  }

  public Interpolator getInterpolator() {
    return interpolator;
  }

  public void addListener(Listener listener) {
    listeners.add(listener);
  }

  public void removeListener(Listener listener) {
    listeners.remove(listener);
  }

  T onFrame(double currentTime) {
    double animationPosition = (currentTime - startTime) / duration;
    double fraction = interpolator.getInterpolation((float) animationPosition);
    return getAnimatedValue(fraction);
  }

  boolean isFinishing() {
    return isFinishing;
  }

  void setFinishing() {
    isFinishing = true;
  }

  public void onProgress() {
    for (Listener listener : listeners) {
      listener.onProgress();
    }
  }

  void onCancel() {
    for (Listener listener : listeners) {
      listener.onCancel();
    }
  }

  void onFinish() {
    for (Listener listener : listeners) {
      listener.onFinish();
    }
  }

  abstract int getCameraProperty();

  // todo camera - make protected?
  abstract T getAnimatedValue(double fraction);

  public interface Listener {

    void onProgress();

    void onCancel();

    void onFinish();
  }
}
