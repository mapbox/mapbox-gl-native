#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
namespace mbgl {
namespace style {
    
  struct CustomVectorSourceOptions : GeoJSONOptions {
        uint8_t minzoom = 0;
        uint16_t tileSize = 512;
    };

    
class CustomVectorSource : public Source {
public:
  CustomVectorSource(std::string id, CustomVectorSourceOptions options, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile);
  
    void setTileData(uint8_t, uint32_t, uint32_t, const mapbox::geojson::geojson&);
    // Private implementation

    class Impl;
    Impl* const impl;
};

template <>
inline bool Source::is<CustomVectorSource>() const {
    return type == SourceType::Vector;
}

} // namespace style
} // namespace mbgl
