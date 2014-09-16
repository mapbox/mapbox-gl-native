#ifndef MBGL_STYLE_STYLE_SOURCE
#define MBGL_STYLE_STYLE_SOURCE

#include <mbgl/style/types.hpp>
#include <mbgl/util/ptr.hpp>
#include <rapidjson/document.h>

#include <vector>
#include <string>

namespace mbgl {

class Source;

class SourceInfo {
public:
    SourceType type = SourceType::Vector;
    std::string url;
    std::vector<std::string> tiles;
    uint16_t tile_size = 512;
    uint16_t min_zoom = 0;
    uint16_t max_zoom = 22;
    std::string attribution;
    std::array<float, 3> center = {{0, 0, 0}};
    std::array<float, 4> bounds = {{-180, -90, 180, 90}};

    void parseTileJSONProperties(const rapidjson::Value&);
};


class StyleSource : public std::enable_shared_from_this<StyleSource> {
public:
    SourceInfo info;

    bool enabled = false;
    util::ptr<Source> source;

    StyleSource(const SourceInfo &info)
        : info(info)
    {}
};

}

#endif
