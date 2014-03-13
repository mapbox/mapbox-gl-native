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
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

public:
    Style();

    void reset();

    void loadJSON(const uint8_t *const data, size_t bytes);

    void cascade(float z);

private:
    static std::pair<std::string, BucketDescription> parseBucket(pbf data);
    static LayerDescription parseLayer(pbf data);
    static std::pair<std::string, ClassDescription> parseClass(pbf data);
    static std::pair<std::string, FillClass> parseFillClass(pbf data);
    static std::pair<std::string, LineClass> parseLineClass(pbf data);
    static std::pair<std::string, PointClass> parsePointClass(pbf data);
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
        BackgroundProperties background;
        std::map<std::string, FillProperties> fills;
        std::map<std::string, LineProperties> lines;
        std::map<std::string, PointProperties> points;
    } computed;
};

}



#endif