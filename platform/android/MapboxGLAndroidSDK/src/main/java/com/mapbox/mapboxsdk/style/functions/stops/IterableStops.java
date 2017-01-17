package com.mapbox.mapboxsdk.style.functions.stops;

import java.util.Iterator;

/**
 * Base class for {@link Stops} implementations with a collection of stops
 *
 * @param <I> the {@link Stops} input type
 * @param <O> the {@link Stops} output type
 * @param <S> the {@link Iterable} element type (usually {@link Stop})
 */
public abstract class IterableStops<I, O, S> extends Stops<I, O> implements Iterable<S> {

  /**
   * @return The size of the contained stops collection
   */
  public abstract int size();

  /**
   * Convenience function to toValueObjects an array of stops to an array of value objects
   *
   * @param stops the stops to toValueObjects
   * @return the stops as value objects
   */
  Object[] toValueObjects(Stop<I, O>[] stops) {
    if (stops != null) {
      Object[] stopsValue = new Object[stops.length];

      for (int i = 0; i < stopsValue.length; i++) {
        Stop stop = stops[i];
        stopsValue[i] = stop.toValueObject();
      }
      return stopsValue;
    }

    return null;
  }

  @Override
  public String toString() {
    StringBuilder buffer = new StringBuilder();
    Iterator<S> iterator = iterator();
    while (iterator.hasNext()) {
      S stop = iterator.next();
      buffer.append(stop);
      if (iterator.hasNext()) {
        buffer.append(",");
      }
    }
    return String.format("%s: [%s]", super.toString(), buffer.toString());
  }
}
