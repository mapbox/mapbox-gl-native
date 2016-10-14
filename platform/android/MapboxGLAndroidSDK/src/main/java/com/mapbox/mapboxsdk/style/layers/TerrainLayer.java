// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

/**
 * Terrain visualization based on DEM data.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layers-terrain">The online documentation</a>
 */
public class TerrainLayer extends Layer {

    /**
     * Creates a TerrainLayer.
     *
     * @param nativePtr pointer used by core
     */
    public TerrainLayer(long nativePtr) {
        super(nativePtr);
    }

    /**
     * Creates a TerrainLayer.
     *
     * @param layerId  the id of the layer
     * @param sourceId the id of the source
     */
    public TerrainLayer(String layerId, String sourceId) {
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
    public TerrainLayer withSourceLayer(String sourceLayer) {
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
    public TerrainLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    /**
     * Set a single filter.
     *
     * @param filter the filter to set
     * @return This
     */
    public TerrainLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    /**
     * Set a property or properties.
     *
     * @param properties the var-args properties
     * @return This
     */
    public TerrainLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    /**
     * Get the TerrainShadowColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTerrainShadowColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTerrainShadowColor());
    }
     /**
     * The color of the terrain that faces away from the light source.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getTerrainShadowColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getTerrainShadowColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("terrain-shadow-color was set as a Function");
        }
    }

 
    /**
     * Get the TerrainHighlightColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTerrainHighlightColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTerrainHighlightColor());
    }
     /**
     * The color of the terrain that faces towards the light source.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getTerrainHighlightColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getTerrainHighlightColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("terrain-highlight-color was set as a Function");
        }
    }

 
    /**
     * Get the TerrainAccentColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTerrainAccentColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTerrainAccentColor());
    }
     /**
     * The color used to accentuate rugged terrain with sharp cliffs and gorges.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getTerrainAccentColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getTerrainAccentColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("terrain-accent-color was set as a Function");
        }
    }

 
    /**
     * Get the TerrainIlluminationDirection property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTerrainIlluminationDirection() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTerrainIlluminationDirection());
    }
 
    /**
     * Get the TerrainIlluminationAlignment property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTerrainIlluminationAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTerrainIlluminationAlignment());
    }
 
    /**
     * Get the TerrainExaggeration property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTerrainExaggeration() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTerrainExaggeration());
    }
 
    private native Object nativeGetTerrainShadowColor();

    private native Object nativeGetTerrainHighlightColor();

    private native Object nativeGetTerrainAccentColor();

    private native Object nativeGetTerrainIlluminationDirection();

    private native Object nativeGetTerrainIlluminationAlignment();

    private native Object nativeGetTerrainExaggeration();


    @Override
    protected native void finalize() throws Throwable;

}
