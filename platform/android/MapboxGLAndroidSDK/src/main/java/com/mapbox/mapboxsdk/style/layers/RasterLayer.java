// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

/**
 * Raster Layer
 */
public class RasterLayer extends Layer {

    public RasterLayer(long nativePtr) {
        super(nativePtr);
    }

    public RasterLayer(String layerId, String sourceId) {
        initialize(layerId, sourceId);
    }

    protected native void initialize(String layerId, String sourceId);

    public void setSourceLayer(String sourceLayer) {
        checkValidity();
        nativeSetSourceLayer(sourceLayer);
    }

    public RasterLayer withSourceLayer(String sourceLayer) {
        setSourceLayer(sourceLayer);
        return this;
    }


    public RasterLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterOpacity());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterHueRotate() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterHueRotate());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterBrightnessMin() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterBrightnessMin());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterBrightnessMax() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterBrightnessMax());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterSaturation() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterSaturation());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterContrast() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterContrast());
    }
 
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getRasterFadeDuration() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetRasterFadeDuration());
    }
 
    private native Object nativeGetRasterOpacity();

    private native Object nativeGetRasterHueRotate();

    private native Object nativeGetRasterBrightnessMin();

    private native Object nativeGetRasterBrightnessMax();

    private native Object nativeGetRasterSaturation();

    private native Object nativeGetRasterContrast();

    private native Object nativeGetRasterFadeDuration();


    @Override
    protected native void finalize() throws Throwable;

}
