package com.mapbox.mapboxsdk.style.functions;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.style.functions.stops.CategoricalStops;
import com.mapbox.mapboxsdk.style.functions.stops.ExponentialStops;
import com.mapbox.mapboxsdk.style.functions.stops.IdentityStops;
import com.mapbox.mapboxsdk.style.functions.stops.IntervalStops;
import com.mapbox.mapboxsdk.style.functions.stops.Stop;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;

import java.util.Map;

import timber.log.Timber;

/**
 * Functions are used to change properties in relation to the state of the map.
 * <p>
 * The value for any layout or paint property may be specified as a function. Functions allow you to
 * make the appearance of a map feature change with the current zoom level and/or the feature's properties.
 *
 * @param <I> the function's input type
 * @param <O> the target property's value type. Make sure it matches.
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#types-function">The style specification</a>
 */
public class Function<I, O> {

  static final String PROPERTY_KEY = "property";
  static final String DEFAULT_VALUE_KEY = "default";

  /**
   * Create an exponential {@link CameraFunction}
   * <p>
   * Zoom functions allow the appearance of a map feature to change with map’s zoom.
   * Zoom functions can be used to create the illusion of depth and control data density.
   * Each stop is an array with two elements, the first is a zoom and the second is a function output value.
   *
   * @param <Z>   the zoom level type (Float, Integer)
   * @param <O>   the property type
   * @param stops the stops implementation that define the function. @see {@link Stops#exponential(Stop[])}
   * @return the {@link CameraFunction}
   * @see CameraFunction
   * @see ExponentialStops
   * @see Stops#exponential(Stop[])
   * @see Stops#exponential(Stop[])
   * @see Stop#stop
   */
  public static <Z extends Number, O> CameraFunction<Z, O> zoom(@NonNull ExponentialStops<Z, O> stops) {
    return new CameraFunction<>(stops);
  }

  /**
   * Create an interval {@link CameraFunction}
   * <p>
   * Zoom functions allow the appearance of a map feature to change with map’s zoom.
   * Zoom functions can be used to create the illusion of depth and control data density.
   * Each stop is an array with two elements, the first is a zoom and the second is a function output value.
   *
   * @param <Z>   the zoom level type (Float, Integer)
   * @param <O>   the property type
   * @param stops the stops implementation that define the function. @see {@link Stops#interval(Stop[])}
   * @return the {@link CameraFunction}
   * @see CameraFunction
   * @see IntervalStops
   * @see Stops#interval(Stop[])
   * @see Stop#stop
   */
  public static <Z extends Number, O> CameraFunction<Z, O> zoom(@NonNull IntervalStops<Z, O> stops) {
    return new CameraFunction<>(stops);
  }

  /**
   * Create an exponential {@link SourceFunction}
   * <p>
   * Source functions allow the appearance of a map feature to change with
   * its properties. Source functions can be used to visually differentiate
   * types of features within the same layer or create data visualizations.
   * Each stop is an array with two elements, the first is a property input
   * value and the second is a function output value. Note that support for
   * property functions is not available across all properties and platforms
   * at this time.
   *
   * @param property the feature property name
   * @param stops    the stops
   * @param <I>      the function input type
   * @param <O>      the function output type
   * @return the {@link SourceFunction}
   * @see SourceFunction
   * @see ExponentialStops
   * @see Stops#exponential(Stop[])
   * @see Stop#stop
   */
  public static <I, O> SourceFunction<I, O> property(@NonNull String property, @NonNull ExponentialStops<I, O> stops) {
    return new SourceFunction<>(property, stops);
  }

  /**
   * Create an identity {@link SourceFunction}
   * <p>
   * Source functions allow the appearance of a map feature to change with
   * its properties. Source functions can be used to visually differentiate
   * types of features within the same layer or create data visualizations.
   * Each stop is an array with two elements, the first is a property input
   * value and the second is a function output value. Note that support for
   * property functions is not available across all properties and platforms
   * at this time.
   *
   * @param property the feature property name
   * @param stops    the stops
   * @param <T>      the function input/output type
   * @return the {@link SourceFunction}
   * @see SourceFunction
   * @see IdentityStops
   * @see Stops#identity()
   * @see Stop#stop
   */
  public static <T> SourceFunction<T, T> property(@NonNull String property, @NonNull IdentityStops<T> stops) {
    return new SourceFunction<>(property, stops);
  }

  /**
   * Create an interval {@link SourceFunction}
   * <p>
   * Source functions allow the appearance of a map feature to change with
   * its properties. Source functions can be used to visually differentiate
   * types of features within the same layer or create data visualizations.
   * Each stop is an array with two elements, the first is a property input
   * value and the second is a function output value. Note that support for
   * property functions is not available across all properties and platforms
   * at this time.
   *
   * @param property the feature property name
   * @param stops    the stops
   * @param <I>      the function input type
   * @param <O>      the function output type
   * @return the {@link SourceFunction}
   * @see SourceFunction
   * @see IntervalStops
   * @see Stops#interval(Stop[])
   * @see Stop#stop
   */
  public static <I, O> SourceFunction<I, O> property(@NonNull String property, @NonNull IntervalStops<I, O> stops) {
    return new SourceFunction<>(property, stops);
  }

