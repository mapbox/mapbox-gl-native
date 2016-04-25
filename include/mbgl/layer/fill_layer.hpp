// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {

class FillLayer : public Layer {
public:
    FillLayer(const std::string& layerID);
    ~FillLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    // Paint properties

    Function<bool> getFillAntialias() const;
    void setFillAntialias(Function<bool>);

    Function<float> getFillOpacity() const;
    void setFillOpacity(Function<float>);

    Function<Color> getFillColor() const;
    void setFillColor(Function<Color>);

    Function<Color> getFillOutlineColor() const;
    void setFillOutlineColor(Function<Color>);

    Function<std::array<float, 2>> getFillTranslate() const;
    void setFillTranslate(Function<std::array<float, 2>>);

    Function<TranslateAnchorType> getFillTranslateAnchor() const;
    void setFillTranslateAnchor(Function<TranslateAnchorType>);

    Function<std::string> getFillPattern() const;
    void setFillPattern(Function<std::string>);

    // Private implementation

    class Impl;
    Impl* const impl;

    FillLayer(const Impl&);
    FillLayer(const FillLayer&) = delete;
};

template <>
inline bool Layer::is<FillLayer>() const {
    return type == Type::Fill;
}

} // namespace mbgl
