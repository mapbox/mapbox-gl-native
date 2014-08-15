#ifndef MBGL_STYLE_STYLE_BUCKET
#define MBGL_STYLE_STYLE_BUCKET

#include <mbgl/style/types.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/vec.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <forward_list>

namespace mbgl {

class Source;

class StyleBucketFill {
public:
    WindingType winding = WindingType::NonZero;
};

class StyleBucketLine {
public:
    CapType cap = CapType::Butt;
    JoinType join = JoinType::Miter;
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
};

class StyleBucketSymbol {
public:
    // Make movable only.
    inline StyleBucketSymbol() = default;
    inline StyleBucketSymbol(StyleBucketSymbol &&) = default;
    inline StyleBucketSymbol& operator=(StyleBucketSymbol &&) = default;
    inline StyleBucketSymbol(const StyleBucketSymbol &) = delete;
    inline StyleBucketSymbol& operator=(const StyleBucketSymbol &) = delete;

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
        vec2<float> offset = {0, 0};
        TranslateAnchorType translate_anchor = TranslateAnchorType::Map;
    } icon;

    struct {
        RotationAlignmentType rotation_alignment = RotationAlignmentType::Viewport;
        std::string field;
        std::string font;
        float max_size = 16.0f;
        float max_width = 15.0f * 24 /* em */;
        float line_height = 1.2f * 24 /* em */;
        float letter_spacing = 0.0f * 24 /* em */;
        TextJustifyType justify = TextJustifyType::Center;
        TextHorizontalAlignType horizontal_align = TextHorizontalAlignType::Center;
        TextVerticalAlignType vertical_align = TextVerticalAlignType::Center;
        float max_angle = 45.0f /* degrees */;
        float rotate = 0.0f;
        float slant = 0.0f;
        float padding = 2.0f;
        bool keep_upright = true;
        TextTransformType transform = TextTransformType::None;
        vec2<float> offset = {0, 0};
        TranslateAnchorType translate_anchor = TranslateAnchorType::Map;
        bool allow_overlap = false;
        bool ignore_placement = false;
        bool optional = false;
    } text;
};

class StyleBucketRaster {
public:
    bool prerendered = false;
    uint16_t size = 256;
    float blur = 0.0f;
    float buffer = 0.03125f;
};

typedef util::variant<StyleBucketFill, StyleBucketLine, StyleBucketSymbol,
                      StyleBucketRaster, std::false_type> StyleBucketRender;


class StyleBucket {
public:
    typedef std::shared_ptr<StyleBucket> Ptr;

    StyleBucket(StyleLayerType type);

    std::string name;
    std::shared_ptr<StyleSource> style_source;
    std::string source_layer;
    FilterExpression filter;
    StyleBucketRender render = std::false_type();
    float min_zoom = -std::numeric_limits<float>::infinity();
    float max_zoom = std::numeric_limits<float>::infinity();
};



};

#endif
