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

    /**
     * Creates a CircleLayer.
     *
     * @param nativePtr pointer used by core
     */
    public CircleLayer(long nativePtr) {
        super(nativePtr);
    }

    /**
     * Creates a CircleLayer.
     *
     * @param layerId  the id of the layer
     * @param sourceId the id of the source
     */
    public CircleLayer(String layerId, String sourceId) {
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
    public CircleLayer withSourceLayer(String sourceLayer) {
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
    public CircleLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    /**
     * Set a single filter.
     *
     * @param filter the filter to set
     * @return This
     */
    public CircleLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    /**
     * Set a property or properties.
     *
     * @param properties the var-args properties
     * @return This
     */
    public CircleLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    /**
     * Get the CircleRadius property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getCircleRadius() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetCircleRadius());
    }
 
    /**
     * Get the CircleColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getCircleColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetCircleColor());
    }
     /**
     * The color of the circle.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
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

 
    /**
     * Get the CircleBlur property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getCircleBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetCircleBlur());
    }
 
    /**
     * Get the CircleOpacity property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getCircleOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetCircleOpacity());
    }
 
    /**
     * Get the CircleTranslate property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getCircleTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetCircleTranslate());
    }
 
    /**
     * Get the CircleTranslateAnchor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getCircleTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetCircleTranslateAnchor());
    }
 
    /**
     * Get the CirclePitchScale property
     *
     * @return property wrapper value around String
     */
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
