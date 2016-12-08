// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class CircleLayer : public Layer {
public:
    CircleLayer(const std::string& layerID, const std::string& sourceID);
    ~CircleLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    static PropertyValue<float> getDefaultCircleRadius();
    PropertyValue<float> getCircleRadius(const optional<std::string>& klass = {}) const;
    void setCircleRadius(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultCircleColor();
    PropertyValue<Color> getCircleColor(const optional<std::string>& klass = {}) const;
    void setCircleColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultCircleBlur();
    PropertyValue<float> getCircleBlur(const optional<std::string>& klass = {}) const;
    void setCircleBlur(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultCircleOpacity();
    PropertyValue<float> getCircleOpacity(const optional<std::string>& klass = {}) const;
    void setCircleOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<std::array<float, 2>> getDefaultCircleTranslate();
    PropertyValue<std::array<float, 2>> getCircleTranslate(const optional<std::string>& klass = {}) const;
    void setCircleTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});

    static PropertyValue<TranslateAnchorType> getDefaultCircleTranslateAnchor();
    PropertyValue<TranslateAnchorType> getCircleTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setCircleTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});

    static PropertyValue<CirclePitchScaleType> getDefaultCirclePitchScale();
    PropertyValue<CirclePitchScaleType> getCirclePitchScale(const optional<std::string>& klass = {}) const;
    void setCirclePitchScale(PropertyValue<CirclePitchScaleType>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultCircleStrokeWidth();
    PropertyValue<float> getCircleStrokeWidth(const optional<std::string>& klass = {}) const;
    void setCircleStrokeWidth(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultCircleStrokeColor();
    PropertyValue<Color> getCircleStrokeColor(const optional<std::string>& klass = {}) const;
    void setCircleStrokeColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultCircleStrokeOpacity();
    PropertyValue<float> getCircleStrokeOpacity(const optional<std::string>& klass = {}) const;
    void setCircleStrokeOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    // Private implementation

    class Impl;
    Impl* const impl;

    CircleLayer(const Impl&);
    CircleLayer(const CircleLayer&) = delete;
};

template <>
inline bool Layer::is<CircleLayer>() const {
    return type == Type::Circle;
}

} // namespace style
} // namespace mbgl
