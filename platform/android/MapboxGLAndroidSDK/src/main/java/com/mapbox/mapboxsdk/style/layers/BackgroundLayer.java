// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

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


    // Property getters

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getBackgroundColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetBackgroundColor());
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

}
