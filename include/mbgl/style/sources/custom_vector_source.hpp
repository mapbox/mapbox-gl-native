#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojson.hpp>

namespace mbgl {
namespace style {

class CustomVectorSource : public Source {
public:
    CustomVectorSource(std::string id,
                       GeoJSONOptions options,
                       std::function<void(const CanonicalTileID&)> fetchTile);

    void setTileData(const CanonicalTileID&, const mapbox::geojson::geojson&);
    void reloadTile(const CanonicalTileID&);
    void reloadRegion(mbgl::LatLngBounds bounds, uint8_t z);
    void reload();

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
