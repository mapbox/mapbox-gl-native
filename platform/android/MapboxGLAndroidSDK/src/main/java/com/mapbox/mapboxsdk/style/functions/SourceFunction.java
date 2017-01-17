package com.mapbox.mapboxsdk.style.functions;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.style.functions.stops.Stops;

import java.util.Map;

/**
 * Source functions take Feature property names as input.
 * <p>
 * Source functions allow the appearance of a map feature to change with
 * its properties. Source functions can be used to visually differentiate
 * types of features within the same layer or create data visualizations.
 * Each stop is an array with two elements, the first is a property input
 * value and the second is a function output value. Note that support for
 * property functions is not available across all properties and platforms
 * at this time.
 *
 * @param <V> the input type
 * @param <T> the output type
 * @see Function#property
 */
public class SourceFunction<V, T> extends Function<V, T> {

  private final String property;

  SourceFunction(@NonNull String property, @NonNull Stops<V, T> stops) {
    super(stops);
    this.property = property;
  }

  /**
   * INTERNAL USAGE ONLY
   *
   * @return The feature property name
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
    valueObject.put("property", property);
    return valueObject;
  }
}
