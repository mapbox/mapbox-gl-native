#ifndef MBGL_STYLE_STYLE_PARSER
#define MBGL_STYLE_STYLE_PARSER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/source/source.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <forward_list>

namespace mbgl {

class StyleLayer;
class Source;

class StyleParser {
public:
    ~StyleParser();

    void parse(const std::string&);

    std::string spriteURL;
    std::string glyphURL;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<StyleLayer>> layers;

    // Statically evaluate layer properties to determine what font stacks are used.
    std::vector<std::string> fontStacks() const;

    static std::unique_ptr<SourceInfo> parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType, uint16_t tileSize);
    static std::unique_ptr<SourceInfo> parseTileJSON(const JSValue&);

    static std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> parseGeoJSON(const JSValue&);

private:
    void parseSources(const JSValue&);
    void parseLayers(const JSValue&);
    void parseLayer(util::ID<StyleLayer> id, const JSValue&, std::unique_ptr<StyleLayer>&);
    void parseVisibility(StyleLayer&, const JSValue& value);

    std::map<util::ID<Source>, const Source*> sourcesMap;
    std::map<util::ID<StyleLayer>, std::pair<const JSValue&, std::unique_ptr<StyleLayer>>> layersMap;

    // Store a stack of layer IDs we're parsing right now. This is to prevent reference cycles.
    std::forward_list<util::ID<StyleLayer>> stack;
};

} // namespace mbgl

#endif
