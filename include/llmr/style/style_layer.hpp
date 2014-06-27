#ifndef LLMR_STYLE_STYLE_LAYER
#define LLMR_STYLE_STYLE_LAYER

#include <llmr/style/class_dictionary.hpp>
#include <llmr/style/class_properties.hpp>
#include <llmr/style/style_properties.hpp>

#include <vector>
#include <memory>
#include <string>
#include <map>

namespace llmr {

class StyleBucket;
class RasterizeProperties;

class StyleLayer;
typedef std::vector<std::shared_ptr<StyleLayer>> StyleLayerGroup;

class StyleLayer {
public:
    template <typename T> const T &getProperties() {
        if (style.is<T>()) {
            return style.get<T>();
        } else {
            return defaultStyleProperties<T>();
        }
    }

public:
    // The name of this layer.
    std::string id;

    // Bucket information, telling the renderer how to generate the geometries
    // for this layer (feature property filters, tessellation instructions, ...).
    std::shared_ptr<StyleBucket> bucket;

    // Contains all style classes that can be applied to this layer.
    std::map<ClassID, ClassProperties> styles;

    // Stores the evaluated, and cascaded styling information, specific to this
    // layer's type.
    StyleProperties style;

    // Rasterization properties are used for prerendering the tile to a bitmap,
    // which is then used as a raster image instead of rendering this layer
    // directly in every frame.
    std::unique_ptr<RasterizeProperties> rasterize;

    // Child layer array (if this layer has child layers).
    std::shared_ptr<StyleLayerGroup> layers;
};


}

#endif
