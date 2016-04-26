#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/filter.hpp>

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/font_stack.hpp>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <forward_list>

namespace mbgl {
namespace style {

std::unique_ptr<Tileset> parseTileJSON(const std::string& json, const std::string& sourceURL, SourceType, uint16_t tileSize);
std::unique_ptr<Tileset> parseTileJSON(const JSValue&);

std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> parseGeoJSON(const JSValue&);

Filter parseFilter(const JSValue&);

class Parser {
public:
    ~Parser();

    void parse(const std::string&);

    std::string spriteURL;
    std::string glyphURL;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<Layer>> layers;

    // Statically evaluate layer properties to determine what font stacks are used.
    std::vector<FontStack> fontStacks() const;

private:
    void parseSources(const JSValue&);
    void parseLayers(const JSValue&);
    void parseLayer(const std::string& id, const JSValue&, std::unique_ptr<Layer>&);
    void parseVisibility(Layer&, const JSValue& value);

    std::unordered_map<std::string, const Source*> sourcesMap;
    std::unordered_map<std::string, std::pair<const JSValue&, std::unique_ptr<Layer>>> layersMap;

    // Store a stack of layer IDs we're parsing right now. This is to prevent reference cycles.
    std::forward_list<std::string> stack;
};

} // namespace style
} // namespace mbgl
