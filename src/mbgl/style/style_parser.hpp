#ifndef MBGL_STYLE_STYLE_PARSER
#define MBGL_STYLE_STYLE_PARSER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/source.hpp>

#include <rapidjson/document.h>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <forward_list>

namespace mbgl {

class StyleLayer;
class Source;

using JSVal = rapidjson::Value;

class StyleParser {
public:
    ~StyleParser();

    void parse(const JSVal&);

    std::string spriteURL;
    std::string glyphURL;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<StyleLayer>> layers;

private:
    void parseSources(const JSVal&);
    void parseLayers(const JSVal&);
    void parseLayer(const std::string& id, const JSVal&, std::unique_ptr<StyleLayer>&);
    void parseVisibility(StyleLayer&, const JSVal& value);

    std::unordered_map<std::string, const Source*> sourcesMap;
    std::unordered_map<std::string, std::pair<const JSVal&, std::unique_ptr<StyleLayer>>> layersMap;

    // Store a stack of layer IDs we're parsing right now. This is to prevent reference cycles.
    std::forward_list<std::string> stack;
};

} // namespace mbgl

#endif
