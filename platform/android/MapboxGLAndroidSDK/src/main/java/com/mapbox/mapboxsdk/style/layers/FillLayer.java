// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

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

    public FillLayer withSourceLayer(String sourceLayer) {
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

    public FillLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    public FillLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    public FillLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
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
      /**
      * The color of the filled part of this layer. This color can be specified as rgba with an alpha component and the color's opacity will not affect the opacity of the 1px stroke, if it is used.
      * @throws RuntimeException
      */
    @ColorInt
    public int getFillColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getFillColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("fill-color was set as a Function");
        }
    }

 
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getFillOutlineColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetFillOutlineColor());
    }
      /**
      * The outline color of the fill. Matches the value of `fill-color` if unspecified.
      * @throws RuntimeException
      */
    @ColorInt
    public int getFillOutlineColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getFillOutlineColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("fill-outline-color was set as a Function");
        }
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
