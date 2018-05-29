package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.NonNull;

import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

/**
 * Base class for the different Layer types
 */
public abstract class Layer {

  private long nativePtr;
  private boolean invalidated;

  public Layer(long nativePtr) {
    checkThread();
    this.nativePtr = nativePtr;
  }

  public Layer() {
    checkThread();
  }

  /**
   * Validates if layer interaction is happening on the UI thread
   */
  protected void checkThread() {
    ThreadUtils.checkThread("Layer");
  }

  public void setProperties(@NonNull PropertyValue<?>... properties) {
    checkThread();
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
    checkThread();
    return nativeGetId();
  }

  public PropertyValue<String> getVisibility() {
    checkThread();
    return new PaintPropertyValue<>("visibility", (String) nativeGetVisibility());
  }

  public float getMinZoom() {
    checkThread();
    return nativeGetMinZoom();
  }

  public float getMaxZoom() {
    checkThread();
    return nativeGetMaxZoom();
  }

  public void setMinZoom(float zoom) {
    checkThread();
    nativeSetMinZoom(zoom);
  }

  public void setMaxZoom(float zoom) {
    checkThread();
    nativeSetMaxZoom(zoom);
  }

  @Override
  protected native void finalize() throws Throwable;

  protected native String nativeGetId();

  protected native Object nativeGetVisibility();

  protected native void nativeSetLayoutProperty(String name, Object value);

  protected native void nativeSetPaintProperty(String name, Object value);

  protected native void nativeSetFilter(Object[] filter);

  protected native JsonElement nativeGetFilter();

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
    if (value != null && value instanceof Expression) {
      return ((Expression) value).toArray();
    }
    return value;
  }
}