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

    std::shared_ptr<StyleLayerGroup> getLayers() {
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
    std::shared_ptr<StyleLayer> createLayer(JSVal value);
    void parseLayers();
    void parseLayer(std::pair<JSVal, std::shared_ptr<StyleLayer>> &pair);
    void parseStyles(JSVal value, std::map<ClassID, ClassProperties> &styles);
    void parseStyle(JSVal, ClassProperties &properties);
    void parseReference(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseBucket(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseRender(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseSprite(JSVal value);
    void parseGlyphURL(JSVal value);

    // Parses optional properties into a render bucket.
    template<typename T>
    bool parseRenderProperty(JSVal value, T &target, const char *name);
    template <typename Parser, typename T>
    bool parseRenderProperty(JSVal value, T &target, const char *name);

    // Parses optional properties into style class properties.
    template <typename T>
    bool parseOptionalProperty(const char *property_name, PropertyKey key, ClassProperties &klass, JSVal value);
    template <typename T>
    bool parseOptionalProperty(const char *property_name, const std::vector<PropertyKey> &keys, ClassProperties &klass, JSVal value);
    template <typename T>
    bool parseOptionalProperty(const char *property_name, T &target, JSVal value);
    template <typename T>
    bool setProperty(JSVal value, const char *property_name, PropertyKey key, ClassProperties &klass);
    template <typename T>
    bool setProperty(JSVal value, const char *property_name, T &target);

    template <typename T>
    std::tuple<bool, T> parseProperty(JSVal value, const char *property_name);

    template <typename T>
    bool parseFunction(PropertyKey key, ClassProperties &klass, JSVal value);
    template <typename T>
    std::tuple<bool, Function<T>> parseFunction(JSVal value);
    template <typename T>
    T parseFunctionArgument(JSVal value);


    FilterExpression parseFilter(JSVal, FilterExpression::Operator op);
    FilterExpression parseFilter(JSVal);
    Value parseValue(JSVal value);
    std::vector<Value> parseValues(JSVal values);

private:
    std::unordered_map<std::string, const rapidjson::Value *> constants;

    std::unordered_map<std::string, const std::shared_ptr<StyleSource>> sources;

    // This stores the root layer.
    std::shared_ptr<StyleLayerGroup> root;

    // This maps ids to Layer objects, with all items being at the root level.
    std::unordered_map<std::string, std::pair<JSVal, std::shared_ptr<StyleLayer>>> layers;

    // Store a stack of layers we're parsing right now. This is to prevent reference cycles.
    std::forward_list<StyleLayer *> stack;

    // Base URL of the sprite image.
    std::string sprite;

    // URL template for glyph PBFs.
    std::string glyph_url;
};

}

#endif
