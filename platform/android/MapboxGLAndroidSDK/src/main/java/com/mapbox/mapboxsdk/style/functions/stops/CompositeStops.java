package com.mapbox.mapboxsdk.style.functions.stops;

import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Size;

import com.mapbox.mapboxsdk.style.functions.stops.Stop.CompositeValue;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * The {@link Stops} implementation for composite functions
 *
 * @param <Z> the zoom type (usually Float)
 * @param <I> the input type (the feature property type)
 * @param <O> the output type (the property type)
 * @param <S> the {@link Stops} implementation (eg CategoricalStops, {@link ExponentialStops} or {@link IntervalStops})
 */
public class CompositeStops<Z extends Number, I, O, S extends IterableStops<I, O, Stop<I, O>>>
  extends IterableStops<CompositeValue<Z, I>, O, Map.Entry<Z, S>> {

  private final Map<Z, S> stops;

  /**
   * JNI Constructor
   *
   * @param stops the stops {@link Map}
   */
  @Keep
  private CompositeStops(@NonNull @Size(min = 1) Map<Z, S> stops) {
    this.stops = stops;
  }

  /**
   * Create composite stops for {@link ExponentialStops}. Use
   * {@link com.mapbox.mapboxsdk.style.functions.Function#composite(String, ExponentialStops)}
   *
   * @param stops the stops
   */
  public CompositeStops(@NonNull ExponentialStops<Stop.CompositeValue<Z, I>, O> stops) {
    this.stops = new HashMap<>();

    for (Map.Entry<Z, List<Stop<I, O>>> entry : collect(stops).entrySet()) {
      // noinspection unchecked
      this.stops.put(entry.getKey(),
        (S) new ExponentialStops<>(stops.getBase(), entry.getValue().toArray(new Stop[0])));
    }
  }

  /**
   * Create composite stops for {@link IntervalStops}.
   * Use {@link com.mapbox.mapboxsdk.style.functions.Function#composite(String, IntervalStops)}
   *
   * @param stops the stops
   */
  public CompositeStops(@NonNull IntervalStops<Stop.CompositeValue<Z, I>, O> stops) {
    this.stops = new HashMap<>();

    for (Map.Entry<Z, List<Stop<I, O>>> entry : collect(stops).entrySet()) {
      // noinspection unchecked
      this.stops.put(entry.getKey(), (S) new IntervalStops<>(entry.getValue().toArray(new Stop[0])));
    }
  }

  /**
   * Create composite stops for {@link CategoricalStops}.
   * Use {@link com.mapbox.mapboxsdk.style.functions.Function#composite(String, CategoricalStops)}
   *
   * @param stops the stops
   */
  public CompositeStops(@NonNull CategoricalStops<CompositeValue<Z, I>, O> stops) {
    this.stops = new HashMap<>();

    for (Map.Entry<Z, List<Stop<I, O>>> entry : collect(stops).entrySet()) {
      // noinspection unchecked
      this.stops.put(entry.getKey(), (S) new CategoricalStops<>(entry.getValue().toArray(new Stop[0])));
    }
  }

  /**
   * {@inheritDoc}
   */
  @Override
  protected String getTypeName() {
    return stops.values().iterator().next().getTypeName();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Map<String, Object> toValueObject() {
    Map<String, Object> map = super.toValueObject();

    // Flatten and toValueObjects stops
    // noinspection unchecked
    map.put("stops", toValueObjects(flatten(this.stops).toArray(new Stop[0])));

    return map;
  }

  @NonNull
  private List<Stop<CompositeValue<Z, I>, O>> flatten(Map<Z, S> stops) {
    List<Stop<CompositeValue<Z, I>, O>> flattenedStops = new ArrayList<>();
    for (Map.Entry<Z, S> entry : stops.entrySet()) {
      for (Stop<I, O> stop : entry.getValue()) {
        flattenedStops.add(new Stop<>(new CompositeValue<>(entry.getKey(), stop.in), stop.out));
      }
    }
    return flattenedStops;
  }

  @NonNull
  private Map<Z, List<Stop<I, O>>> collect(
    @NonNull IterableStops<CompositeValue<Z, I>, O, Stop<CompositeValue<Z, I>, O>> stops) {
    Map<Z, List<Stop<I, O>>> converted = new HashMap<>();

    for (Stop<CompositeValue<Z, I>, O> stop : stops) {
      List<Stop<I, O>> stopsForZ = converted.get(stop.in.zoom);
      if (stopsForZ == null) {
        stopsForZ = new ArrayList<>();
        converted.put(stop.in.zoom, stopsForZ);
      }

      stopsForZ.add(new Stop<>(stop.in.value, stop.out));
    }

    return converted;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Iterator<Map.Entry<Z, S>> iterator() {
    return stops.entrySet().iterator();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public int size() {
    return stops.size();
  }

}
