package com.mapbox.mapboxsdk.style.functions;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.style.functions.stops.CompositeStops;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;

import java.util.Map;

/**
 * Composite functions combine {@link android.graphics.Camera} and {@link SourceFunction}s.
 * <p>
 * Composite functions allow the appearance of a map feature to change with both its
 * properties and zoom. Each stop is an array with two elements, the first is an object
 * with a property input value and a zoom, and the second is a function output value. Note
 * that support for property functions is not yet complete.
 *
 * @param <Z> the zoom type (usually Float)
 * @param <I> the input type (the feature property type)
 * @param <O> the output type (the property type)
 * @see Function#composite
 */
public class CompositeFunction<Z extends Number, I, O> extends Function<I, O> {

  private final String property;
  private O defaultValue;

  CompositeFunction(@NonNull String property,
                    @NonNull CompositeStops<Z, I, O, ? extends Stops<I, O>> stops) {
    this(null, property, stops);
  }


  /**
   * JNI Constructor
   */
  private CompositeFunction(@Nullable O defaultValue, @NonNull String property,
                            @NonNull CompositeStops<Z, I, O, ? extends Stops<I, O>> stops) {
    super(stops);
    this.defaultValue = defaultValue;
    this.property = property;
  }

  /**
   * Set the default value
   *
   * @param defaultValue the default value to use when no other applies
   * @return this (for chaining)
   */
  public CompositeFunction<Z, I, O> withDefaultValue(O defaultValue) {
    this.defaultValue = defaultValue;
    return this;
  }

  /**
   * @return the defaultValue
   */
  @Nullable
  public O getDefaultValue() {
    return defaultValue;
  }

  /**
   * INTERNAL USAGE ONLY
   *
   * @return the feature property name
   */
  public String getProperty() {
    return property;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Map<String, Object> toValueObject() {
    Map<String, Object> valueObject = super.toValueObject();
    valueObject.put(PROPERTY_KEY, property);
    if (defaultValue != null) {
      valueObject.put(DEFAULT_VALUE_KEY, defaultValue);
    }
    return valueObject;
  }

}
