// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

/**
 * Fill Layer
 */
public class FillLayer extends Layer {

    public FillLayer(long nativePtr) {
        super(nativePtr);
    }

    public FillLayer(String layerId, String sourceId) {
        initialize(layerId, sourceId);
    }

    protected native void initialize(String layerId, String sourceId);

    public void setSourceLayer(String sourceLayer) {
        checkValidity();
        nativeSetSourceLayer(sourceLayer);
    }

    public void setFilter(Filter.Statement filter) {
        checkValidity();
        this.setFilter(filter.toArray());
    }

    public void setFilter(Object[] filter) {
        checkValidity();
        nativeSetFilter(filter);
    }


    // Property getters

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getFillAntialias() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetFillAntialias());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getFillOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetFillOpacity());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getFillColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetFillColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getFillOutlineColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetFillOutlineColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getFillTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetFillTranslate());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getFillTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetFillTranslateAnchor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getFillPattern() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetFillPattern());
    }

    private native Object nativeGetFillAntialias();

    private native Object nativeGetFillOpacity();

    private native Object nativeGetFillColor();

    private native Object nativeGetFillOutlineColor();

    private native Object nativeGetFillTranslate();

    private native Object nativeGetFillTranslateAnchor();

    private native Object nativeGetFillPattern();

}
