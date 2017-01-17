package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.style.functions.Function;

import timber.log.Timber;

/**
 * Properties for Layer
 */
public class PropertyValue<T> {

  public final String name;
  public final T value;

  /* package */ PropertyValue(@NonNull String name, T value) {
    this.name = name;
    this.value = value;
  }

  public boolean isNull() {
    return value == null;
  }

  public boolean isFunction() {
    return !isNull() && value instanceof Function;
  }

  public boolean isValue() {
    return !isNull() && !isFunction();
  }

  @Nullable
  public Function<?, T> getFunction() {
    if (isFunction()) {
      // noinspection unchecked
      return (Function<?, T>) value;
    } else {
      Timber.w("not a function, try value");
      return null;
    }
  }

  @Nullable
  public T getValue() {
    if (isValue()) {
      // noinspection unchecked
      return (T) value;
    } else {
      Timber.w("not a value, try function");
      return null;
    }
  }

  @Override
  public String toString() {
    return String.format("%s: %s", name, value);
  }
}
