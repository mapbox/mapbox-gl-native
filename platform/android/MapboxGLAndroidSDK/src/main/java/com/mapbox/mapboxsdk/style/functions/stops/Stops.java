package com.mapbox.mapboxsdk.style.functions.stops;

import android.support.annotation.CallSuper;
import android.support.annotation.NonNull;
import android.support.annotation.Size;

import java.util.HashMap;
import java.util.Map;

/**
 * The base class for different stops implementations
 *
 * @param <I> the input type
 * @param <O> the output type
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#types-function">The style specification</a>
 */
public abstract class Stops<I, O> {

  /**
   * Convenience method for use in function declarations
   *
   * @param stops the collections of discrete stops
   * @param <I>   the Stops input type
   * @param <O>   the Stops output type
   * @return the {@link Stops} implementation
   * @see Stop#stop
   * @see CategoricalStops
   */
  @SafeVarargs
  public static <I, O> CategoricalStops<I, O> categorical(@NonNull @Size(min = 1) Stop<I, O>... stops) {
    return new CategoricalStops<>(stops);
  }

  /**
   * Convenience method for use in function declarations
   *
   * @param stops the collections of discrete stops
   * @param <I>   the Stops input type
   * @param <O>   the Stops output type
   * @return the {@link Stops} implementation
   * @see Stop#stop
   * @see ExponentialStops
   */
  @SafeVarargs
  public static <I, O> ExponentialStops<I, O> exponential(@NonNull @Size(min = 1) Stop<I, O>... stops) {
    return new ExponentialStops<>(stops);
  }

  /**
   * Convenience method for use in function declarations
   *
   * @param <T> the Stops input/output type
   * @return the {@link IdentityStops} implementation
   * @see Stop#stop
   * @see IdentityStops
   */
  public static <T> IdentityStops<T> identity() {
    return new IdentityStops<>();
  }

  /**
   * Convenience method for use in function declarations
   *
   * @param stops the collections of discrete stops
   * @param <I>   the Stops input type
   * @param <O>   the Stops output type
   * @return the {@link Stops} implementation
   * @see Stop#stop
   * @see IntervalStops
   */
  @SafeVarargs
  public static <I, O> IntervalStops<I, O> interval(@NonNull @Size(min = 1) Stop<I, O>... stops) {
    return new IntervalStops<>(stops);
  }

  /**
   * INTERNAL USAGE ONLY
   *
   * @return the value object representation for conversion to core
   */
  @CallSuper
  public Map<String, Object> toValueObject() {
    HashMap<String, Object> map = new HashMap<>();
    map.put("type", getTypeName());
    return map;
  }

  /**
   * INTERNAL USAGE ONLY
   *
   * @return the unique type name as a string according to the style specification
   */
  protected abstract String getTypeName();

  @Override
  public String toString() {
    return getTypeName();
  }
}
