#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/light.hpp>

#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/geo.hpp>

#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <forward_list>

namespace mbgl {
namespace style {

using StyleParseResult = std::exception_ptr;

class Parser {
public:
    ~Parser();

    StyleParseResult parse(const std::string&);

    std::string spriteURL;
    std::string glyphURL;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<Layer>> layers;

    TransitionOptions transition;
    Light light;

    std::string name;
    LatLng latLng;
    double zoom = 0;
    double bearing = 0;
    double pitch = 0;

    // Statically evaluate layer properties to determine what font stacks are used.
    std::vector<FontStack> fontStacks() const;

private:
    void parseTransition(const JSValue&);
    void parseLight(const JSValue&);
    void parseSources(const JSValue&);
    void parseLayers(const JSValue&);
    void parseLayer(const std::string& id, const JSValue&, std::unique_ptr<Layer>&);

    std::unordered_map<std::string, const Source*> sourcesMap;
    std::unordered_map<std::string, std::pair<const JSValue&, std::unique_ptr<Layer>>> layersMap;

    // Store a stack of layer IDs we're parsing right now. This is to prevent reference cycles.
    std::forward_list<std::string> stack;
};

} // namespace style
} // namespace mbgl
