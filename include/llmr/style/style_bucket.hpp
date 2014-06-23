#ifndef LLMR_STYLE_STYLE_BUCKET
#define LLMR_STYLE_STYLE_BUCKET

#include <llmr/style/types.hpp>
#include <llmr/map/filter_expression.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/util/variant.hpp>

namespace llmr {

class Source;

class StyleBucketFill {
public:
    WindingType winding = parseWindingType();
};

class StyleBucketLine {
public:
    CapType cap = parseCapType();
    JoinType join = parseJoinType();
    float miter_limit = 2.0f;
    float round_limit = 1.0f;
};

class StyleBucketIcon {
public:
    uint16_t size = 16;
    vec2<float> translate {0, 0};
    TranslateAnchorType translate_anchor = parseTranslateAnchorType();
    std::string icon;
    float spacing = 0.0f;
    float padding = 2.0f;
};

class StyleBucketText {
public:
    std::string field;
    TextPathType path = parseTextPathType();
    std::string font;
    float max_size = 16.0f;
    float max_width = 15.0f * 24;
    float line_height = 1.2f * 24;
    float letter_spacing = 0.0f;
    float alignment = 0.5f;
    float vertical_alignment = 0.5;
    vec2<float> translate {0, 0};
    TranslateAnchorType translate_anchor = parseTranslateAnchorType();
    float max_angle_delta = M_PI;
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

    BucketType type = BucketType::None;
    std::string name;
    std::shared_ptr<Source> source;
    std::string source_layer;
    PropertyFilterExpression filter = std::true_type();
    StyleBucketRender render = std::false_type();
};



};

#endif
