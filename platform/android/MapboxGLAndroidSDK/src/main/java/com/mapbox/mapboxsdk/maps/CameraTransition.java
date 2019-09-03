package com.mapbox.mapboxsdk.maps;

public abstract class CameraTransition<T> {

  static final int PROPERTY_CENTER = 0;
  static final int PROPERTY_ZOOM = 1;
  static final int PROPERTY_PITCH = 2;
  static final int PROPERTY_BEARING = 3;
  static final int PROPERTY_ANCHOR = 4;
  static final int PROPERTY_PADDING = 5;

  public static final int TYPE_ANY = 0;
  public static final int TYPE_GESTURE = 1;
  public static final int TYPE_LOCATION = 2;

  private final int type;
  T startValue;
  protected final T endValue;
  double startTime;
  protected final double duration;
  private final double delay;
  protected final double endTime;
  protected boolean canceled;
  private boolean isFinishing;

  CameraTransition(int type, double duration, double delay, T endValue) {
    this.type = type;
    this.startTime = System.currentTimeMillis() + delay;
    this.duration = duration;
    this.delay = delay;
    this.endTime = startTime + duration;
    this.endValue = endValue;
  }

  public int getType() {
    return type;
  }

  public void cancel() {
    canceled = true;
  }

  public T getStartValue() {
    return startValue;
  }

  void setStartValue(T startValue) {
    this.startValue = startValue;
  }

  public T getEndValue() {
    return endValue;
  }

  public boolean isCanceled() {
    return canceled;
  }

  public double getStartTime() {
    return startTime;
  }

  void setStartTime(double startTime) {
    this.startTime = startTime;
  }

  public double getDuration() {
    return duration;
  }

  public double getDelay() {
    return delay;
  }

  public double getEndTime() {
    return endTime;
  }

  abstract int getCameraProperty();

  abstract T onFrame(double currentTime);

  boolean isFinishing() {
    return isFinishing;
  }

  void setFinishing() {
    isFinishing = true;
  }

  void onCancel() {
    // todo camera - notify listeners
  }

  void onFinish() {
    // todo camera - notify listeners
  }
}
