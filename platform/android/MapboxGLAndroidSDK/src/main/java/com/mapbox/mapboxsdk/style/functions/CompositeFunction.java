package com.mapbox.mapboxsdk.style.functions;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.style.functions.stops.CategoricalStops;
import com.mapbox.mapboxsdk.style.functions.stops.ExponentialStops;
import com.mapbox.mapboxsdk.style.functions.stops.IntervalStops;
import com.mapbox.mapboxsdk.style.functions.stops.Stop;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;
import com.mapbox.mapboxsdk.style.layers.PropertyValue;

import java.util.Map;

/**
 * Composite functions combine Camera and SourceFunctions.
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
public class CompositeFunction<Z extends Number, I, O> extends Function<Stop.CompositeValue<Z, I>, O> {

  private final String property;
  private PropertyValue<O> defaultValue;

  CompositeFunction(@NonNull String property,
                    @NonNull CategoricalStops<Stop.CompositeValue<Z, I>, O> stops) {
    this(null, property, stops);
  }

  CompositeFunction(@NonNull String property,
                    @NonNull ExponentialStops<Stop.CompositeValue<Z, I>, O> stops) {
    this(null, property, stops);
  }

  CompositeFunction(@NonNull String property,
                    @NonNull IntervalStops<Stop.CompositeValue<Z, I>, O> stops) {
    this(null, property, stops);
  }


  /**
   * JNI Constructor
   */
  private CompositeFunction(@Nullable O defaultValue, @NonNull String property,
                            @NonNull Stops<Stop.CompositeValue<Z, I>, O> stops) {
    super(stops);
    this.defaultValue = new PropertyValue<>(property, defaultValue);
    this.property = property;
  }

  /**
   * Set the default value
   *
   * @param defaultValue the default value to use when no other applies
   * @return this (for chaining)
   */
  public CompositeFunction<Z, I, O> withDefaultValue(PropertyValue<O> defaultValue) {
    this.defaultValue = defaultValue;
    return this;
  }

  /**
   * @return the defaultValue
   */
  @Nullable
  public PropertyValue<O> getDefaultValue() {
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
      valueObject.put(DEFAULT_VALUE_KEY, defaultValue.value);
    }
    return valueObject;
  }

}
