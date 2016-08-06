// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

/**
 * Line Layer
 */
public class LineLayer extends Layer {

    public LineLayer(long nativePtr) {
        super(nativePtr);
    }

    public LineLayer(String layerId, String sourceId) {
        initialize(layerId, sourceId);
    }

    protected native void initialize(String layerId, String sourceId);

    public void setSourceLayer(String sourceLayer) {
        checkValidity();
        nativeSetSourceLayer(sourceLayer);
    }

    public LineLayer withSourceLayer(String sourceLayer) {
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

    public LineLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    public LineLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    public LineLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineCap() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineCap());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineJoin() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineJoin());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineMiterLimit() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineMiterLimit());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineRoundLimit() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineRoundLimit());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineOpacity());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineColor());
    }
      /**
      * The color with which the line will be drawn.
      * @throws RuntimeException
      */
    @ColorInt
    public int getLineColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getLineColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("line-color was set as a Function");
        }
    }

 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getLineTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetLineTranslate());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineTranslateAnchor());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineWidth());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineGapWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineGapWidth());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineOffset() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineOffset());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineBlur());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getLineDasharray() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetLineDasharray());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLinePattern() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLinePattern());
    }
 
    private native Object nativeGetLineCap();

    private native Object nativeGetLineJoin();

    private native Object nativeGetLineMiterLimit();

    private native Object nativeGetLineRoundLimit();

    private native Object nativeGetLineOpacity();

    private native Object nativeGetLineColor();

    private native Object nativeGetLineTranslate();

    private native Object nativeGetLineTranslateAnchor();

    private native Object nativeGetLineWidth();

    private native Object nativeGetLineGapWidth();

    private native Object nativeGetLineOffset();

    private native Object nativeGetLineBlur();

    private native Object nativeGetLineDasharray();

    private native Object nativeGetLinePattern();

}
