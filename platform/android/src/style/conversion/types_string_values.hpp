// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make android-style-code`.
#pragma once

#include <mbgl/style/types.hpp>

#include <string>
#include <stdexcept>

namespace mbgl {
namespace android {
namespace conversion {

    // visibility
    inline std::string toString(mbgl::style::VisibilityType value) {
        switch (value) {
          case mbgl::style::VisibilityType::Visible:
            return "visible";
            break;
          case mbgl::style::VisibilityType::None:
            return "none";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // line-cap
    inline std::string toString(mbgl::style::LineCapType value) {
        switch (value) {
          case mbgl::style::LineCapType::Butt:
            return "butt";
            break;
          case mbgl::style::LineCapType::Round:
            return "round";
            break;
          case mbgl::style::LineCapType::Square:
            return "square";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // line-join
    inline std::string toString(mbgl::style::LineJoinType value) {
        switch (value) {
          case mbgl::style::LineJoinType::Bevel:
            return "bevel";
            break;
          case mbgl::style::LineJoinType::Round:
            return "round";
            break;
          case mbgl::style::LineJoinType::Miter:
            return "miter";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // symbol-placement
    inline std::string toString(mbgl::style::SymbolPlacementType value) {
        switch (value) {
          case mbgl::style::SymbolPlacementType::Point:
            return "point";
            break;
          case mbgl::style::SymbolPlacementType::Line:
            return "line";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // icon-rotation-alignment
    inline std::string toString(mbgl::style::AlignmentType value) {
        switch (value) {
          case mbgl::style::AlignmentType::Map:
            return "map";
            break;
          case mbgl::style::AlignmentType::Viewport:
            return "viewport";
            break;
          case mbgl::style::AlignmentType::Auto:
            return "auto";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // icon-text-fit
    inline std::string toString(mbgl::style::IconTextFitType value) {
        switch (value) {
          case mbgl::style::IconTextFitType::None:
            return "none";
            break;
          case mbgl::style::IconTextFitType::Width:
            return "width";
            break;
          case mbgl::style::IconTextFitType::Height:
            return "height";
            break;
          case mbgl::style::IconTextFitType::Both:
            return "both";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // icon-anchor
    inline std::string toString(mbgl::style::SymbolAnchorType value) {
        switch (value) {
          case mbgl::style::SymbolAnchorType::Center:
            return "center";
            break;
          case mbgl::style::SymbolAnchorType::Left:
            return "left";
            break;
          case mbgl::style::SymbolAnchorType::Right:
            return "right";
            break;
          case mbgl::style::SymbolAnchorType::Top:
            return "top";
            break;
          case mbgl::style::SymbolAnchorType::Bottom:
            return "bottom";
            break;
          case mbgl::style::SymbolAnchorType::TopLeft:
            return "top-left";
            break;
          case mbgl::style::SymbolAnchorType::TopRight:
            return "top-right";
            break;
          case mbgl::style::SymbolAnchorType::BottomLeft:
            return "bottom-left";
            break;
          case mbgl::style::SymbolAnchorType::BottomRight:
            return "bottom-right";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // text-justify
    inline std::string toString(mbgl::style::TextJustifyType value) {
        switch (value) {
          case mbgl::style::TextJustifyType::Left:
            return "left";
            break;
          case mbgl::style::TextJustifyType::Center:
            return "center";
            break;
          case mbgl::style::TextJustifyType::Right:
            return "right";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // text-transform
    inline std::string toString(mbgl::style::TextTransformType value) {
        switch (value) {
          case mbgl::style::TextTransformType::None:
            return "none";
            break;
          case mbgl::style::TextTransformType::Uppercase:
            return "uppercase";
            break;
          case mbgl::style::TextTransformType::Lowercase:
            return "lowercase";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // fill-translate-anchor
    inline std::string toString(mbgl::style::TranslateAnchorType value) {
        switch (value) {
          case mbgl::style::TranslateAnchorType::Map:
            return "map";
            break;
          case mbgl::style::TranslateAnchorType::Viewport:
            return "viewport";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // circle-pitch-scale
    inline std::string toString(mbgl::style::CirclePitchScaleType value) {
        switch (value) {
          case mbgl::style::CirclePitchScaleType::Map:
            return "map";
            break;
          case mbgl::style::CirclePitchScaleType::Viewport:
            return "viewport";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // hillshade-illumination-anchor
    inline std::string toString(mbgl::style::HillshadeIlluminationAnchorType value) {
        switch (value) {
          case mbgl::style::HillshadeIlluminationAnchorType::Map:
            return "map";
            break;
          case mbgl::style::HillshadeIlluminationAnchorType::Viewport:
            return "viewport";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    // anchor
    inline std::string toString(mbgl::style::LightAnchorType value) {
        switch (value) {
          case mbgl::style::LightAnchorType::Map:
            return "map";
            break;
          case mbgl::style::LightAnchorType::Viewport:
            return "viewport";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }


} // namespace conversion
} // namespace android
} // namespace mbgl
