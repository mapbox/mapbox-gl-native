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

    static PropertyValue<float> getDefaultLineOpacity();
    PropertyValue<float> getLineOpacity(const optional<std::string>& klass = {}) const;
    void setLineOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultLineColor();
    PropertyValue<Color> getLineColor(const optional<std::string>& klass = {}) const;
    void setLineColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<std::array<float, 2>> getDefaultLineTranslate();
    PropertyValue<std::array<float, 2>> getLineTranslate(const optional<std::string>& klass = {}) const;
    void setLineTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});

    static PropertyValue<TranslateAnchorType> getDefaultLineTranslateAnchor();
    PropertyValue<TranslateAnchorType> getLineTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setLineTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultLineWidth();
    PropertyValue<float> getLineWidth(const optional<std::string>& klass = {}) const;
    void setLineWidth(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultLineGapWidth();
    PropertyValue<float> getLineGapWidth(const optional<std::string>& klass = {}) const;
    void setLineGapWidth(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultLineOffset();
    PropertyValue<float> getLineOffset(const optional<std::string>& klass = {}) const;
    void setLineOffset(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultLineBlur();
    PropertyValue<float> getLineBlur(const optional<std::string>& klass = {}) const;
    void setLineBlur(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<std::vector<float>> getDefaultLineDasharray();
    PropertyValue<std::vector<float>> getLineDasharray(const optional<std::string>& klass = {}) const;
    void setLineDasharray(PropertyValue<std::vector<float>>, const optional<std::string>& klass = {});

    static PropertyValue<std::string> getDefaultLinePattern();
    PropertyValue<std::string> getLinePattern(const optional<std::string>& klass = {}) const;
    void setLinePattern(PropertyValue<std::string>, const optional<std::string>& klass = {});

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
