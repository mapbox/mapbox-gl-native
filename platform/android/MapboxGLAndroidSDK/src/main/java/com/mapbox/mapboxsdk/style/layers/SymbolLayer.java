// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.exceptions.ConversionException;

import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;

import static com.mapbox.mapboxsdk.utils.ColorUtils.*;

/**
 * Symbol Layer
 */
public class SymbolLayer extends Layer {

    /**
     * Creates a SymbolLayer.
     *
     * @param nativePtr pointer used by core
     */
    public SymbolLayer(long nativePtr) {
        super(nativePtr);
    }

    /**
     * Creates a SymbolLayer.
     *
     * @param layerId  the id of the layer
     * @param sourceId the id of the source
     */
    public SymbolLayer(String layerId, String sourceId) {
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
    public SymbolLayer withSourceLayer(String sourceLayer) {
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
    public SymbolLayer withFilter(Object[] filter) {
        setFilter(filter);
        return this;
    }

    /**
     * Set a single filter.
     *
     * @param filter the filter to set
     * @return This
     */
    public SymbolLayer withFilter(Filter.Statement filter) {
        setFilter(filter);
        return this;
    }


    /**
     * Set a property or properties.
     *
     * @param properties the var-args properties
     * @return This
     */
    public SymbolLayer withProperties(@NonNull Property<?>... properties) {
        setProperties(properties);
        return this;
    }

    // Property getters

    /**
     * Get the SymbolPlacement property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getSymbolPlacement() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetSymbolPlacement());
    }
 
    /**
     * Get the SymbolSpacing property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getSymbolSpacing() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetSymbolSpacing());
    }
 
    /**
     * Get the SymbolAvoidEdges property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getSymbolAvoidEdges() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetSymbolAvoidEdges());
    }
 
    /**
     * Get the IconAllowOverlap property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconAllowOverlap() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconAllowOverlap());
    }
 
    /**
     * Get the IconIgnorePlacement property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconIgnorePlacement() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconIgnorePlacement());
    }
 
    /**
     * Get the IconOptional property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconOptional() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconOptional());
    }
 
    /**
     * Get the IconRotationAlignment property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconRotationAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconRotationAlignment());
    }
 
    /**
     * Get the IconSize property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconSize() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconSize());
    }
 
    /**
     * Get the IconTextFit property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconTextFit() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconTextFit());
    }
 
    /**
     * Get the IconTextFitPadding property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getIconTextFitPadding() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconTextFitPadding());
    }
 
    /**
     * Get the IconImage property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconImage() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconImage());
    }
 
    /**
     * Get the IconRotate property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconRotate() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconRotate());
    }
 
    /**
     * Get the IconPadding property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconPadding() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconPadding());
    }
 
    /**
     * Get the IconKeepUpright property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconKeepUpright() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconKeepUpright());
    }
 
    /**
     * Get the IconOffset property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getIconOffset() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconOffset());
    }
 
    /**
     * Get the TextPitchAlignment property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextPitchAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextPitchAlignment());
    }
 
    /**
     * Get the TextRotationAlignment property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextRotationAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextRotationAlignment());
    }
 
    /**
     * Get the TextField property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextField() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextField());
    }
 
    /**
     * Get the TextFont property
     *
     * @return property wrapper value around String[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String[]> getTextFont() {
        checkValidity();
        return (PropertyValue<String[]>) new PropertyValue(nativeGetTextFont());
    }
 
    /**
     * Get the TextSize property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextSize() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextSize());
    }
 
    /**
     * Get the TextMaxWidth property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextMaxWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextMaxWidth());
    }
 
    /**
     * Get the TextLineHeight property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextLineHeight() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextLineHeight());
    }
 
    /**
     * Get the TextLetterSpacing property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextLetterSpacing() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextLetterSpacing());
    }
 
    /**
     * Get the TextJustify property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextJustify() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextJustify());
    }
 
    /**
     * Get the TextAnchor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextAnchor());
    }
 
    /**
     * Get the TextMaxAngle property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextMaxAngle() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextMaxAngle());
    }
 
    /**
     * Get the TextRotate property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextRotate() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextRotate());
    }
 
    /**
     * Get the TextPadding property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextPadding() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextPadding());
    }
 
    /**
     * Get the TextKeepUpright property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextKeepUpright() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextKeepUpright());
    }
 
    /**
     * Get the TextTransform property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextTransform() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextTransform());
    }
 
    /**
     * Get the TextOffset property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getTextOffset() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetTextOffset());
    }
 
    /**
     * Get the TextAllowOverlap property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextAllowOverlap() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextAllowOverlap());
    }
 
    /**
     * Get the TextIgnorePlacement property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextIgnorePlacement() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextIgnorePlacement());
    }
 
    /**
     * Get the TextOptional property
     *
     * @return property wrapper value around Boolean
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextOptional() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextOptional());
    }
 
    /**
     * Get the IconOpacity property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconOpacity());
    }
 
    /**
     * Get the IconColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconColor());
    }
     /**
     * The color of the icon. This can only be used with sdf icons.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getIconColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getIconColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("icon-color was set as a Function");
        }
    }

 
    /**
     * Get the IconHaloColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconHaloColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconHaloColor());
    }
     /**
     * The color of the icon's halo. Icon halos can only be used with sdf icons.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getIconHaloColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getIconHaloColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("icon-halo-color was set as a Function");
        }
    }

 
    /**
     * Get the IconHaloWidth property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconHaloWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconHaloWidth());
    }
 
    /**
     * Get the IconHaloBlur property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconHaloBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconHaloBlur());
    }
 
    /**
     * Get the IconTranslate property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getIconTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconTranslate());
    }
 
    /**
     * Get the IconTranslateAnchor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconTranslateAnchor());
    }
 
    /**
     * Get the TextOpacity property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextOpacity());
    }
 
    /**
     * Get the TextColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextColor());
    }
     /**
     * The color with which the text will be drawn.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getTextColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getTextColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("text-color was set as a Function");
        }
    }

 
    /**
     * Get the TextHaloColor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextHaloColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextHaloColor());
    }
     /**
     * The color of the text's halo, which helps it stand out from backgrounds.
     *
     * @return int representation of a rgba string color
     * @throws RuntimeException thrown if property isn't a value
     */
    @ColorInt
    public int getTextHaloColorAsInt() {
        checkValidity();
        PropertyValue<String> value = getTextHaloColor();
        if (value.isValue()) {
            return rgbaToColor(value.getValue());
        } else {
            throw new RuntimeException("text-halo-color was set as a Function");
        }
    }

 
    /**
     * Get the TextHaloWidth property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextHaloWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextHaloWidth());
    }
 
    /**
     * Get the TextHaloBlur property
     *
     * @return property wrapper value around Float
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextHaloBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextHaloBlur());
    }
 
    /**
     * Get the TextTranslate property
     *
     * @return property wrapper value around Float[]
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getTextTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetTextTranslate());
    }
 
    /**
     * Get the TextTranslateAnchor property
     *
     * @return property wrapper value around String
     */
    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextTranslateAnchor());
    }
 
