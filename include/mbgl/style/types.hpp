#pragma once

#include <mbgl/util/enum.hpp>

#include <string>
#include <array>
#include <vector>
#include <utility>

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
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    Function(const T& constant)
        : stops({{ 0, constant }}) {}

    explicit Function(const Stops& stops_, float base_)
        : base(base_), stops(stops_) {}

    float getBase() const { return base; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    float base = 1;
    std::vector<std::pair<float, T>> stops;
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

enum class LineCapType : uint8_t {
    Round,
    Butt,
    Square,
};

enum class LineJoinType : uint8_t {
    Miter,
    Bevel,
    Round,
    // the following two types are for internal use only
    FakeRound,
    FlipBevel
};

enum class TranslateAnchorType : bool {
    Map,
    Viewport
};

enum class RotateAnchorType : bool {
    Map,
    Viewport,
};

enum class SymbolPlacementType : bool {
    Point,
    Line,
};

enum class RotationAlignmentType : bool {
    Map,
    Viewport,
};

enum class TextJustifyType : uint8_t {
    Center,
    Left,
    Right
};

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

enum class TextTransformType : uint8_t {
    None,
    Uppercase,
    Lowercase,
};

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