  /**
   * Create an categorical {@link SourceFunction}
   * <p>
   * Source functions allow the appearance of a map feature to change with
   * its properties. Source functions can be used to visually differentiate
   * types of features within the same layer or create data visualizations.
   * Each stop is an array with two elements, the first is a property input
   * value and the second is a function output value. Note that support for
   * property functions is not available across all properties and platforms
   * at this time.
   *
   * @param property the feature property name
   * @param stops    the stops
   * @param <I>      the function input type
   * @param <O>      the function output type
   * @return the {@link SourceFunction}
   * @see SourceFunction
   * @see CategoricalStops
   * @see Stops#categorical(Stop[])
   * @see Stop#stop
   */
  public static <I, O> SourceFunction<I, O> property(@NonNull String property, @NonNull CategoricalStops<I, O> stops) {
    return new SourceFunction<>(property, stops);
  }

  /**
   * Create a composite, categorical function.
   * <p>
   * Composite functions allow the appearance of a map feature to change with both its
   * properties and zoom. Each stop is an array with two elements, the first is an object
   * with a property input value and a zoom, and the second is a function output value. Note
   * that support for property functions is not yet complete.
   *
   * @param property the feature property name for the source part of the function
   * @param stops    the stops
   * @param <Z>      the zoom function input type (Float usually)
   * @param <I>      the function input type for the source part of the function
   * @param <O>      the function output type
   * @return the {@link CompositeFunction}
   * @see CompositeFunction
   * @see CategoricalStops
   * @see Stops#categorical(Stop[])
   * @see Stop#stop
   */
  public static <Z extends Number, I, O> CompositeFunction<Z, I, O> composite(
    @NonNull String property,
    @NonNull CategoricalStops<Stop.CompositeValue<Z, I>, O> stops) {

    return new CompositeFunction<>(property, stops);
  }

  /**
   * Create a composite, exponential function.
   * <p>
   * Composite functions allow the appearance of a map feature to change with both its
   * properties and zoom. Each stop is an array with two elements, the first is an object
   * with a property input value and a zoom, and the second is a function output value. Note
   * that support for property functions is not yet complete.
   *
   * @param property the feature property name for the source part of the function
   * @param stops    the stops
   * @param <Z>      the zoom function input type (Float usually)
   * @param <I>      the function input type for the source part of the function
   * @param <O>      the function output type
   * @return the {@link CompositeFunction}
   * @see CompositeFunction
   * @see ExponentialStops
   * @see Stops#exponential(Stop[])
   * @see Stop#stop
   */
  public static <Z extends Number, I, O> CompositeFunction<Z, I, O> composite(
    @NonNull String property,
    @NonNull ExponentialStops<Stop.CompositeValue<Z, I>, O> stops) {

    return new CompositeFunction<>(property, stops);
  }

  /**
   * Create a composite, interval function.
   * <p>
   * Composite functions allow the appearance of a map feature to change with both its
   * properties and zoom. Each stop is an array with two elements, the first is an object
   * with a property input value and a zoom, and the second is a function output value. Note
   * that support for property functions is not yet complete.
   *
   * @param property the feature property name for the source part of the function
   * @param stops    the stops
   * @param <Z>      the zoom function input type (Float usually)
   * @param <I>      the function input type for the source part of the function
   * @param <O>      the function output type
   * @return the {@link CompositeFunction}
   * @see CompositeFunction
   * @see IntervalStops
   * @see Stops#interval(Stop[])
   * @see Stop#stop
   */
  public static <Z extends Number, I, O> CompositeFunction<Z, I, O> composite(
    @NonNull String property,
    @NonNull IntervalStops<Stop.CompositeValue<Z, I>, O> stops) {

    return new CompositeFunction<>(property, stops);
  }

  // Class definition //

  private final Stops<I, O> stops;

  /**
   * JNI Cosntructor for implementation classes
   *
   * @param stops the stops
   */
  Function(@NonNull Stops<I, O> stops) {
    this.stops = stops;
  }

  /**
   * @return the stops in this function
   */
  public Stops getStops() {
    return stops;
  }

  /**
   * Convenience method
   *
   * @param <S> the Stops implementation type
   * @return the Stops implementation or null when the wrong type is specified
   */
  @Nullable
  public <S extends Stops> S getStopsAs() {
    try {
      // noinspection unchecked
      return (S) stops;
    } catch (ClassCastException exception) {
      Timber.e(exception, "Stops: %s is a different type: ", stops.getClass());
      return null;
    }
  }

  /**
   * INTERNAL USAGE ONLY
   *
   * @return a value object representation for core conversion
   */
  public Map<String, Object> toValueObject() {
    return stops.toValueObject();
  }

  @Override
  public String toString() {
    return String.format("%s: %s", getClass().getSimpleName(), stops);
  }
}
