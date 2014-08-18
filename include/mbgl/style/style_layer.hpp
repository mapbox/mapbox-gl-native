#ifndef MBGL_STYLE_STYLE_LAYER
#define MBGL_STYLE_STYLE_LAYER

#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/applied_class_properties.hpp>

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <set>

namespace mbgl {

class StyleBucket;
class StyleLayerGroup;

class StyleLayer {
public:
    StyleLayer(const std::string &id, std::map<ClassID, ClassProperties> &&styles);

    template <typename T> const T &getProperties() {
        if (properties.is<T>()) {
            return properties.get<T>();
        } else {
            return defaultStyleProperties<T>();
        }
    }

    // Determines whether this layer is the background layer.
    bool isBackground() const;

    // Updates the StyleProperties information in this layer by evaluating all
    // pending transitions and applied classes in order.
    void updateProperties(float z, timestamp now);

    // Sets the list of classes and creates transitions to the currently applied values.
    void setClasses(const std::vector<std::string> &class_names, timestamp now,
                    const PropertyTransition &defaultTransition);

    bool hasTransitions() const;

private:
    // Applies all properties from a class, if they haven't been applied already.
    void applyClassProperties(ClassID class_id, std::set<PropertyKey> &already_applied,
                              timestamp now, const PropertyTransition &defaultTransition);

    // Sets the properties of this object by evaluating all pending transitions and
    // aplied classes in order.
    template <typename T> void applyStyleProperties(float z, timestamp now);
    template <typename T> void applyStyleProperty(PropertyKey key, T &, float z, timestamp now);

    // Removes all expired style transitions.
    void cleanupAppliedStyleProperties(timestamp now);

public:
    // The name of this layer.
    const std::string id;

    StyleLayerType type = StyleLayerType::Unknown;

    // Bucket information, telling the renderer how to generate the geometries
    // for this layer (feature property filters, tessellation instructions, ...).
    std::shared_ptr<StyleBucket> bucket;

    // Contains all style classes that can be applied to this layer.
    const std::map<ClassID, ClassProperties> styles;

private:
    // For every property, stores a list of applied property values, with
    // optional transition times.
    std::map<PropertyKey, AppliedClassProperties> appliedStyle;

public:
    // Stores the evaluated, and cascaded styling information, specific to this
    // layer's type.
    StyleProperties properties;

    // Child layer array (if this layer has child layers).
    std::shared_ptr<StyleLayerGroup> layers;
};

}

#endif
