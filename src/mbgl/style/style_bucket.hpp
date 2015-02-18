#ifndef MBGL_STYLE_STYLE_BUCKET
#define MBGL_STYLE_STYLE_BUCKET

#include <mbgl/style/types.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/vec.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/style/class_properties.hpp>

#include <forward_list>

namespace mbgl {

class Source;

class StyleBucketFill {
public:
    // Make movable only.
    StyleBucketFill() = default;
    StyleBucketFill(StyleBucketFill &&) = default;
    StyleBucketFill& operator=(StyleBucketFill &&) = default;
    StyleBucketFill(const StyleBucketFill &) = delete;
    StyleBucketFill& operator=(const StyleBucketFill &) = delete;
};

class StyleBucketLine {
public:
    // Make movable only.
    StyleBucketLine() = default;
    StyleBucketLine(StyleBucketLine &&) = default;
    StyleBucketLine& operator=(StyleBucketLine &&) = default;
    StyleBucketLine(const StyleBucketLine &) = delete;
    StyleBucketLine& operator=(const StyleBucketLine &) = delete;

    CapType cap = CapType::Butt;
    JoinType join = JoinType::Miter;
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
};

class StyleBucketSymbol {
public:
    // Make movable only.
    StyleBucketSymbol() = default;
    StyleBucketSymbol(StyleBucketSymbol &&) = default;
    StyleBucketSymbol& operator=(StyleBucketSymbol &&) = default;
    StyleBucketSymbol(const StyleBucketSymbol &) = delete;
    StyleBucketSymbol& operator=(const StyleBucketSymbol &) = delete;

    PlacementType placement = PlacementType::Point;
    float min_distance = 250.0f;
    bool avoid_edges = false;

    struct {
        bool allow_overlap = false;
        bool ignore_placement = false;
        bool optional = false;
        RotationAlignmentType rotation_alignment = RotationAlignmentType::Viewport;
        float max_size = 1.0f;
        std::string image;
        float rotate = 0.0f;
        float padding = 2.0f;
        bool keep_upright = false;
        std::array<float, 2> offset = {{ 0, 0 }};
    } icon;

    struct {
        RotationAlignmentType rotation_alignment = RotationAlignmentType::Viewport;
        std::string field;
        std::string font;
        float max_size = 16.0f;
        float max_width = 15.0f /* em */;
        float line_height = 1.2f /* em */;
        float letter_spacing = 0.0f /* em */;
        TextJustifyType justify = TextJustifyType::Center;
        TextAnchorType anchor = TextAnchorType::Center;
        float max_angle = 45.0f /* degrees */;
        float rotate = 0.0f;
        float padding = 2.0f;
        bool keep_upright = true;
        TextTransformType transform = TextTransformType::None;
        std::array<float, 2> offset = {{ 0, 0 }};
        bool allow_overlap = false;
        bool ignore_placement = false;
        bool optional = false;
    } text;
};

class StyleBucketRaster {
public:
    // Make movable only.
    StyleBucketRaster() = default;
    StyleBucketRaster(StyleBucketRaster &&) = default;
    StyleBucketRaster& operator=(StyleBucketRaster &&) = default;
    StyleBucketRaster(const StyleBucketRaster &) = delete;
    StyleBucketRaster& operator=(const StyleBucketRaster &) = delete;
};

class StyleBucketBackground {
public:
};

class StyleBucket : public util::noncopyable {
public:
    typedef util::ptr<StyleBucket> Ptr;

    StyleBucket(StyleLayerType type);
    StyleLayerType type;
    std::string name;
    util::ptr<StyleSource> style_source;
    std::string source_layer;
    FilterExpression filter;
    ClassProperties layout;
    float min_zoom = -std::numeric_limits<float>::infinity();
    float max_zoom = std::numeric_limits<float>::infinity();
    VisibilityType visibility = VisibilityType::Visible;
};

template <typename T>
const T &defaultLayoutProperties();

};

#endif
