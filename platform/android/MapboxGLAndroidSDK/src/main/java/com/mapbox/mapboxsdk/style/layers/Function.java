package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.Size;

import java.util.HashMap;
import java.util.Map;

/**
 * Functions are used to change properties in relation to the state of the map.
 * <p>
 * Currently, only zoom functions are supported.
 * </p>
 *
 * @param <T> the target property's value type. Make sure it matches.
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#types-function">The online documentation</a>
 */
public class Function<T> {

  /**
   * A stop represents a certain point in the range of this function
   *
   * @param <I> input
   * @param <O> output
   */
  public static class Stop<I, O> {
    public final I in;
    public final O out;

    Stop(I in, O out) {
      this.in = in;
      this.out = out;
    }

    /**
     * @return an array representation of the Stop
     */
    Object[] toValueObject() {
      return new Object[] {in, out};
    }

    @Override
    public String toString() {
      return String.format("[%s, %s]", in, out);
    }
  }

  /**
   * Zoom functions allow the appearance of a map feature to change with map’s zoom.
   * Zoom functions can be used to create the illusion of depth and control data density.
   * Each stop is an array with two elements, the first is a zoom and the second is a function output value.
   *
   * @param stops the stops that define the function
   * @param <T>   the property type
   * @return the {@link Function}
   */
  @SafeVarargs
  public static <T> Function<T> zoom(@NonNull @Size(min = 1) Stop<Float, T>... stops) {
    return new Function<T>(stops);
  }


  /**
   * Zoom functions allow the appearance of a map feature to change with map’s zoom.
   * Zoom functions can be used to create the illusion of depth and control data density.
   * Each stop is an array with two elements, the first is a zoom and the second is a function output value.
   *
   * @param stops the stops that define the function
   * @param base  the exponential base of the interpolation curve - Default 1
   * @param <T>   the property type
   * @return the {@link Function}
   */
  @SafeVarargs
  public static <T> Function<T> zoom(
    @FloatRange(from = 0, to = 1, fromInclusive = false, toInclusive = false) float base,
    @NonNull @Size(min = 1) Stop<Float, T>... stops) {
    return new Function<T>(stops)
      .withBase(base);
  }

  /**
   * Creates a stop to use in a {@link Function}
   *
   * @param in     the input for the stop
   * @param output the output for the stop
   * @param <T>    the output property type
   * @return the {@link Stop}
   */
  public static <T> Stop<Float, T> stop(float in, Property<T> output) {
    return new Stop<>(in, output.value);
  }

  private final Stop<Float, T>[] stops;
  private Float base;

  Function(@NonNull @Size(min = 1) Stop<Float, T>[] stops) {
    this.stops = stops;
  }

  Function<T> withBase(float base) {
    this.base = base;
    return this;
  }

  /**
   * @return the base
   */
  @Nullable
  public Float getBase() {
    return base;
  }

  /**
   * @return the stops in this function
   */
  public Stop<Float, T>[] getStops() {
    return stops;
  }

  Map<String, Object> toValueObject() {
    Object[] stopsValue = new Object[stops.length];

    for (int i = 0; i < stopsValue.length; i++) {
      Stop stop = stops[i];
      stopsValue[i] = stop.toValueObject();
    }

    Map<String, Object> value = new HashMap<>();
    if (base != null) {
      value.put("base", base);
    }
    value.put("stops", stopsValue);
    return value;
  }
}
