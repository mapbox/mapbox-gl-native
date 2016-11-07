#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
namespace mbgl {
namespace style {
    
class CustomVectorSource : public Source {
public:
    CustomVectorSource(std::string id, GeoJSONOptions options, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile);
  
    void setTileData(uint8_t, uint32_t, uint32_t, const mapbox::geojson::geojson&);
    void updateTile(uint8_t, uint32_t, uint32_t);

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
