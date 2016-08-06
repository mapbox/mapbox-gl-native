// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(const std::string& layerID);
    ~BackgroundLayer() final;

    // Paint properties

    PropertyValue<Color> getBackgroundColor() const;
    void setBackgroundColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    PropertyValue<std::string> getBackgroundPattern() const;
    void setBackgroundPattern(PropertyValue<std::string>, const optional<std::string>& klass = {});

    PropertyValue<float> getBackgroundOpacity() const;
    void setBackgroundOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    // Private implementation

    class Impl;
    Impl* const impl;

    BackgroundLayer(const Impl&);
    BackgroundLayer(const BackgroundLayer&) = delete;
};

template <>
inline bool Layer::is<BackgroundLayer>() const {
    return type == Type::Background;
}

} // namespace style
} // namespace mbgl
