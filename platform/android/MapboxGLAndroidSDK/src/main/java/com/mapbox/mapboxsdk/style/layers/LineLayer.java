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

    /**
     * Creates a LineLayer.
     *
     * @param nativePtr pointer used by core
     */
    public LineLayer(long nativePtr) {
        super(nativePtr);
    }

    /**
     * Creates a LineLayer.
     *
     * @param layerId  the id of the layer
     * @param sourceId the id of the source
     */
    public LineLayer(String layerId, String sourceId) {
        initialize(layerId, sourceId);
    }

    protected native void initialize(String layerId, String sourceId);

    /**
     * Set the source layer.
     *
     * @param sourceLayer the source layer to set
     */
    public void setSourceLayer(String sourceLayer) {
        checkValidity();
        nativeSetSourceLayer(sourceLayer);
    }

    /**
     * Set the source Layer.
     *
     * @param sourceLayer the source layer to set
     * @return This
     */
    public LineLayer withSourceLayer(String sourceLayer) {
        setSourceLayer(sourceLayer);
        return this;
    }
    /**
     * Set a single filter.
     *
     * @param filter the filter to set
     */
    public void setFilter(Filter.Statement filter) {
        checkValidity();
        this.setFilter(filter.toArray());
    }

    /**
     * Set an array of filters.
     *
     * @param filter the filter array to set
     */
    public void setFilter(Object[] filter) {
        checkValidity();
        nativeSetFilter(filter);
    }

    /**
     * Set an array of filters.
     *
     * @param filter tthe filter array to set
     * @return This
     */
    public LineLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    /**
     * Set a single filter.
     *
     * @param filter the filter to set
     * @return This
     */
    public LineLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    /**
     * Set a property or properties.
     *
     * @param properties the var-args properties
     * @return This
     */
    public LineLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    /**
     * Get the LineCap property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineCap() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineCap());
    }
 
    /**
     * Get the LineJoin property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineJoin() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineJoin());
    }
 
    /**
     * Get the LineMiterLimit property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineMiterLimit() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineMiterLimit());
    }
 
    /**
     * Get the LineRoundLimit property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineRoundLimit() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineRoundLimit());
    }
 
    /**
     * Get the LineOpacity property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineOpacity());
    }
 
    /**
     * Get the LineColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineColor());
    }
     /**
     * The color with which the line will be drawn.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
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

 
    /**
     * Get the LineTranslate property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getLineTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetLineTranslate());
    }
 
    /**
     * Get the LineTranslateAnchor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getLineTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetLineTranslateAnchor());
    }
 
    /**
     * Get the LineWidth property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineWidth());
    }
 
    /**
     * Get the LineGapWidth property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineGapWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineGapWidth());
    }
 
    /**
     * Get the LineOffset property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineOffset() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineOffset());
    }
 
    /**
     * Get the LineBlur property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getLineBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetLineBlur());
    }
 
    /**
     * Get the LineDasharray property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getLineDasharray() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetLineDasharray());
    }
 
    /**
     * Get the LinePattern property
     *
     * @return property wrapper value around String
     */
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


    @Override
    protected native void finalize() throws Throwable;

}
