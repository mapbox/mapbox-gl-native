package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.google.gson.JsonArray;
import com.mapbox.mapboxsdk.exceptions.ConversionException;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.utils.ColorUtils;

/**
 * Properties for Layer
 */
public class PropertyValue<T> {

  private static final String TAG = "Mbgl-PropertyValue";

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
   * Returns if this is a expression.
   *
   * @return true if this is a expression, false if not
   */
  public boolean isExpression() {
    return !isNull() && value instanceof JsonArray;
  }

  /**
   * Get the expression of the property.
   *
   * @return the property expression
   */
  @Nullable
  public Expression getExpression() {
    if (isExpression()) {
      return Expression.Converter.convert((JsonArray) value);
    } else {
      Logger.w(TAG, "not a expression, try value");
      return null;
    }
  }

  /**
   * Returns if this is a value.
   *
   * @return true if is a value, false if not
   */
  public boolean isValue() {
    return !isNull() && !isExpression();
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
      return value;
    } else {
      Logger.w(TAG, "not a value, try function");
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
      Logger.e(TAG, String.format("%s is not a String value and can not be converted to a color it", name));
      return null;
    }

    try {
      return ColorUtils.rgbaToColor((String) value);
    } catch (ConversionException ex) {
      Logger.e(TAG, String.format("%s could not be converted to a Color int: %s", name, ex.getMessage()));
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