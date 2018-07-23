package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.Keep;
import android.support.annotation.NonNull;

import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

/**
 * Base class for the different Layer types
 */
public abstract class Layer {

  @Keep
  private long nativePtr;
  @Keep
  private boolean invalidated;

  @Keep
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
  @Keep
  protected native void finalize() throws Throwable;

  @Keep
  protected native String nativeGetId();

  @Keep
  protected native Object nativeGetVisibility();

  @Keep
  protected native void nativeSetLayoutProperty(String name, Object value);

  @Keep
  protected native void nativeSetPaintProperty(String name, Object value);

  @Keep
  protected native void nativeSetFilter(Object[] filter);

  @Keep
  protected native JsonElement nativeGetFilter();

  @Keep
  protected native void nativeSetSourceLayer(String sourceLayer);

  @Keep
  protected native String nativeGetSourceLayer();

  @Keep
  protected native float nativeGetMinZoom();

  @Keep
  protected native float nativeGetMaxZoom();

  @Keep
  protected native void nativeSetMinZoom(float zoom);

  @Keep
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