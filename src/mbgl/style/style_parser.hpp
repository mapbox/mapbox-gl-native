#ifndef MBGL_STYLE_STYLE_PARSER
#define MBGL_STYLE_STYLE_PARSER

#include <rapidjson/document.h>
#include <mbgl/style/style.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <unordered_map>
#include <forward_list>
#include <tuple>

namespace mbgl {

enum class ClassID : uint32_t;

class StyleLayer;
class Source;

class StyleParser {
public:
    using JSVal = const rapidjson::Value&;

    enum Status : bool {
        StyleParserFailure = 0,
        StyleParserSuccess
    };

    template<typename T>
    using Result = std::pair<Status, T>;

    StyleParser(MapData& data);

    void parse(JSVal document);

    std::vector<std::unique_ptr<Source>>&& getSources() {
        return std::move(sources);
    }

    std::vector<util::ptr<StyleLayer>> getLayers() {
        return layers;
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
    void parseLayers(JSVal value);
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
    Status parseRenderProperty(JSVal value, T &target, const char *name);
    template <typename Parser, typename T>
    Status parseRenderProperty(JSVal value, T &target, const char *name);

    // Parses optional properties into style class properties.
    template <typename T>
    void parseVisibility(StyleBucket &bucket, JSVal value);
    template <typename T>
    Status parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value);
    template <typename T>
    Status parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value, const char *transition_name);
    template <typename T>
    Status setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass);
    template <typename T>
    Status setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass, JSVal transition);

    template <typename T>
    Result<T> parseProperty(JSVal value, const char *property_name);
    template <typename T>
    Result<T> parseProperty(JSVal value, const char *property_name, JSVal transition);

    template <typename T>
    Result<Function<T>> parseFunction(JSVal value, const char *);
    template <typename T>
    Result<PiecewiseConstantFunction<T>> parsePiecewiseConstantFunction(JSVal value, Duration duration);
    template <typename T>
    Result<std::vector<std::pair<float, T>>> parseStops(JSVal value, const char *property_name);

    Result<std::vector<float>> parseFloatArray(JSVal value);

    FilterExpression parseFilter(JSVal);

private:
    std::unordered_map<std::string, const rapidjson::Value *> constants;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<util::ptr<StyleLayer>> layers;

    std::unordered_map<std::string, const Source*> sourcesMap;
    std::unordered_map<std::string, std::pair<JSVal, util::ptr<StyleLayer>>> layersMap;

    // Store a stack of layers we're parsing right now. This is to prevent reference cycles.
    std::forward_list<StyleLayer *> stack;

    // Base URL of the sprite image.
    std::string sprite;

    // URL template for glyph PBFs.
    std::string glyph_url;

    // Obtain default transition duration from map data.
    MapData& data;
};

}

#endif
