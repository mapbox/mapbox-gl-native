#ifndef MBGL_STYLE_STYLE_BUCKET
#define MBGL_STYLE_STYLE_BUCKET

#include <mbgl/style/types.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/style_source.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/variant.hpp>

#include <memory>
#include <forward_list>

namespace mbgl {

class Source;

class StyleBucketFill {
public:
    WindingType winding = WindingType::Default;
};

class StyleBucketLine {
public:
    CapType cap = CapType::Default;
    JoinType join = JoinType::Default;
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
};

class StyleBucketIcon {
public:
    uint16_t size = 16;
    vec2<float> translate {0, 0};
    TranslateAnchorType translate_anchor = TranslateAnchorType::Default;
    std::string icon;
    float spacing = 0.0f;
    float padding = 2.0f;
};

class StyleBucketText {
public:
    std::string field;
    TextPathType path = TextPathType::Default;
    TextTransformType transform = TextTransformType::Default;
    std::string font;
    float max_size = 16.0f;
    float max_width = 15.0f * 24;
    float line_height = 1.2f * 24;
    float letter_spacing = 0.0f;
    float alignment = 0.5f;
    float vertical_alignment = 0.5;
    vec2<float> translate {0, 0};
    TranslateAnchorType translate_anchor = TranslateAnchorType::Default;
    float max_angle_delta = 45.0f;
    float min_distance = 250.0f;
    float rotate = 0.0f; // what is this?
    float padding = 2.0f;
    float slant = 0.0f;
    bool always_visible = false;
};

class StyleBucketRaster {
public:
};

typedef util::variant<StyleBucketFill, StyleBucketLine, StyleBucketIcon,
                      StyleBucketText, StyleBucketRaster,
                      std::false_type> StyleBucketRender;


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
