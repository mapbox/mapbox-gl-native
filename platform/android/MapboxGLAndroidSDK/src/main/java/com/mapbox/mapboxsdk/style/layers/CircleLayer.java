// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

/**
 * Circle Layer
 */
public class CircleLayer extends Layer {

    public CircleLayer(long nativePtr) {
        super(nativePtr);
    }

    public CircleLayer(String layerId, String sourceId) {
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
    public PropertyValue<Float> getCircleRadius() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetCircleRadius());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getCircleColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetCircleColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getCircleBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetCircleBlur());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getCircleOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetCircleOpacity());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getCircleTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetCircleTranslate());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getCircleTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetCircleTranslateAnchor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getCirclePitchScale() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetCirclePitchScale());
    }

    private native Object nativeGetCircleRadius();

    private native Object nativeGetCircleColor();

    private native Object nativeGetCircleBlur();

    private native Object nativeGetCircleOpacity();

    private native Object nativeGetCircleTranslate();

    private native Object nativeGetCircleTranslateAnchor();

    private native Object nativeGetCirclePitchScale();

}
