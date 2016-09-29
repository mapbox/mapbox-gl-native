// This file is generated. Edit android/platform/scripts/generate-style-code.js, then run `make style-code-android`.
#pragma once

#include <mbgl/style/types.hpp>

#include <string>
#include <stdexcept>

namespace mbgl {
namespace android {
namespace conversion {

    //visibility
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

    //line-cap
    inline std::string toString(mbgl::style::LineCapType value) {
        switch (value) {
          //A cap with a squared-off end which is drawn to the exact endpoint of the line.
          case mbgl::style::LineCapType::Butt:
            return "butt";
            break;
          //A cap with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
          case mbgl::style::LineCapType::Round:
            return "round";
            break;
          //A cap with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
          case mbgl::style::LineCapType::Square:
            return "square";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //line-join
    inline std::string toString(mbgl::style::LineJoinType value) {
        switch (value) {
          //A join with a squared-off end which is drawn beyond the endpoint of the line at a distance of one-half of the line's width.
          case mbgl::style::LineJoinType::Bevel:
            return "bevel";
            break;
          //A join with a rounded end which is drawn beyond the endpoint of the line at a radius of one-half of the line's width and centered on the endpoint of the line.
          case mbgl::style::LineJoinType::Round:
            return "round";
            break;
          //A join with a sharp, angled corner which is drawn with the outer sides beyond the endpoint of the path until they meet.
          case mbgl::style::LineJoinType::Miter:
            return "miter";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //symbol-placement
    inline std::string toString(mbgl::style::SymbolPlacementType value) {
        switch (value) {
          //The label is placed at the point where the geometry is located.
          case mbgl::style::SymbolPlacementType::Point:
            return "point";
            break;
          //The label is placed along the line of the geometry. Can only be used on `LineString` and `Polygon` geometries.
          case mbgl::style::SymbolPlacementType::Line:
            return "line";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //icon-rotation-alignment
    inline std::string toString(mbgl::style::AlignmentType value) {
        switch (value) {
          //When `symbol-placement` is set to `point`, aligns icons east-west. When `symbol-placement` is set to `line`, aligns icon x-axes with the line.
          case mbgl::style::AlignmentType::Map:
            return "map";
            break;
          //Produces icons whose x-axes are aligned with the x-axis of the viewport, regardless of the value of `symbol-placement`.
          case mbgl::style::AlignmentType::Viewport:
            return "viewport";
            break;
          //When `symbol-placement` is set to `point`, this is equivalent to `viewport`. When `symbol-placement` is set to `line`, this is equivalent to `map`.
          case mbgl::style::AlignmentType::Auto:
            return "auto";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //icon-text-fit
    inline std::string toString(mbgl::style::IconTextFitType value) {
        switch (value) {
          //The icon is displayed at its intrinsic aspect ratio.
          case mbgl::style::IconTextFitType::None:
            return "none";
            break;
          //The icon is scaled in the x-dimension to fit the width of the text.
          case mbgl::style::IconTextFitType::Width:
            return "width";
            break;
          //The icon is scaled in the y-dimension to fit the height of the text.
          case mbgl::style::IconTextFitType::Height:
            return "height";
            break;
          //The icon is scaled in both x- and y-dimensions.
          case mbgl::style::IconTextFitType::Both:
            return "both";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //text-justify
    inline std::string toString(mbgl::style::TextJustifyType value) {
        switch (value) {
          //The text is aligned to the left.
          case mbgl::style::TextJustifyType::Left:
            return "left";
            break;
          //The text is centered.
          case mbgl::style::TextJustifyType::Center:
            return "center";
            break;
          //The text is aligned to the right.
          case mbgl::style::TextJustifyType::Right:
            return "right";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //text-anchor
    inline std::string toString(mbgl::style::TextAnchorType value) {
        switch (value) {
          //The center of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::Center:
            return "center";
            break;
          //The left side of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::Left:
            return "left";
            break;
          //The right side of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::Right:
            return "right";
            break;
          //The top of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::Top:
            return "top";
            break;
          //The bottom of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::Bottom:
            return "bottom";
            break;
          //The top left corner of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::TopLeft:
            return "top-left";
            break;
          //The top right corner of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::TopRight:
            return "top-right";
            break;
          //The bottom left corner of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::BottomLeft:
            return "bottom-left";
            break;
          //The bottom right corner of the text is placed closest to the anchor.
          case mbgl::style::TextAnchorType::BottomRight:
            return "bottom-right";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //text-transform
    inline std::string toString(mbgl::style::TextTransformType value) {
        switch (value) {
          //The text is not altered.
          case mbgl::style::TextTransformType::None:
            return "none";
            break;
          //Forces all letters to be displayed in uppercase.
          case mbgl::style::TextTransformType::Uppercase:
            return "uppercase";
            break;
          //Forces all letters to be displayed in lowercase.
          case mbgl::style::TextTransformType::Lowercase:
            return "lowercase";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //fill-translate-anchor
    inline std::string toString(mbgl::style::TranslateAnchorType value) {
        switch (value) {
          //The fill is translated relative to the map.
          case mbgl::style::TranslateAnchorType::Map:
            return "map";
            break;
          //The fill is translated relative to the viewport.
          case mbgl::style::TranslateAnchorType::Viewport:
            return "viewport";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }

    //circle-pitch-scale
    inline std::string toString(mbgl::style::CirclePitchScaleType value) {
        switch (value) {
          //Circles are scaled according to their apparent distance to the camera.
          case mbgl::style::CirclePitchScaleType::Map:
            return "map";
            break;
          //Circles are not scaled.
          case mbgl::style::CirclePitchScaleType::Viewport:
            return "viewport";
            break;
          default:
            throw std::runtime_error("Not implemented");
        }
    }


} // namespace conversion
} // namespace android
} // namespace mbgl