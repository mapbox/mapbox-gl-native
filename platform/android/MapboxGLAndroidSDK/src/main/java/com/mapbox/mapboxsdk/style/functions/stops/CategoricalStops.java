package com.mapbox.mapboxsdk.style.functions.stops;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.Size;

import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;

/**
 * {@link Stops} implementation for categorical functions
 *
 * @param <I> the {@link Stop} input type
 * @param <O> the {@link Stop} output type
 */
public class CategoricalStops<I, O> extends IterableStops<I, O, Stop<I, O>> {

  private final Stop<I, O>[] stops;
  private I defaultValue;

  /**
   * Create a categorical {@link Stops} implementation. Use through {@link Stops#categorical(Stop[])}
   *
   * @param stops the stops
   */
  @SafeVarargs
  public CategoricalStops(@NonNull @Size(min = 1) Stop<I, O>... stops) {
    this(null, stops);
  }

  /**
   * Create a categorical {@link Stops} implementation. Use through {@link Stops#categorical(Stop[])}
   *
   * @param stops the stops
   */
  @SafeVarargs
  public CategoricalStops(I defaultValue, @NonNull @Size(min = 1) Stop<I, O>... stops) {
    this.defaultValue = defaultValue;
    this.stops = stops;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Map<String, Object> toValueObject() {
    Map<String, Object> map = super.toValueObject();
    map.put("stops", toValueObjects(stops));
    if (defaultValue != null) {
      map.put("defaultValue", defaultValue);
    }
    return map;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String getTypeName() {
    return "categorical";
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

  /**
   * Set the default value
   *
   * @param defaultValue the default value to use when no other applies
   * @return this (for chaining)
   */
  public CategoricalStops<I, O> withDefaultValue(I defaultValue) {
    this.defaultValue = defaultValue;
    return this;
  }

  /**
   * @return the default value or null
   */
  @Nullable
  public I getDefaultValue() {
    return defaultValue;
  }
}
