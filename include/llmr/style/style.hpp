#ifndef LLMR_STYLE_STYLE
#define LLMR_STYLE_STYLE

#include <cstdint>

#include <llmr/util/pbf.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/style/layer_description.hpp>
#include <llmr/style/class_description.hpp>

#include <map>
#include <vector>
#include <set>
#include <memory>

namespace llmr {

class Sprite;

class Style {
public:
    Style();

    void reset();
    void load(const uint8_t *const data, uint32_t bytes);

    // This is commented out because it is not fully implemented yet. For now,
    // we keep using the protobuf stylesheet format
    // void loadJSON(const std::string& data);

    void cascade(float z);

private:
    static std::pair<std::string, BucketDescription> parseBucket(pbf data);
    static LayerDescription parseLayer(pbf data);
    static std::pair<std::string, ClassDescription> parseClass(pbf data);
    static std::pair<std::string, FillClass> parseFillClass(pbf data);
    static std::pair<std::string, LineClass> parseLineClass(pbf data);
    template <typename T> static FunctionProperty<T> parseProperty(pbf data);
    static Color parseColor(pbf& data);

public:
    std::shared_ptr<Sprite> sprite;

    // This is static information parsed from the stylesheet.
    std::map<std::string, BucketDescription> buckets;
    std::vector<LayerDescription> layers;
    std::map<std::string, ClassDescription> classes;


    // This are applied settings.
    std::set<std::string> appliedClasses;
    struct {
        std::map<std::string, FillProperties> fills;
        std::map<std::string, LineProperties> lines;
    } computed;
};

}



#endif