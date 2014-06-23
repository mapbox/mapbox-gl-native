#ifndef LLMR_STYLE_STYLE_PARSER
#define LLMR_STYLE_STYLE_PARSER

#include <rapidjson/document.h>
#include <llmr/style/style.hpp>
#include <llmr/map/source.hpp>

#include <unordered_map>
#include <forward_list>


namespace llmr {

class StyleLayer;
typedef std::vector<std::shared_ptr<StyleLayer>> StyleLayerGroup;

class StyleParser {
public:
    using JSVal = const rapidjson::Value&;

    void parse(JSVal document);

    std::shared_ptr<StyleLayerGroup> getLayers() {
        return root;
    }

private:
    void parseConstants(JSVal value);
    JSVal replaceConstant(JSVal value);

    void parseSources(JSVal value);

    std::unique_ptr<StyleLayerGroup> createLayers(JSVal value);
    std::shared_ptr<StyleLayer> createLayer(JSVal value);
    void parseLayers();
    void parseLayer(std::pair<JSVal, std::shared_ptr<StyleLayer>> &pair);
    void parseStyles(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseStyle(JSVal, ClassProperties &properties);
    void parseRasterize(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseReference(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseBucket(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseFilter(JSVal value, std::shared_ptr<StyleLayer> &layer);
    void parseRender(JSVal value, std::shared_ptr<StyleLayer> &layer);

    // Parses optional properties into a render bucket.
    template<typename T>
    bool parseRenderProperty(JSVal value, T &target, const char *name);
    template <typename T, typename Parser>
    bool parseRenderProperty(JSVal value, T &target, const char *name, Parser &parser);

    // Parses optional properties into style class properties.
    template <typename T>
    bool parseStyleProperty(const char *property_name, ClassPropertyKey key, ClassProperties &klass, JSVal value);
    template <typename T>
    bool parseStyleProperty(const char *property_name, const std::vector<ClassPropertyKey> &keys, ClassProperties &klass, JSVal value);

//    PropertyFilterExpression parseFilterOrExpression(JSVal value);
//    Value parseValue(JSVal value);
//

private:
    std::unordered_map<std::string, const rapidjson::Value *> constants;

    std::unordered_map<std::string, const std::shared_ptr<Source>> sources;

    // This stores the root layer.
    std::shared_ptr<StyleLayerGroup> root;

    // This maps ids to Layer objects, with all items being at the root level.
    std::unordered_map<std::string, std::pair<JSVal, std::shared_ptr<StyleLayer>>> layers;

    // Store a stack of layers we're parsing right now. This is to prevent reference cycles.
    std::forward_list<StyleLayer *> stack;
};

}

#endif
