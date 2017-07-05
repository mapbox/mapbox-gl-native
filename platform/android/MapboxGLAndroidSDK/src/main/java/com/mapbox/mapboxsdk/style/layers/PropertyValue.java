package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.exceptions.ConversionException;
import com.mapbox.mapboxsdk.style.functions.Function;
import com.mapbox.mapboxsdk.utils.ColorUtils;

import timber.log.Timber;

/**
 * Properties for Layer
 */
public class PropertyValue<T> {

  public final String name;
  public final T value;

  /**
   * Not part of the public API.
   *
   * @param name  the property name
   * @param value the property value
   * @see PropertyFactory for construction of {@link PropertyValue}s
   */
  public PropertyValue(@NonNull String name, T value) {
    this.name = name;
    this.value = value;
  }

  /**
   * Returns if this is null
   *
   * @return true if this is null, false if not
   */
  public boolean isNull() {
    return value == null;
  }

  /**
   * Returns if this is a function.
   *
   * @return true if is a function, false if not
   */
  public boolean isFunction() {
    return !isNull() && value instanceof Function;
  }

  /**
   * Returns if this is a value.
   *
   * @return true if is a value, false if not
   */
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

  /**
   * Get the value of the property.
   *
   * @return the property value
   */
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

  /**
   * Get the color int value of the property if the value is a color.
   *
   * @return the color int value of the property, null if not a color value
   */
  @ColorInt
  @Nullable
  public Integer getColorInt() {
    if (!isValue() || !(value instanceof String)) {
      Timber.e("%s is not a String value and can not be converted to a color it", name);
      return null;
    }

    try {
      return ColorUtils.rgbaToColor((String) value);
    } catch (ConversionException ex) {
      Timber.e("%s could not be converted to a Color int: %s", name, ex.getMessage());
      return null;
    }
  }

  /**
   * Get the string representation of a property value.
   *
   * @return the string representation
   */
  @Override
  public String toString() {
    return String.format("%s: %s", name, value);
  }
}
