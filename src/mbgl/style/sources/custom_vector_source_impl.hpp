#pragma once

#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {
namespace style {
    
class CustomVectorSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&, GeoJSONOptions options, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile);
      
    void loadDescription(FileSource&) final {}
    void setTileData(uint8_t, uint32_t, uint32_t, const mapbox::geojson::geojson&);
    void updateTile(uint8_t, uint32_t, uint32_t);

private:
    GeoJSONOptions options;
    std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile;
      
    uint16_t getTileSize() const;
    Range<uint8_t> getZoomRange() final;
    std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
      
};
  
} // namespace style
} // namespace mbgl
