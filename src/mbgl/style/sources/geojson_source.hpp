#pragma once

#include <mbgl/style/source.hpp>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {
namespace style {

class GeoJSONSource : public Source {
public:
    GeoJSONSource(std::string id,
                  std::string url,
                  uint16_t tileSize,
                  std::unique_ptr<Tileset>&&,
                  std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&&);

    void load(FileSource&) final;

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;
};

} // namespace style
} // namespace mbgl
