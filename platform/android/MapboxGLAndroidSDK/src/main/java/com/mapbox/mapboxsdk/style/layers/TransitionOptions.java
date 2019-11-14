package com.mapbox.mapboxsdk.style.layers;

import androidx.annotation.Keep;

/**
 * Resembles transition property from the style specification.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#transition">Transition documentation</a>
 */
public class TransitionOptions {

  @Keep
  private long duration;
  @Keep
  private long delay;
  @Keep
  private boolean enablePlacementTransitions;

  /**
   * Create a transition property based on duration and a delay.
   *
   * @param duration the duration of the transition
   * @param delay    the delay to start the transition
   */
  public TransitionOptions(long duration, long delay) {
    this(duration, delay, true);
  }

  /**
   * Create a transition property.
   *
   * @param duration                   the duration of the transition
   * @param delay                      the delay to start the transition
   * @param enablePlacementTransitions the flag that describes whether the fade in/out symbol placement transition
   *                                   should be enabled. Defaults to true.
   */
  public TransitionOptions(long duration, long delay, boolean enablePlacementTransitions) {
    this.duration = duration;
    this.delay = delay;
    this.enablePlacementTransitions = enablePlacementTransitions;
  }

  /**
   * Create a transition property based on duration and a delay.
   *
   * @param duration the duration of the transition
   * @param delay    the delay to start the transition
   * @return a new transition property object
   * @deprecated use {@link #fromTransitionOptions(long, long, boolean)} instead
   */
  @Keep
  @Deprecated
  public static TransitionOptions fromTransitionOptions(long duration, long delay) {
    // Invoked from JNI only
    return new TransitionOptions(duration, delay);
  }

  /**
   * Create a transition property.
   *
   * @param duration                   the duration of the transition
   * @param delay                      the delay to start the transition
   * @param enablePlacementTransitions the flag that describes whether the fade in/out symbol placement transition
   *                                   should be enabled. Defaults to true.
   * @return a new transition property object
   */
  @Keep
  static TransitionOptions fromTransitionOptions(long duration, long delay, boolean enablePlacementTransitions) {
    // Invoked from JNI only
    return new TransitionOptions(duration, delay, enablePlacementTransitions);
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

  /**
   * Get the flag that describes whether the fade in/out symbol placement transition should be enabled.
   *
   * @return true if the fade in/out symbol placement transition should be enabled, false otherwise
   */
  public boolean isEnablePlacementTransitions() {
    return enablePlacementTransitions;
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
    if (delay != that.delay) {
      return false;
    }
    return enablePlacementTransitions == that.enablePlacementTransitions;
  }

  @Override
  public int hashCode() {
    int result = (int) (duration ^ (duration >>> 32));
    result = 31 * result + (int) (delay ^ (delay >>> 32));
    result = 31 * result + (enablePlacementTransitions ? 1 : 0);
    return result;
  }

  @Override
  public String toString() {
    return "TransitionOptions{"
      + "duration=" + duration
      + ", delay=" + delay
      + ", enablePlacementTransitions=" + enablePlacementTransitions
      + '}';
  }
}
