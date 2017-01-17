package com.mapbox.mapboxsdk.style.functions.stops;

import android.support.annotation.NonNull;
import android.support.annotation.Size;

import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;

/**
 * The {@link Stops} implementation for exponential functions
 *
 * @param <I> the input type
 * @param <O> the output type
 */
public class ExponentialStops<I, O> extends IterableStops<I, O, Stop<I, O>> {

  private float base;
  private final Stop<I, O>[] stops;

  /**
   * Create exponential stops with an explicit base. Use through {@link Stops#exponential(Stop[])}
   *
   * @param base  The exponential base of the interpolation curve. It controls the rate at which the function output
   *              increases. Higher values make the output increase more towards the high end of the range.
   *              With values close to 1 the output increases linearly.
   * @param stops the stops
   */
  @SafeVarargs
  public ExponentialStops(Float base, @NonNull @Size(min = 1) Stop<I, O>... stops) {
    this.base = base != null ? base : 1.0f;
    this.stops = stops;
  }

  /**
   * Create exponential stops without an explicit base. Use through {@link Stops#exponential(Stop[])}
   *
   * @param stops the stops
   */
  @SafeVarargs
  public ExponentialStops(@NonNull @Size(min = 1) Stop<I, O>... stops) {
    this(null, stops);
  }

  /**
   * Set the exponential base
   *
   * @param base the base to use in the exponential function
   * @return this (for chaining)
   */
  public ExponentialStops<I, O> withBase(float base) {
    this.base = base;
    return this;
  }

  /**
   * @return The exponential base
   */
  public float getBase() {
    return base;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Map<String, Object> toValueObject() {
    Map<String, Object> map = super.toValueObject();
    map.put("base", base);
    map.put("stops", toValueObjects(stops));
    return map;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String getTypeName() {
    return "exponential";
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Iterator<Stop<I, O>> iterator() {
    return Arrays.asList(stops).iterator();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public int size() {
    return stops.length;
  }
}
