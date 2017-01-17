package com.mapbox.mapboxsdk.style.functions.stops;

import android.support.annotation.NonNull;
import android.support.annotation.Size;

import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;

/**
 * The {@link Stops} implementation for interval functions
 *
 * @param <I> the input type
 * @param <O> the output type
 */
public class IntervalStops<I, O> extends IterableStops<I, O, Stop<I, O>> {

  private final Stop<I, O>[] stops;

  @SafeVarargs
  public IntervalStops(@NonNull @Size(min = 1) Stop<I, O>... stops) {
    this.stops = stops;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String getTypeName() {
    return "interval";
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Map<String, Object> toValueObject() {
    Map<String, Object> map = super.toValueObject();
    map.put("stops", toValueObjects(stops));
    return map;
  }

  /**
   * @return an {@link Iterator} for the contained stops
   */
  @Override
  public Iterator<Stop<I, O>> iterator() {
    return Arrays.asList(stops).iterator();
  }

  /**
   * @return The number of contained stops
   */
  @Override
  public int size() {
    return stops.length;
  }
}
