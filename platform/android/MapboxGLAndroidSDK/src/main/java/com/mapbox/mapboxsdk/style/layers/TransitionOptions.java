package com.mapbox.mapboxsdk.style.layers;

/**
 * Resembles transition property from the style specification.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#transition">Transition documentation</a>
 */
public class TransitionOptions {

  private long duration;
  private long delay;

  /**
   * Create a transition property based on duration and a delay.
   *
   * @param duration the duration of the transition
   * @param delay    the delay to start the transition
   */
  public TransitionOptions(long duration, long delay) {
    this.duration = duration;
    this.delay = delay;
  }

  /**
   * Create a transition property based on duration and a delay.
   *
   * @param duration the duration of the transition
   * @param delay    the delay to start the transition
   * @return a new transition property object
   */
  public static TransitionOptions fromTransitionOptions(long duration, long delay) {
    return new TransitionOptions(duration, delay);
  }

  /**
   * Get the transition duration.
   *
   * @return the transition duration
   */
  public long getDuration() {
    return duration;
  }

  /**
   * Get the transition delay.
   *
   * @return the transition delay
   */
  public long getDelay() {
    return delay;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    TransitionOptions that = (TransitionOptions) o;

    if (duration != that.duration) {
      return false;
    }
    return delay == that.delay;
  }

  @Override
  public int hashCode() {
    int result = (int) (duration ^ (duration >>> 32));
    result = 31 * result + (int) (delay ^ (delay >>> 32));
    return result;
  }

  @Override
  public String toString() {
    return "TransitionOptions{"
      + "duration=" + duration
      + ", delay=" + delay
      + '}';
  }
}
