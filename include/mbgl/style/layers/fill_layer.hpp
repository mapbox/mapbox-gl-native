// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class FillLayer : public Layer {
public:
    FillLayer(const std::string& layerID, const std::string& sourceID);
    ~FillLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Paint properties

    static PropertyValue<bool> getDefaultFillAntialias();
    PropertyValue<bool> getFillAntialias(const optional<std::string>& klass = {}) const;
    void setFillAntialias(PropertyValue<bool>, const optional<std::string>& klass = {});
    void setFillAntialiasTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillAntialiasTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultFillOpacity();
    DataDrivenPropertyValue<float> getFillOpacity(const optional<std::string>& klass = {}) const;
    void setFillOpacity(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setFillOpacityTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillOpacityTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultFillColor();
    DataDrivenPropertyValue<Color> getFillColor(const optional<std::string>& klass = {}) const;
    void setFillColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setFillColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillColorTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultFillOutlineColor();
    DataDrivenPropertyValue<Color> getFillOutlineColor(const optional<std::string>& klass = {}) const;
    void setFillOutlineColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setFillOutlineColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillOutlineColorTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::array<float, 2>> getDefaultFillTranslate();
    PropertyValue<std::array<float, 2>> getFillTranslate(const optional<std::string>& klass = {}) const;
    void setFillTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});
    void setFillTranslateTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillTranslateTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<TranslateAnchorType> getDefaultFillTranslateAnchor();
    PropertyValue<TranslateAnchorType> getFillTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setFillTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});
    void setFillTranslateAnchorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillTranslateAnchorTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::string> getDefaultFillPattern();
    PropertyValue<std::string> getFillPattern(const optional<std::string>& klass = {}) const;
    void setFillPattern(PropertyValue<std::string>, const optional<std::string>& klass = {});
    void setFillPatternTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillPatternTransition(const optional<std::string>& klass = {}) const;

    // Private implementation

    class Impl;
    Impl* const impl;

    FillLayer(const Impl&);
    FillLayer(const FillLayer&) = delete;
};

template <>
inline bool Layer::is<FillLayer>() const {
    return type == LayerType::Fill;
}

} // namespace style
} // namespace mbgl
