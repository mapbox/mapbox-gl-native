#ifndef MBGL_STYLE_STYLE_PARSER
#define MBGL_STYLE_STYLE_PARSER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <forward_list>

namespace mbgl {

class StyleLayer;
class Source;

class StyleParser {
public:
    ~StyleParser();

    void parse(const JSValue&);

    std::string spriteURL;
    std::string glyphURL;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<StyleLayer>> layers;

private:
    void parseSources(const JSValue&);
    bool parseVectorSource(Source&, const JSValue&);
    bool parseRasterSource(Source&, const JSValue&);
    bool parseGeoJSONSource(Source&, const JSValue&);
    void parseLayers(const JSValue&);
    void parseLayer(const std::string& id, const JSValue&, std::unique_ptr<StyleLayer>&);
    void parseVisibility(StyleLayer&, const JSValue& value);

    std::unordered_map<std::string, const Source*> sourcesMap;
    std::unordered_map<std::string, std::pair<const JSValue&, std::unique_ptr<StyleLayer>>> layersMap;

    // Store a stack of layer IDs we're parsing right now. This is to prevent reference cycles.
    std::forward_list<std::string> stack;
};

} // namespace mbgl

#endif
