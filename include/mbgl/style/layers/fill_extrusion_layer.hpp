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
    void setFillExtrusionOpacityTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionOpacityTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultFillExtrusionColor();
    DataDrivenPropertyValue<Color> getFillExtrusionColor(const optional<std::string>& klass = {}) const;
    void setFillExtrusionColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setFillExtrusionColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionColorTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::array<float, 2>> getDefaultFillExtrusionTranslate();
    PropertyValue<std::array<float, 2>> getFillExtrusionTranslate(const optional<std::string>& klass = {}) const;
    void setFillExtrusionTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});
    void setFillExtrusionTranslateTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionTranslateTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<TranslateAnchorType> getDefaultFillExtrusionTranslateAnchor();
    PropertyValue<TranslateAnchorType> getFillExtrusionTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});
    void setFillExtrusionTranslateAnchorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionTranslateAnchorTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::string> getDefaultFillExtrusionPattern();
    PropertyValue<std::string> getFillExtrusionPattern(const optional<std::string>& klass = {}) const;
    void setFillExtrusionPattern(PropertyValue<std::string>, const optional<std::string>& klass = {});
    void setFillExtrusionPatternTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionPatternTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultFillExtrusionHeight();
    DataDrivenPropertyValue<float> getFillExtrusionHeight(const optional<std::string>& klass = {}) const;
    void setFillExtrusionHeight(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setFillExtrusionHeightTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionHeightTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultFillExtrusionBase();
    DataDrivenPropertyValue<float> getFillExtrusionBase(const optional<std::string>& klass = {}) const;
    void setFillExtrusionBase(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setFillExtrusionBaseTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getFillExtrusionBaseTransition(const optional<std::string>& klass = {}) const;

    // Private implementation

    class Impl;
    Impl* const impl;

    FillExtrusionLayer(const Impl&);
    FillExtrusionLayer(const FillExtrusionLayer&) = delete;
};

template <>
inline bool Layer::is<FillExtrusionLayer>() const {
    return type == LayerType::FillExtrusion;
}

} // namespace style
} // namespace mbgl
