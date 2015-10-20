#ifndef MBGL_STYLE_STYLE_LAYER
#define MBGL_STYLE_STYLE_LAYER

#include <mbgl/style/types.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/paint_properties_map.hpp>

#include <mbgl/renderer/render_pass.hpp>
#include <mbgl/map/tile_data.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/ptr.hpp>

#include <rapidjson/document.h>

#include <vector>
#include <string>

namespace mbgl {

class StyleCalculationParameters;
class StyleBucketParameters;
class PropertyTransition;

using JSVal = rapidjson::Value;

class StyleLayer : public util::noncopyable {
public:
    static std::unique_ptr<StyleLayer> create(StyleLayerType);

    virtual ~StyleLayer() = default;

    virtual void parseLayout(const JSVal& value) = 0;
    virtual void parsePaints(const JSVal& value) = 0;

    // If the layer has a ref, the ref. Otherwise, the id.
    const std::string& bucketName() const;

    // Partially evaluate paint properties based on a set of classes.
    void cascade(const std::vector<std::string>& classNames,
                 const TimePoint& now,
                 const PropertyTransition& defaultTransition);

    // Fully evaluate cascaded paint properties based on a zoom level.
    virtual void recalculate(const StyleCalculationParameters&) = 0;

    virtual std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const = 0;

    // Checks whether this layer has any active paint properties with transitions.
    bool hasTransitions() const;

    // Checks whether this layer needs to be rendered in the given render pass.
    bool hasRenderPass(RenderPass) const;

public:
    StyleLayerType type;
    std::string id;
    std::string ref;
    std::string source;
    std::string sourceLayer;
    FilterExpression filter;
    float minZoom = -std::numeric_limits<float>::infinity();
    float maxZoom = std::numeric_limits<float>::infinity();
    VisibilityType visibility = VisibilityType::Visible;
    ClassProperties layout;
    PaintPropertiesMap paints;

protected:
    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;
};

}

#endif
