#include <mbgl/style/sources/custom_vector_source_impl.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mapbox/geojsonvt.hpp>

namespace mbgl {
  namespace style {
    
    CustomVectorSource::Impl::Impl(std::string id, Source& base_, CustomVectorSourceOptions options_, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile_)
    : Source::Impl(SourceType::Vector, std::move(id), base_), options(options_), fetchTile(fetchTile_) {
      loaded = true;
    }
    
    Range<uint8_t> CustomVectorSource::Impl::getZoomRange() {
      return { options.minzoom, options.maxzoom };
    }
    
    uint16_t CustomVectorSource::Impl::getTileSize() const {
      return options.tileSize;
    }
    
    std::unique_ptr<Tile> CustomVectorSource::Impl::createTile(const OverscaledTileID& tileID,
                                                          const UpdateParameters& parameters) {
      auto tilePointer = std::make_unique<GeoJSONTile>(tileID, base.getID(), parameters);
      fetchTile(tileID.canonical.z, tileID.canonical.x, tileID.canonical.y);
      return std::move(tilePointer);
    }
    
    void CustomVectorSource::Impl::setTileData(uint8_t z, uint32_t x, uint32_t y, const mapbox::geojson::geojson& geoJSON)
    {
      double scale = util::EXTENT / util::tileSize;
      
      mapbox::geojsonvt::Options vtOptions;
      vtOptions.maxZoom = options.maxzoom;
      vtOptions.extent = util::EXTENT;
      vtOptions.buffer = std::round(scale * 0);//options.buffer
      vtOptions.tolerance = scale * 1;//options.tolerance
      mapbox::geojsonvt::GeoJSONVT geojSONVT = mapbox::geojsonvt::GeoJSONVT(geoJSON, vtOptions);
      
      for (auto const &item : tiles) {
        GeoJSONTile* geoJSONTile = static_cast<GeoJSONTile*>(item.second.get());
        if(geoJSONTile->id.canonical.z == z && geoJSONTile->id.canonical.x == x && geoJSONTile->id.canonical.y == y) {
          geoJSONTile->updateData(geojSONVT.getTile(z, x, y).features);
        }
      }
    }
    
  } // namespace style
} // namespace mbgl
