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
    typedef std::shared_ptr<StyleLayer> Ptr;

public:
    template <typename T> const T &getProperties() {
        if (style.is<T>()) {
            return style.get<T>();
        } else {
            return defaultStyleProperties<T>();
        }
    }

public:
    std::string id;
    std::shared_ptr<StyleBucket> bucket;
    std::map<ClassID, ClassProperties> styles;
    StyleProperties style;
    std::unique_ptr<RasterizeProperties> rasterize;
    std::shared_ptr<StyleLayerGroup> layers;
};


}

#endif
