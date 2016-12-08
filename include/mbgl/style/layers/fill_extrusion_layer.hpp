// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class FillExtrusionLayer : public Layer {
public:
    FillExtrusionLayer(const std::string& layerID, const std::string& sourceID);
    ~FillExtrusionLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    static PropertyValue<float> getDefaultFillExtrusionOpacity();
    PropertyValue<float> getFillExtrusionOpacity(const optional<std::string>& klass = {}) const;
    void setFillExtrusionOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultFillExtrusionColor();
    PropertyValue<Color> getFillExtrusionColor(const optional<std::string>& klass = {}) const;
    void setFillExtrusionColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<std::array<float, 2>> getDefaultFillExtrusionTranslate();
    PropertyValue<std::array<float, 2>> getFillExtrusionTranslate(const optional<std::string>& klass = {}) const;
    void setFillExtrusionTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});

    static PropertyValue<TranslateAnchorType> getDefaultFillExtrusionTranslateAnchor();
    PropertyValue<TranslateAnchorType> getFillExtrusionTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});

    static PropertyValue<std::string> getDefaultFillExtrusionPattern();
    PropertyValue<std::string> getFillExtrusionPattern(const optional<std::string>& klass = {}) const;
    void setFillExtrusionPattern(PropertyValue<std::string>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultFillExtrusionHeight();
    PropertyValue<float> getFillExtrusionHeight(const optional<std::string>& klass = {}) const;
    void setFillExtrusionHeight(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultFillExtrusionBase();
    PropertyValue<float> getFillExtrusionBase(const optional<std::string>& klass = {}) const;
    void setFillExtrusionBase(PropertyValue<float>, const optional<std::string>& klass = {});

    // Private implementation

    class Impl;
    Impl* const impl;

    FillExtrusionLayer(const Impl&);
    FillExtrusionLayer(const FillExtrusionLayer&) = delete;
};

template <>
inline bool Layer::is<FillExtrusionLayer>() const {
    return type == Type::FillExtrusion;
}

} // namespace style
} // namespace mbgl
