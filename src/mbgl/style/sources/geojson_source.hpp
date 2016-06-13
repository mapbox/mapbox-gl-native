#pragma once

#include <mbgl/style/source.hpp>

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
};

} // namespace style
} // namespace mbgl
