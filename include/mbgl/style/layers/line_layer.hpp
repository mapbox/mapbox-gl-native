// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

class TransitionOptions;

class LineLayer : public Layer {
public:
    LineLayer(const std::string& layerID, const std::string& sourceID);
    ~LineLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Layout properties

    static PropertyValue<LineCapType> getDefaultLineCap();
    PropertyValue<LineCapType> getLineCap() const;
    void setLineCap(PropertyValue<LineCapType>);

    static PropertyValue<LineJoinType> getDefaultLineJoin();
    PropertyValue<LineJoinType> getLineJoin() const;
    void setLineJoin(PropertyValue<LineJoinType>);

    static PropertyValue<float> getDefaultLineMiterLimit();
    PropertyValue<float> getLineMiterLimit() const;
    void setLineMiterLimit(PropertyValue<float>);

    static PropertyValue<float> getDefaultLineRoundLimit();
    PropertyValue<float> getLineRoundLimit() const;
    void setLineRoundLimit(PropertyValue<float>);

    // Paint properties

    static DataDrivenPropertyValue<float> getDefaultLineOpacity();
    DataDrivenPropertyValue<float> getLineOpacity(const optional<std::string>& klass = {}) const;
    void setLineOpacity(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setLineOpacityTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineOpacityTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultLineColor();
    DataDrivenPropertyValue<Color> getLineColor(const optional<std::string>& klass = {}) const;
    void setLineColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setLineColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineColorTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::array<float, 2>> getDefaultLineTranslate();
    PropertyValue<std::array<float, 2>> getLineTranslate(const optional<std::string>& klass = {}) const;
    void setLineTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});
    void setLineTranslateTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineTranslateTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<TranslateAnchorType> getDefaultLineTranslateAnchor();
    PropertyValue<TranslateAnchorType> getLineTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setLineTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});
    void setLineTranslateAnchorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineTranslateAnchorTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultLineWidth();
    PropertyValue<float> getLineWidth(const optional<std::string>& klass = {}) const;
    void setLineWidth(PropertyValue<float>, const optional<std::string>& klass = {});
    void setLineWidthTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineWidthTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultLineGapWidth();
    DataDrivenPropertyValue<float> getLineGapWidth(const optional<std::string>& klass = {}) const;
    void setLineGapWidth(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setLineGapWidthTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineGapWidthTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultLineOffset();
    DataDrivenPropertyValue<float> getLineOffset(const optional<std::string>& klass = {}) const;
    void setLineOffset(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setLineOffsetTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineOffsetTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultLineBlur();
    DataDrivenPropertyValue<float> getLineBlur(const optional<std::string>& klass = {}) const;
    void setLineBlur(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setLineBlurTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineBlurTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::vector<float>> getDefaultLineDasharray();
    PropertyValue<std::vector<float>> getLineDasharray(const optional<std::string>& klass = {}) const;
    void setLineDasharray(PropertyValue<std::vector<float>>, const optional<std::string>& klass = {});
    void setLineDasharrayTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLineDasharrayTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::string> getDefaultLinePattern();
    PropertyValue<std::string> getLinePattern(const optional<std::string>& klass = {}) const;
    void setLinePattern(PropertyValue<std::string>, const optional<std::string>& klass = {});
    void setLinePatternTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getLinePatternTransition(const optional<std::string>& klass = {}) const;

    // Private implementation

    class Impl;
    Impl* const impl;

    LineLayer(const Impl&);
    LineLayer(const LineLayer&) = delete;
};

template <>
inline bool Layer::is<LineLayer>() const {
    return type == LayerType::Line;
}

} // namespace style
} // namespace mbgl
