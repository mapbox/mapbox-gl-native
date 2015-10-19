#ifndef MBGL_STYLE_STYLE_LAYER
#define MBGL_STYLE_STYLE_LAYER

#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/applied_class_properties.hpp>
#include <mbgl/style/zoom_history.hpp>
#include <mbgl/style/property_evaluator.hpp>

#include <mbgl/renderer/render_pass.hpp>

#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/interpolate.hpp>

#include <vector>
#include <string>
#include <map>
#include <set>

namespace mbgl {

class StyleBucket;

class StyleLayer : public util::noncopyable {
public:
    static std::unique_ptr<StyleLayer> create(StyleLayerType);

    virtual ~StyleLayer() = default;

    // Determines whether this layer is the background layer.
    bool isBackground() const;

    // Checks whether this layer needs to be rendered in the given render pass.
    bool hasRenderPass(RenderPass) const;

    // Updates the StyleProperties information in this layer by evaluating all
    // pending transitions and applied classes in order.
    void updateProperties(float z, const TimePoint& now, ZoomHistory &zoomHistory);

    // Sets the list of classes and creates transitions to the currently applied values.
    void setClasses(const std::vector<std::string> &class_names, const TimePoint& now,
                    const PropertyTransition &defaultTransition);

    bool hasTransitions() const;

public:
    // The name of this layer.
    std::string id;

    StyleLayerType type = StyleLayerType::Unknown;

    // Bucket information, telling the renderer how to generate the geometries
    // for this layer (feature property filters, tessellation instructions, ...).
    util::ptr<StyleBucket> bucket;

    // Contains all style classes that can be applied to this layer.
    std::map<ClassID, ClassProperties> styles;

protected:
    // TODO: extract
    template <typename T>
    void applyStyleProperty(PropertyKey key, T& target, const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
        auto it = appliedStyle.find(key);
        if (it != appliedStyle.end()) {
            AppliedClassPropertyValues &applied = it->second;
            // Iterate through all properties that we need to apply in order.
            const PropertyEvaluator<T> evaluator(z, zoomHistory);
            for (auto& property : applied.propertyValues) {
                if (now >= property.begin) {
                    // We overwrite the current property with the new value.
                    target = mapbox::util::apply_visitor(evaluator, property.value);
                } else {
                    // Do not apply this property because its transition hasn't begun yet.
                }
            }
        }
    }

    template <typename T>
    void applyTransitionedStyleProperty(PropertyKey key, T& target, const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
        auto it = appliedStyle.find(key);
        if (it != appliedStyle.end()) {
            AppliedClassPropertyValues &applied = it->second;
            // Iterate through all properties that we need to apply in order.
            const PropertyEvaluator<T> evaluator(z, zoomHistory);
            for (auto& property : applied.propertyValues) {
                if (now >= property.end) {
                    // We overwrite the current property with the new value.
                    target = mapbox::util::apply_visitor(evaluator, property.value);
                } else if (now >= property.begin) {
                    // We overwrite the current property partially with the new value.
                    float progress = std::chrono::duration<float>(now - property.begin) / (property.end - property.begin);
                    target = util::interpolate(target, mapbox::util::apply_visitor(evaluator, property.value), progress);
                    hasPendingTransitions = true;
                } else {
                    // Do not apply this property because its transition hasn't begun yet.
                }
            }
        }
    }

private:
    // Applies all properties from a class, if they haven't been applied already.
    void applyClassProperties(ClassID class_id, std::set<PropertyKey> &already_applied,
                              const TimePoint& now, const PropertyTransition &defaultTransition);

    // Sets the properties of this object by evaluating all pending transitions and
    // aplied classes in order.
    virtual RenderPass applyStyleProperties(float z, const TimePoint& now, const ZoomHistory&) = 0;

    // Removes all expired style transitions.
    void cleanupAppliedStyleProperties(const TimePoint& now);

    // For every property, stores a list of applied property values, with
    // optional transition times.
    std::map<PropertyKey, AppliedClassPropertyValues> appliedStyle;

    // Stores what render passes this layer is currently enabled for. This depends on the
    // evaluated StyleProperties object and is updated accordingly.
    RenderPass passes = RenderPass::None;

    // Stores whether there are pending transitions to be done on each update.
    bool hasPendingTransitions = false;
};

}

#endif
