// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

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

    PropertyValue<LineCapType> getLineCap() const;
    void setLineCap(PropertyValue<LineCapType>);

    PropertyValue<LineJoinType> getLineJoin() const;
    void setLineJoin(PropertyValue<LineJoinType>);

    PropertyValue<float> getLineMiterLimit() const;
    void setLineMiterLimit(PropertyValue<float>);

    PropertyValue<float> getLineRoundLimit() const;
    void setLineRoundLimit(PropertyValue<float>);

    // Paint properties

    PropertyValue<float> getLineOpacity() const;
    void setLineOpacity(PropertyValue<float>);

    PropertyValue<Color> getLineColor() const;
    void setLineColor(PropertyValue<Color>);

    PropertyValue<std::array<float, 2>> getLineTranslate() const;
    void setLineTranslate(PropertyValue<std::array<float, 2>>);

    PropertyValue<TranslateAnchorType> getLineTranslateAnchor() const;
    void setLineTranslateAnchor(PropertyValue<TranslateAnchorType>);

    PropertyValue<float> getLineWidth() const;
    void setLineWidth(PropertyValue<float>);

    PropertyValue<float> getLineGapWidth() const;
    void setLineGapWidth(PropertyValue<float>);

    PropertyValue<float> getLineOffset() const;
    void setLineOffset(PropertyValue<float>);

    PropertyValue<float> getLineBlur() const;
    void setLineBlur(PropertyValue<float>);

    PropertyValue<std::vector<float>> getLineDasharray() const;
    void setLineDasharray(PropertyValue<std::vector<float>>);

    PropertyValue<std::string> getLinePattern() const;
    void setLinePattern(PropertyValue<std::string>);

    // Private implementation

    class Impl;
    Impl* const impl;

    LineLayer(const Impl&);
    LineLayer(const LineLayer&) = delete;
};

template <>
inline bool Layer::is<LineLayer>() const {
    return type == Type::Line;
}

} // namespace style
} // namespace mbgl
