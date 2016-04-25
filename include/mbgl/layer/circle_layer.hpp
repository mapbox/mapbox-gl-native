// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {

class CircleLayer : public Layer {
public:
    CircleLayer(const std::string& layerID);
    ~CircleLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    // Paint properties

    Function<float> getCircleRadius() const;
    void setCircleRadius(Function<float>);

    Function<Color> getCircleColor() const;
    void setCircleColor(Function<Color>);

    Function<float> getCircleBlur() const;
    void setCircleBlur(Function<float>);

    Function<float> getCircleOpacity() const;
    void setCircleOpacity(Function<float>);

    Function<std::array<float, 2>> getCircleTranslate() const;
    void setCircleTranslate(Function<std::array<float, 2>>);

    Function<TranslateAnchorType> getCircleTranslateAnchor() const;
    void setCircleTranslateAnchor(Function<TranslateAnchorType>);

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

} // namespace mbgl