    private native Object nativeGetSymbolPlacement();

    private native Object nativeGetSymbolSpacing();

    private native Object nativeGetSymbolAvoidEdges();

    private native Object nativeGetIconAllowOverlap();

    private native Object nativeGetIconIgnorePlacement();

    private native Object nativeGetIconOptional();

    private native Object nativeGetIconRotationAlignment();

    private native Object nativeGetIconSize();

    private native Object nativeGetIconTextFit();

    private native Object nativeGetIconTextFitPadding();

    private native Object nativeGetIconImage();

    private native Object nativeGetIconRotate();

    private native Object nativeGetIconPadding();

    private native Object nativeGetIconKeepUpright();

    private native Object nativeGetIconOffset();

    private native Object nativeGetTextPitchAlignment();

    private native Object nativeGetTextRotationAlignment();

    private native Object nativeGetTextField();

    private native Object nativeGetTextFont();

    private native Object nativeGetTextSize();

    private native Object nativeGetTextMaxWidth();

    private native Object nativeGetTextLineHeight();

    private native Object nativeGetTextLetterSpacing();

    private native Object nativeGetTextJustify();

    private native Object nativeGetTextAnchor();

    private native Object nativeGetTextMaxAngle();

    private native Object nativeGetTextRotate();

    private native Object nativeGetTextPadding();

    private native Object nativeGetTextKeepUpright();

    private native Object nativeGetTextTransform();

    private native Object nativeGetTextOffset();

    private native Object nativeGetTextAllowOverlap();

    private native Object nativeGetTextIgnorePlacement();

    private native Object nativeGetTextOptional();

    private native Object nativeGetIconOpacity();

    private native Object nativeGetIconColor();

    private native Object nativeGetIconHaloColor();

    private native Object nativeGetIconHaloWidth();

    private native Object nativeGetIconHaloBlur();

    private native Object nativeGetIconTranslate();

    private native Object nativeGetIconTranslateAnchor();

    private native Object nativeGetTextOpacity();

    private native Object nativeGetTextColor();

    private native Object nativeGetTextHaloColor();

    private native Object nativeGetTextHaloWidth();

    private native Object nativeGetTextHaloBlur();

    private native Object nativeGetTextTranslate();

    private native Object nativeGetTextTranslateAnchor();


    @Override
    protected native void finalize() throws Throwable;

}
