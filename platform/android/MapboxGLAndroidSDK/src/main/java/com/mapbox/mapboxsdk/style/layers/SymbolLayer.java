// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
package com.mapbox.mapboxsdk.style.layers;

/**
 * Symbol Layer
 */
public class SymbolLayer extends Layer {

    public SymbolLayer(long nativePtr) {
        super(nativePtr);
    }

    public SymbolLayer(String layerId, String sourceId) {
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
    public PropertyValue<String> getSymbolPlacement() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetSymbolPlacement());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getSymbolSpacing() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetSymbolSpacing());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getSymbolAvoidEdges() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetSymbolAvoidEdges());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconAllowOverlap() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconAllowOverlap());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconIgnorePlacement() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconIgnorePlacement());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconOptional() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconOptional());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconRotationAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconRotationAlignment());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconSize() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconSize());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconTextFit() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconTextFit());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getIconTextFitPadding() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconTextFitPadding());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconImage() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconImage());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconRotate() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconRotate());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconPadding() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconPadding());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getIconKeepUpright() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetIconKeepUpright());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getIconOffset() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconOffset());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextPitchAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextPitchAlignment());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextRotationAlignment() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextRotationAlignment());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextField() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextField());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String[]> getTextFont() {
        checkValidity();
        return (PropertyValue<String[]>) new PropertyValue(nativeGetTextFont());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextSize() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextSize());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextMaxWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextMaxWidth());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextLineHeight() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextLineHeight());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextLetterSpacing() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextLetterSpacing());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextJustify() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextJustify());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextAnchor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextMaxAngle() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextMaxAngle());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextRotate() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextRotate());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextPadding() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextPadding());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextKeepUpright() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextKeepUpright());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextTransform() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextTransform());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getTextOffset() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetTextOffset());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextAllowOverlap() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextAllowOverlap());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextIgnorePlacement() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextIgnorePlacement());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Boolean> getTextOptional() {
        checkValidity();
        return (PropertyValue<Boolean>) new PropertyValue(nativeGetTextOptional());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconOpacity());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconHaloColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconHaloColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconHaloWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconHaloWidth());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getIconHaloBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetIconHaloBlur());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getIconTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetIconTranslate());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getIconTranslateAnchor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetIconTranslateAnchor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextOpacity() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextOpacity());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<String> getTextHaloColor() {
        checkValidity();
        return (PropertyValue<String>) new PropertyValue(nativeGetTextHaloColor());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextHaloWidth() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextHaloWidth());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float> getTextHaloBlur() {
        checkValidity();
        return (PropertyValue<Float>) new PropertyValue(nativeGetTextHaloBlur());
    }

    @SuppressWarnings("unchecked")
    public PropertyValue<Float[]> getTextTranslate() {
        checkValidity();
        return (PropertyValue<Float[]>) new PropertyValue(nativeGetTextTranslate());
    }

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

}
