package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.NonNull;

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

    public void setProperties(@NonNull Property<?>... properties) {
        checkValidity();

        if (properties.length == 0) {
            return;
        }

        boolean updateClasses = false;
        for (Property<?> property : properties) {
            Object converted = convertValue(property.value);
            if (property instanceof PaintProperty) {
                updateClasses = true;
                nativeSetPaintProperty(property.name, converted);
            } else {
                nativeSetLayoutProperty(property.name, converted);
            }
        }

        nativeUpdateStyle(updateClasses);
    }

    public String getId() {
        checkValidity();
        return nativeGetId();
    }

    public PropertyValue<String> getVisibility() {
        checkValidity();
        return new PropertyValue<>(nativeGetVisibility());
    }

    public float getMinZoom() {
        checkValidity();
        return nativeGetMinZoom();
    }

    public float getMaxZoom() {
        checkValidity();
        return nativeGetMaxZoom();
    }

    public void setMinZoom(float zoom) {
        checkValidity();
        nativeSetMinZoom(zoom);
    }

    public void setMaxZoom(float zoom) {
        checkValidity();
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

    protected native void nativeUpdateStyle(boolean updateClasses);

    protected native float nativeGetMinZoom();

    protected native float nativeGetMaxZoom();

    protected native void nativeSetMinZoom(float zoom);

    protected native void nativeSetMaxZoom(float zoom);

    public long getNativePtr() {
        return nativePtr;
    }

    private Object convertValue(Object value) {
        return value != null && value instanceof Function ? ((Function) value).toValueObject() : value;
    }

    protected void checkValidity() {
        if (invalidated) {
            throw new RuntimeException("Layer has been invalidated. Request a new reference after adding");
        }
    }

    public final void invalidate() {
        this.invalidated = true;
    }
}
