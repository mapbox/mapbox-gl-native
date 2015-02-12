#ifndef MBGL_STYLE_STYLE_PARSER
#define MBGL_STYLE_STYLE_PARSER

#include <rapidjson/document.h>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_source.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <unordered_map>
#include <forward_list>
#include <tuple>

namespace mbgl {

enum class ClassID : uint32_t;

class StyleLayer;
class StyleLayerGroup;

class StyleParser {
public:
    using JSVal = const rapidjson::Value&;

    StyleParser();

    void parse(JSVal document);

    util::ptr<StyleLayerGroup> getLayers() {
        return root;
    }

    std::string getSprite() const {
        return sprite;
    }

    std::string getGlyphURL() const {
        return glyph_url;
    }

private:
    void parseConstants(JSVal value);
    JSVal replaceConstant(JSVal value);

    void parseSources(JSVal value);

    std::unique_ptr<StyleLayerGroup> createLayers(JSVal value);
    util::ptr<StyleLayer> createLayer(JSVal value);
    void parseLayers();
    void parseLayer(std::pair<JSVal, util::ptr<StyleLayer>> &pair);
    void parsePaints(JSVal value, std::map<ClassID, ClassProperties> &paints);
    void parsePaint(JSVal, ClassProperties &properties);
    void parseReference(JSVal value, util::ptr<StyleLayer> &layer);
    void parseBucket(JSVal value, util::ptr<StyleLayer> &layer);
    void parseLayout(JSVal value, util::ptr<StyleBucket> &bucket);
    void parseSprite(JSVal value);
    void parseGlyphURL(JSVal value);

    // Parses optional properties into a render bucket.
    template<typename T>
    bool parseRenderProperty(JSVal value, T &target, const char *name);
    template <typename Parser, typename T>
    bool parseRenderProperty(JSVal value, T &target, const char *name);

    // Parses optional properties into style class properties.
    template <typename T>
    void parseVisibility(StyleBucket &bucket, JSVal value);
    template <typename T>
    bool parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value);
    template <typename T>
    bool parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value, const char *transition_name);
    template <typename T>
    bool setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass);
    template <typename T>
    bool setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass, JSVal transition);

    template <typename T>
    std::tuple<bool, T> parseProperty(JSVal value, const char *property_name);
    template <typename T>
    std::tuple<bool, T> parseProperty(JSVal value, const char *property_name, JSVal transition);

    template <typename T>
    std::tuple<bool, Function<T>> parseFunction(JSVal value, const char *);
    template <typename T>
    std::tuple<bool, PiecewiseConstantFunction<T>> parsePiecewiseConstantFunction(JSVal value, std::chrono::steady_clock::duration duration);
    template <typename T>
    std::tuple<bool, std::vector<std::pair<float, T>>> parseStops(JSVal value, const char *property_name);

    std::tuple<bool,std::vector<float>> parseFloatArray(JSVal value);

    FilterExpression parseFilter(JSVal);

private:
    std::unordered_map<std::string, const rapidjson::Value *> constants;

    std::unordered_map<std::string, const util::ptr<StyleSource>> sources;

    // This stores the root layer.
    util::ptr<StyleLayerGroup> root;

    // This maps ids to Layer objects, with all items being at the root level.
    std::unordered_map<std::string, std::pair<JSVal, util::ptr<StyleLayer>>> layers;

    // Store a stack of layers we're parsing right now. This is to prevent reference cycles.
    std::forward_list<StyleLayer *> stack;

    // Base URL of the sprite image.
    std::string sprite;

    // URL template for glyph PBFs.
    std::string glyph_url;
};

}

#endif
