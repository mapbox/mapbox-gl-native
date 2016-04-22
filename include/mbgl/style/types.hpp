#ifndef MBGL_STYLE_TYPES
#define MBGL_STYLE_TYPES

#include <mbgl/util/enum.hpp>

#include <string>
#include <array>
#include <vector>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
using Color = std::array<float, 4>;

// An array of font names
using FontStack = std::vector<std::string>;

std::string fontStackToString(const FontStack&);

struct FontStackHash {
    std::size_t operator()(const FontStack&) const;
};

template <typename T>
struct Faded {
    Faded() = default;
    Faded(const T& v) : to(v) {}

    T from;
    float fromScale = 0;
    T to;
    float toScale = 0;
    float t = 0;
};

// -------------------------------------------------------------------------------------------------

enum class SourceType : uint8_t {
    Vector,
    Raster,
    GeoJSON,
    Video,
    Annotations
};

MBGL_DEFINE_ENUM_CLASS(SourceTypeClass, SourceType, {
    { SourceType::Vector, "vector" },
    { SourceType::Raster, "raster" },
    { SourceType::GeoJSON, "geojson" },
    { SourceType::Video, "video" },
    { SourceType::Annotations, "annotations" },
});

// -------------------------------------------------------------------------------------------------

enum class VisibilityType : bool {
    Visible,
    None,
};

MBGL_DEFINE_ENUM_CLASS(VisibilityTypeClass, VisibilityType, {
    { VisibilityType::Visible, "visible" },
    { VisibilityType::None, "none" },
});

// -------------------------------------------------------------------------------------------------

enum class LineCapType : uint8_t {
    Round,
    Butt,
    Square,
};

MBGL_DEFINE_ENUM_CLASS(LineCapTypeClass, LineCapType, {
    { LineCapType::Round, "round" },
    { LineCapType::Butt, "butt" },
    { LineCapType::Square, "square" },
});

// -------------------------------------------------------------------------------------------------

enum class LineJoinType : uint8_t {
    Miter,
    Bevel,
    Round,
    // the following two types are for internal use only
    FakeRound,
    FlipBevel
};

MBGL_DEFINE_ENUM_CLASS(LineJoinTypeClass, LineJoinType, {
    { LineJoinType::Miter, "miter" },
    { LineJoinType::Bevel, "bevel" },
    { LineJoinType::Round, "round" },
    { LineJoinType::FakeRound, "fakeround" },
    { LineJoinType::FlipBevel, "flipbevel" },
});

// -------------------------------------------------------------------------------------------------

enum class TranslateAnchorType : bool {
    Map,
    Viewport
};

MBGL_DEFINE_ENUM_CLASS(TranslateAnchorTypeClass, TranslateAnchorType, {
    { TranslateAnchorType::Map, "map" },
    { TranslateAnchorType::Viewport, "viewport" },
});

// -------------------------------------------------------------------------------------------------

enum class RotateAnchorType : bool {
    Map,
    Viewport,
};

MBGL_DEFINE_ENUM_CLASS(RotateAnchorTypeClass, RotateAnchorType, {
    { RotateAnchorType::Map, "map" },
    { RotateAnchorType::Viewport, "viewport" },
});

// -------------------------------------------------------------------------------------------------

enum class SymbolPlacementType : bool {
    Point,
    Line,
};

MBGL_DEFINE_ENUM_CLASS(SymbolPlacementTypeClass, SymbolPlacementType, {
    { SymbolPlacementType::Point, "point" },
    { SymbolPlacementType::Line, "line" },
});

// -------------------------------------------------------------------------------------------------

enum class RotationAlignmentType : bool {
    Map,
    Viewport,
};

MBGL_DEFINE_ENUM_CLASS(RotationAlignmentTypeClass, RotationAlignmentType, {
    { RotationAlignmentType::Map, "map" },
    { RotationAlignmentType::Viewport, "viewport" },
});

// -------------------------------------------------------------------------------------------------

enum class TextJustifyType : uint8_t {
    Center,
    Left,
    Right
};

MBGL_DEFINE_ENUM_CLASS(TextJustifyTypeClass, TextJustifyType, {
    { TextJustifyType::Center, "center" },
    { TextJustifyType::Left, "left" },
    { TextJustifyType::Right, "right" },
});

// -------------------------------------------------------------------------------------------------

enum class TextAnchorType : uint8_t {
    Center,
    Left,
    Right,
    Top,
    Bottom,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

MBGL_DEFINE_ENUM_CLASS(TextAnchorTypeClass, TextAnchorType, {
    { TextAnchorType::Center, "center" },
    { TextAnchorType::Left, "left" },
    { TextAnchorType::Right, "right" },
    { TextAnchorType::Top, "top" },
    { TextAnchorType::Bottom, "bottom" },
    { TextAnchorType::TopLeft, "top-left" },
    { TextAnchorType::TopRight, "top-right" },
    { TextAnchorType::BottomLeft, "bottom-left" },
    { TextAnchorType::BottomRight, "bottom-right" }
});

// -------------------------------------------------------------------------------------------------

enum class TextTransformType : uint8_t {
    None,
    Uppercase,
    Lowercase,
};

MBGL_DEFINE_ENUM_CLASS(TextTransformTypeClass, TextTransformType, {
    { TextTransformType::None, "none" },
    { TextTransformType::Uppercase, "uppercase" },
    { TextTransformType::Lowercase, "lowercase" },
});

/**
 * Initialize any GL state needed by the custom layer. This method is called once, from the
 * rendering thread, at a point when the GL context is active but before rendering for the
 * first time.
 *
 * Resources that are acquired in this method must be released in the UninitializeFunction.
 */
using CustomLayerInitializeFunction = void (*)(void* context);

/**
 * Parameters that define the current camera position for a CustomLayerRenderFunction.
 */
struct CustomLayerRenderParameters {
    double width;
    double height;
    double latitude;
    double longitude;
    double zoom;
    double bearing;
    double pitch;
    double altitude;
};

/**
 * Render the layer. This method is called once per frame. The implementation should not make
 * any assumptions about the GL state (other than that the correct context is active). It may
 * make changes to the state, and is not required to reset values such as the depth mask, stencil
 * mask, and corresponding test flags to their original values.
 */
using CustomLayerRenderFunction = void (*)(void* context, const CustomLayerRenderParameters&);

/**
 * Destroy any GL state needed by the custom layer, and deallocate context, if necessary. This
 * method is called once, from the rendering thread, at a point when the GL context is active.
 *
 * Note that it may be called even when the InitializeFunction has not been called.
 */
using CustomLayerDeinitializeFunction = void (*)(void* context);

} // namespace mbgl

#endif

