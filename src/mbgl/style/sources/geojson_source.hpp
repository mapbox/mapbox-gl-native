#pragma once

#include <mbgl/style/source.hpp>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class AsyncRequest;

namespace style {

class GeoJSONSource : public Source {
public:
    GeoJSONSource(std::string id,
                  std::string url,
                  uint16_t tileSize,
                  std::unique_ptr<Tileset>&&,
                  std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&&);
    ~GeoJSONSource() final;

    void load(FileSource&) final;

private:
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;
    std::unique_ptr<AsyncRequest> req;
};

} // namespace style
} // namespace mbgl
