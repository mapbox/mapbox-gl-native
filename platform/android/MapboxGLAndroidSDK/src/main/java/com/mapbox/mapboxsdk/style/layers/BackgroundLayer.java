// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

/**
 * Background Layer
 */
public class BackgroundLayer extends Layer {

    public BackgroundLayer(long nativePtr) {
        super(nativePtr);
    }

    public BackgroundLayer(String layerId) {
        initialize(layerId);
    }

    protected native void initialize(String layerId);


    public BackgroundLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getBackgroundColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetBackgroundColor());
    }
      /**
      * The color with which the background will be drawn.
      * @throws RuntimeException
      */
    @ColorInt
    public int getBackgroundColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getBackgroundColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("background-color was set as a Function");
        }
    }

 
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getBackgroundPattern() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetBackgroundPattern());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getBackgroundOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetBackgroundOpacity());
    }
 
    private native Object nativeGetBackgroundColor();

    private native Object nativeGetBackgroundPattern();

    private native Object nativeGetBackgroundOpacity();


    @Override
    protected native void finalize() throws Throwable;

}
