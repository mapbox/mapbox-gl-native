#pragma once

#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {
namespace style {
    
class CustomVectorSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&, GeoJSONOptions options, std::function<void(const CanonicalTileID&)> fetchTile);
      
    void loadDescription(FileSource&) final {}
    void setTileData(const CanonicalTileID& tileID, const mapbox::geojson::geojson&);
    void reloadTile(const CanonicalTileID& tileID);
    void reloadRegion(mbgl::LatLngBounds bounds, uint8_t z);
    void reload();

private:
    GeoJSONOptions options;
    std::function<void(const CanonicalTileID&)> fetchTile;
      
    uint16_t getTileSize() const;
    Range<uint8_t> getZoomRange() final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
      
};
  
} // namespace style
} // namespace mbgl
