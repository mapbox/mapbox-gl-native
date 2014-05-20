#ifndef LLMR_STYLE_STYLE
#define LLMR_STYLE_STYLE

#include <cstdint>

#include <llmr/util/pbf.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/style/layer_description.hpp>
#include <llmr/style/class_description.hpp>
#include <llmr/geometry/sprite_atlas.hpp>

#include <map>
#include <vector>
#include <set>
#include <memory>

namespace llmr {

class Sprite;

class Style {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

public:
    Style();

    void reset();

    void loadJSON(const uint8_t *const data, size_t bytes);

    size_t layerCount() const;
    void cascade(float z);

public:
    std::shared_ptr<Sprite> sprite;

    // This is static information parsed from the stylesheet.
    std::map<std::string, BucketDescription> buckets;
    std::vector<LayerDescription> layers;
    std::map<std::string, ClassDescription> classes;


    // This are applied settings.
    std::set<std::string> appliedClasses;
    struct {
        BackgroundProperties background;
        std::map<std::string, FillProperties> fills;
        std::map<std::string, LineProperties> lines;
        std::map<std::string, IconProperties> icons;
        std::map<std::string, TextProperties> texts;
        std::map<std::string, RasterProperties> rasters;
        std::map<std::string, CompositeProperties> composites;
    } computed;
};

}



#endif