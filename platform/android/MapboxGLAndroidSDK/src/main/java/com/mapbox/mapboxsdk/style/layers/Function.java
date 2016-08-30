package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.FloatRange;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.Size;

import java.util.HashMap;
import java.util.Map;

/**
 * Representation of <a href="https://www.mapbox.com/mapbox-gl-style-spec/#types-function">Function</a> in the Mapbox style specification
 *
 * @param <T> the target property's value type. Make sure it matches.
 */
public class Function<T> {

    public static class Stop<I, O> {
        public final I in;
        public final O out;

        Stop(I in, O out) {
            this.in = in;
            this.out = out;
        }

        Object[] toValueObject() {
            return new Object[]{in, out};
        }
    }

    @SafeVarargs
    public static <T> Function<T> zoom(@NonNull @Size(min = 1) Stop<Float, T>... stops) {
        return new Function<T>(stops);
    }

    @SafeVarargs
    public static <T> Function<T> zoom(
            @FloatRange(from = 0, to = 1, fromInclusive = false, toInclusive = false) float base,
            @NonNull @Size(min = 1) Stop<Float, T>... stops) {
        return new Function<T>(stops)
                .withBase(base);
    }

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

    @Nullable
    public Float getBase() {
        return base;
    }

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
