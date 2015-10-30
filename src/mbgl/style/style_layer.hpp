#ifndef MBGL_STYLE_STYLE_LAYER
#define MBGL_STYLE_STYLE_LAYER

#include <mbgl/style/types.hpp>
#include <mbgl/style/paint_properties_map.hpp>

#include <mbgl/renderer/render_pass.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/ptr.hpp>

#include <rapidjson/document.h>

#include <vector>
#include <string>

namespace mbgl {

class StyleBucket;
class StyleCalculationParameters;
class PropertyTransition;

using JSVal = rapidjson::Value;

class StyleLayer : public util::noncopyable {
public:
    static std::unique_ptr<StyleLayer> create(StyleLayerType);

    virtual ~StyleLayer() = default;

    virtual void parseLayout(const JSVal& value) = 0;
    virtual void parsePaints(const JSVal& value) = 0;

    // Partially evaluate paint properties based on a set of classes.
    void cascade(const std::vector<std::string>& classNames,
                 const TimePoint& now,
                 const PropertyTransition& defaultTransition);

    // Fully evaluate cascaded paint properties based on a zoom level.
    virtual void recalculate(const StyleCalculationParameters&) = 0;

    // Checks whether this layer has any active paint properties with transitions.
    bool hasTransitions() const;

    // Checks whether this layer needs to be rendered in the given render pass.
    bool hasRenderPass(RenderPass) const;

public:
    std::string id;
    StyleLayerType type = StyleLayerType::Unknown;

    // Bucket information, telling the renderer how to generate the geometries
    // for this layer (feature property filters, tessellation instructions, ...).
    util::ptr<StyleBucket> bucket;

    // Contains all paint classes that can be applied to this layer.
    PaintPropertiesMap paints;

protected:
    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;
};

}

#endif
