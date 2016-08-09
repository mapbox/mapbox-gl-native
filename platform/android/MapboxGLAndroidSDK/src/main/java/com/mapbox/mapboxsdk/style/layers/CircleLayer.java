// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

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

    public CircleLayer withSourceLayer(String sourceLayer) {
        setSourceLayer(sourceLayer);
        return this;
    }

    public void setFilter(Filter.Statement filter) {
        checkValidity();
        this.setFilter(filter.toArray());
    }

    public void setFilter(Object[] filter) {
        checkValidity();
        nativeSetFilter(filter);
    }

    public CircleLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    public CircleLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    public CircleLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
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
      /**
      * The color of the circle.
      * @throws RuntimeException
      */
    @ColorInt
    public int getCircleColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getCircleColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("circle-color was set as a Function");
        }
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


    @Override
    protected native void finalize() throws Throwable;

}
