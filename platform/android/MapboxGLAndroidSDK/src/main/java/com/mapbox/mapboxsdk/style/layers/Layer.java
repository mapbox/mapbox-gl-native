package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.functions.Function;

/**
 * Base class for the different Layer types
 */
public abstract class Layer {

  private long nativePtr;
  private boolean invalidated;

  public Layer(long nativePtr) {
    this.nativePtr = nativePtr;
  }

  public Layer() {
  }

  public void setProperties(@NonNull PropertyValue<?>... properties) {
    if (properties.length == 0) {
      return;
    }

    for (PropertyValue<?> property : properties) {
      Object converted = convertValue(property.value);
      if (property instanceof PaintPropertyValue) {
        nativeSetPaintProperty(property.name, converted);
      } else {
        nativeSetLayoutProperty(property.name, converted);
      }
    }
  }

  public String getId() {
    return nativeGetId();
  }

  public PropertyValue<String> getVisibility() {
    return new PaintPropertyValue<>("visibility", (String) nativeGetVisibility());
  }

  public float getMinZoom() {
    return nativeGetMinZoom();
  }

  public float getMaxZoom() {
    return nativeGetMaxZoom();
  }

  public void setMinZoom(float zoom) {
    nativeSetMinZoom(zoom);
  }

  public void setMaxZoom(float zoom) {
    nativeSetMaxZoom(zoom);
  }

  @Override
  protected native void finalize() throws Throwable;

  protected native String nativeGetId();

  protected native Object nativeGetVisibility();

  protected native void nativeSetLayoutProperty(String name, Object value);

  protected native void nativeSetPaintProperty(String name, Object value);

  protected native void nativeSetFilter(Object[] filter);

  protected native void nativeSetSourceLayer(String sourceLayer);

  protected native String nativeGetSourceLayer();

  protected native float nativeGetMinZoom();

  protected native float nativeGetMaxZoom();

  protected native void nativeSetMinZoom(float zoom);

  protected native void nativeSetMaxZoom(float zoom);

  public long getNativePtr() {
    return nativePtr;
  }

  private Object convertValue(Object value) {
    if (value != null) {
      if (value instanceof Function) {
        return ((Function) value).toValueObject();
      } else if (value instanceof Expression) {
        return ((Expression) value).toArray();
      }
    }
    return value;
  }

}