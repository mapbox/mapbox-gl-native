// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>

#include <vector>

namespace mbgl {

class LineLayer : public Layer {
public:
    LineLayer(const std::string& layerID);
    ~LineLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    // Layout properties

    Function<LineCapType> getLineCap() const;
    void setLineCap(Function<LineCapType>);

    Function<LineJoinType> getLineJoin() const;
    void setLineJoin(Function<LineJoinType>);

    Function<float> getLineMiterLimit() const;
    void setLineMiterLimit(Function<float>);

    Function<float> getLineRoundLimit() const;
    void setLineRoundLimit(Function<float>);

    // Paint properties

    Function<float> getLineOpacity() const;
    void setLineOpacity(Function<float>);

    Function<Color> getLineColor() const;
    void setLineColor(Function<Color>);

    Function<std::array<float, 2>> getLineTranslate() const;
    void setLineTranslate(Function<std::array<float, 2>>);

    Function<TranslateAnchorType> getLineTranslateAnchor() const;
    void setLineTranslateAnchor(Function<TranslateAnchorType>);

    Function<float> getLineWidth() const;
    void setLineWidth(Function<float>);

    Function<float> getLineGapWidth() const;
    void setLineGapWidth(Function<float>);

    Function<float> getLineOffset() const;
    void setLineOffset(Function<float>);

    Function<float> getLineBlur() const;
    void setLineBlur(Function<float>);

    Function<std::vector<float>> getLineDasharray() const;
    void setLineDasharray(Function<std::vector<float>>);

    Function<std::string> getLinePattern() const;
    void setLinePattern(Function<std::string>);

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

} // namespace mbgl
