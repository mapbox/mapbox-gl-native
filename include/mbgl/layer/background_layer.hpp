// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(const std::string& layerID);
    ~BackgroundLayer() final;

    // Paint properties

    Function<Color> getBackgroundColor() const;
    void setBackgroundColor(Function<Color>);

    Function<std::string> getBackgroundPattern() const;
    void setBackgroundPattern(Function<std::string>);

    Function<float> getBackgroundOpacity() const;
    void setBackgroundOpacity(Function<float>);

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

} // namespace mbgl
