#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/custom_vector_source.hpp>

namespace mbgl {
namespace style {

class CustomVectorSource::Impl : public Source::Impl {
public:
    Impl(std::string id,
         Source&,
         GeoJSONOptions options,
         std::function<void(const CanonicalTileID&)> fetchTile);

    void loadDescription(FileSource&) final {
    }
    void setTileData(const CanonicalTileID& tileID, const mapbox::geojson::geojson&);
    void reloadTile(const CanonicalTileID& tileID);
    void reloadRegion(mbgl::LatLngBounds bounds, uint8_t z);
    void reload();

private:
    uint16_t getTileSize() const;
    Range<uint8_t> getZoomRange() const final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;

private:
    GeoJSONOptions options;
    std::function<void(const CanonicalTileID&)> fetchTile;
};

} // namespace style
} // namespace mbgl
