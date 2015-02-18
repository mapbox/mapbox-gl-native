#ifndef MBGL_STYLE_STYLE_BUCKET
#define MBGL_STYLE_STYLE_BUCKET

#include <mbgl/style/types.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/style_source.hpp>
#include <mbgl/style/class_properties.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>

namespace mbgl {

class Source;

class StyleBucket : public util::noncopyable {
public:
    typedef util::ptr<StyleBucket> Ptr;

    inline StyleBucket(StyleLayerType type_) : type(type_) {}
    const StyleLayerType type;
    std::string name;
    util::ptr<StyleSource> style_source;
    std::string source_layer;
    FilterExpression filter;
    ClassProperties layout;
    float min_zoom = -std::numeric_limits<float>::infinity();
    float max_zoom = std::numeric_limits<float>::infinity();
    VisibilityType visibility = VisibilityType::Visible;
};

};

#endif
