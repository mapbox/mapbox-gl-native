package com.mapbox.mapboxsdk.style.layers;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.types.Formatted;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

/**
 * Base class for the different Layer types
 */
public abstract class Layer {

  private final static String TAG = "Mbgl-Layer";

  @Keep
  private long nativePtr;
  @Keep
  private boolean invalidated;
  private boolean detached;

  static {
    LibraryLoader.load();
  }

  @Keep
  protected Layer(long nativePtr) {
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
    ThreadUtils.checkThread(TAG);
  }

  public void setProperties(@NonNull PropertyValue<?>... properties) {
    if (detached) {
      return;
    }

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

  @NonNull
  public String getId() {
    checkThread();
    return nativeGetId();
  }

  @NonNull
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

  @NonNull
  @Keep
  protected native String nativeGetId();

  @NonNull
  @Keep
  protected native Object nativeGetVisibility();

  @Keep
  protected native void nativeSetLayoutProperty(String name, Object value);

  @Keep
  protected native void nativeSetPaintProperty(String name, Object value);

  @Keep
  protected native void nativeSetFilter(Object[] filter);

  @Nullable
  @Keep
  protected native JsonElement nativeGetFilter();

  @Keep
  protected native void nativeSetSourceLayer(String sourceLayer);

  @NonNull
  @Keep
  protected native String nativeGetSourceLayer();

  @NonNull
  @Keep
  protected native String nativeGetSourceId();

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

  @Nullable
  private Object convertValue(@Nullable Object value) {
    if (value instanceof Expression) {
      return ((Expression) value).toArray();
    } else if (value instanceof Formatted) {
      return ((Formatted) value).toArray();
    } else {
      return value;
    }
  }

  public void setDetached() {
    detached = true;
  }

  public boolean isDetached() {
    return detached;
  }
}